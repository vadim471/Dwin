#include <initp/system/debug.h>
#include <initp/platform/named_pipe.h>

#ifdef SYSTEM_TRACE_NAMED_PIPE
#include <initp/tools/format.h>
#endif // SYSTEM_TRACE_NAMED_PIPE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static BOOL ConnectToNewClient(sys_named_pipe_tp object) {

    if (object->connected) return TRUE;
    if (object->fPending) return TRUE;

    // Start an overlapped connection for this pipe instance.
    BOOL fConnected = ConnectNamedPipe(object->handle, &object->oOverlap);

    // Overlapped ConnectNamedPipe should return zero.
    if (fConnected) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "ConnectToNewClient", "ConnectNamedPipe failed with 0x%08lx", GetLastError());
        return FALSE;
    }

    switch (GetLastError()) {

        // The overlapped connection in progress.
        case ERROR_IO_PENDING:
            object->fPending = TRUE;
            return TRUE;

        // Client is already connected, so signal an event.
        case ERROR_PIPE_CONNECTED:
            if (SetEvent(object->oOverlap.hEvent)) {
                object->connected = TRUE;
                if (object->on_connect) {
                    object->on_connect(object->on_connect_eparam);
                }
            }
            return TRUE;

        // If an error occurs during the connect operation...
        default:
            sys_debug_print(SYSTEM_LEVEL_ERROR, "ConnectToNewClient", "ConnectNamedPipe failed with 0x%08lx", GetLastError());
            return FALSE;
    }
}

static BOOL error_was_print = FALSE;

static BOOL ConnectToServer(sys_named_pipe_tp object) {

    if (object->connected) return TRUE;

    HANDLE hPipe = CreateFileA(
        object->path,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,//FILE_FLAG_OVERLAPPED,
        NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        DWORD dwErr = GetLastError();
        if (!error_was_print) {
            error_was_print = TRUE;
            if (dwErr == ERROR_PIPE_BUSY) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "ConnectToServer", "All pipe slots are busy 0x%08lx", dwErr);
            } else if (dwErr == ERROR_FILE_NOT_FOUND) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "ConnectToServer", "No pipe with name %s found", object->path);
            } else {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "ConnectToServer", "CreateFileA failed with 0x%08lx", dwErr);
            }
        }
        return FALSE;
    } else {
        error_was_print = FALSE;
        sys_debug_print(SYSTEM_LEVEL_INFO, "ConnectToServer", "CreateFileA success");
    }
    object->connected = TRUE;
    object->handle = hPipe;
    if (object->on_connect) {
        object->on_connect(object->on_connect_eparam);
    }
    return TRUE;
}

static BOOL DisconnectAndReconnect(sys_named_pipe_tp object) {
    sys_close_named_pipe(object);
    if (object->is_server) {
        return ConnectToNewClient(object);
    } else {
        return ConnectToServer(object);
    }
}

static BOOL PollNamedPipe(sys_named_pipe_tp object) {

    DWORD cbRet;
    BOOL fSuccess;

    DWORD dwWait = WaitForSingleObject(
        object->hEvent,     // event object
        0                   // don't wait
    );

    switch (dwWait) {
        case WAIT_OBJECT_0:
            if (object->fPending) {
                object->fPending = FALSE;

                fSuccess = GetOverlappedResult(
                    object->handle,         // handle to pipe
                    &object->oOverlap,      // OVERLAPPED structure
                    &cbRet,                 // bytes transferred
                    FALSE                   // do not wait
                );

                if (!object->connected) {

                    // Pending connect operation
                    if (!fSuccess) {
                        sys_debug_print(SYSTEM_LEVEL_ERROR, "PollNamedPipe", "Operation failed with 0x%08lx", GetLastError());
                        DisconnectAndReconnect(object);
                        return FALSE;
                    }
                    object->connected = TRUE;
                    if (object->on_connect) {
                        object->on_connect(object->on_connect_eparam);
                    }
                } else {

                    // Pending read operation
                    if (!fSuccess || cbRet == 0) {
                        DisconnectAndReconnect(object);
                        return FALSE;
                    }
                }
            }
            break;
        case WAIT_TIMEOUT:
            break;
        default:
            sys_debug_print(SYSTEM_LEVEL_ERROR, "PollNamedPipe", "WaitForSingleObject failed with 0x%08lx", GetLastError());
            return FALSE;
    }
    return TRUE;
}

uint8_t sys_init_named_pipe(sys_named_pipe_tp object, const char* path) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_named_pipe", "Named pipe pointer is null");
        return 0;
    }

    const char* prefix = "\\\\.\\pipe\\";
    size_t length = strlen(path);
    size_t prefix_length = length;
    if (length >= 0x80 - prefix_length) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_named_pipe", "Path too long");
        return 0;
    }
    sprintf(object->path, "%s%s", prefix, path);

    object->handle = NULL;
    object->hEvent = NULL;
    object->fPending = FALSE;
    object->flush = FALSE;
    object->is_server = FALSE;
    object->connected = FALSE;
    object->on_connect = NULL;
    object->on_connect_eparam = 0;
    object->on_disconnect = NULL;
    object->on_disconnect_eparam = 0;

    object->hEvent = CreateEvent(
        NULL,    // default security attribute
        TRUE,    // manual-reset event
        TRUE,    // initial state = signaled
        NULL);   // unnamed event object
    if (object->hEvent == NULL) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_named_pipe", "CreateEvent (read) failed with 0x%08lx", GetLastError());
        return 0;
    }
    object->oOverlap.hEvent = object->hEvent;
    return 1;
}

uint8_t sys_create_named_pipe(sys_named_pipe_tp object) {

    if (!object->handle) {
        object->handle = CreateNamedPipeA(
            object->path,            // pipe name
            PIPE_ACCESS_DUPLEX |     // read/write access
            FILE_FLAG_OVERLAPPED,    // overlapped mode
            //PIPE_TYPE_MESSAGE |      // message-type pipe
            //PIPE_READMODE_MESSAGE |  // message-read mode
            PIPE_WAIT,               // blocking mode
            1,                       // number of instances
            NAMEDPIPE_BUFSIZE*sizeof(TCHAR),// output buffer size
            NAMEDPIPE_BUFSIZE*sizeof(TCHAR),// input buffer size
            INFINITE,                // client time-out
            NULL);                   // default security attributes
    }

    if (object->handle == INVALID_HANDLE_VALUE) {
        object->handle = NULL;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_create_named_pipe", "CreateNamedPipe failed with 0x%08lx", GetLastError());
        return 0;
    }
    object->is_server = TRUE;
    return 1;
}

uint8_t sys_open_named_pipe(sys_named_pipe_tp object, uint8_t flush) {
    object->flush = (flush > 0) ? TRUE : FALSE;
    if (object->is_server) {
        // Call the subroutine to connect to the new client
        return ConnectToNewClient(object) ? 1 : 0;
    } else {
        return ConnectToServer(object) ? 1 : 0;
    }
}

void sys_close_named_pipe(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_close_named_pipe", "Named pipe pointer is null");
        return;
    }

    if (object->handle) {

        if (object->fPending) {
            CancelIoEx(object->handle, &object->oOverlap);
        }

        FlushFileBuffers(object->handle);

        if (object->is_server) {
            if (!DisconnectNamedPipe(object->handle)) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_close_named_pipe", "DisconnectNamedPipe failed with 0x%08lx", GetLastError());
            }
        } else {
            if (!CloseHandle(object->handle)) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_close_named_pipe", "CloseHandle failed with 0x%08lx", GetLastError());
            }
            object->handle = NULL;
        }
    }

    object->connected = FALSE;
    object->fPending = FALSE;
    if (object->on_disconnect) {
        object->on_disconnect(object->on_disconnect_eparam);
    }
}

void sys_delete_named_pipe(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_delete_named_pipe", "Named pipe pointer is null");
        return;
    }

    if (object->is_server && object->handle) {
        if (!CloseHandle(object->handle)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_delete_named_pipe", "CloseHandle failed with 0x%08lx", GetLastError());
        }
        object->handle = NULL;
    }
}

uint8_t sys_named_pipe_opened(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_named_pipe_opened", "Named pipe pointer is null");
        return 0;
    }

    return object->connected ? 1 : 0;
}

size_t sys_read_named_pipe(sys_named_pipe_tp object, char* data, size_t size) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_read_named_pipe", "Named pipe pointer is null");
        return 0;
    }
    if (!data) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_read_named_pipe", "Data pointer is null");
        return 0;
    }
    if (size < 1) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_read_named_pipe", "Data size is null");
        return 0;
    }

    PollNamedPipe(object);
    if (!object->connected) {
        if (!sys_open_named_pipe(object, object->flush ? 1 : 0)) {
            return 0;
        }
    }

    DWORD total = 0;
    if (PeekNamedPipe(object->handle, NULL, 0, NULL, &total, NULL)) {
        if (error_was_print)
            error_was_print = FALSE;
        if (total > 0) {
            if (total > size) total = size;
            if (ReadFile(object->handle, data, total, &total, NULL)) {
                if (error_was_print)
                    error_was_print = FALSE;
                #ifdef SYSTEM_TRACE_NAMED_PIPE
                sys_trace_array("sys_read_named_pipe", data, total);
                #endif // SYSTEM_TRACE_NAMED_PIPE
                return (size_t)total;
            } else {
                if (!error_was_print) {
                    error_was_print = TRUE;
                    DWORD error = GetLastError();
                    if (error == ERROR_PIPE_NOT_CONNECTED) {
                        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Named pipe %s disconnected", object->path);
                    } else if (error == ERROR_BAD_PIPE) {
                        //sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Bad named pipe %s", object->path);
                    } else if (error == ERROR_BROKEN_PIPE) {
                        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Named pipe %s broken", object->path);
                    } else {
                        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "ReadFile failed with 0x%08lx", error);
                    }
                }
                DisconnectAndReconnect(object);
            }
        }
    } else {
        if (!error_was_print) {
            error_was_print = TRUE;
            DWORD error = GetLastError();
            if (error == ERROR_PIPE_NOT_CONNECTED) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Named pipe %s disconnected", object->path);
            } else if (error == ERROR_BAD_PIPE) {
                //sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Bad named pipe %s", object->path);
            } else if (error == ERROR_BROKEN_PIPE) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Named pipe %s broken", object->path);
            } else {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "PeekNamedPipe failed with 0x%08lx", error);
            }
        }
        DisconnectAndReconnect(object);
    }
    return 0;
}

size_t sys_write_named_pipe(sys_named_pipe_tp object, const char* data, size_t length) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_write_named_pipe", "Named pipe pointer is null");
        return 0;
    }
    if (!data) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_write_named_pipe", "Data pointer is null");
        return 0;
    }
    if (length < 1) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_write_named_pipe", "Data length is null");
        return 0;
    }

    PollNamedPipe(object);
    if (!object->connected) return length;

    DWORD dwWrite = 0;
    if (WriteFile(object->handle, data, length, &dwWrite, 0)) {
        if (error_was_print)
            error_was_print = FALSE;
        #ifdef SYSTEM_TRACE_NAMED_PIPE
        sys_trace_array("sys_write_named_pipe", data, dwWrite);
        #endif // SYSTEM_TRACE_NAMED_PIPE
        return (size_t)dwWrite;
    } else {
        if (!error_was_print) {
            error_was_print = TRUE;
            DWORD error = GetLastError();
            if (error == ERROR_PIPE_NOT_CONNECTED) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "Named pipe %s disconnected", object->path);
            } else if (error == ERROR_BAD_PIPE) {
                //sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "Bad named pipe %s", object->path);
            } else if (error == ERROR_BROKEN_PIPE) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "Named pipe %s broken", object->path);
            } else {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "WriteFile failed with 0x%08lx", error);
            }
        }
        DisconnectAndReconnect(object);
    }
    return 0;
}
