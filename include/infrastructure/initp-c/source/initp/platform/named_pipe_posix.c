#include <initp/system/debug.h>
#include <initp/platform/named_pipe.h>

#ifdef SYSTEM_TRACE_NAMED_PIPE
#include <initp/tools/format.h>
#endif // SYSTEM_TRACE_NAMED_PIPE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>

uint8_t sys_init_named_pipe(sys_named_pipe_tp object, const char* path) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_named_pipe", "Named pipe pointer is null");
        return 0;
    }

    size_t length = strlen(path);
    if (length >= 0x80) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_named_pipe", "Path too long");
        return 0;
    }

    strcpy(object->path, path);
    object->fd_r = 0;
    object->fd_w = 0;
    object->error = 0;
    object->is_server = 0;
    object->connected = 0;
    object->on_connect = 0;
    object->on_connect_eparam = 0;
    object->on_disconnect = 0;
    object->on_disconnect_eparam = 0;
    return 1;
}

uint8_t sys_create_named_pipe(sys_named_pipe_tp object) {
    char path_r[0x100];
    sprintf(path_r, "/tmp/%s_r", object->path);
    if (mkfifo(path_r, 0666) != 0) {
        int error = errno;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_create_named_pipe", "Error %d from mkfifo. %s", error, strerror(error));
        if (error != EEXIST) return 0;
    }
    char path_w[0x100];
    sprintf(path_w, "/tmp/%s_w", object->path);
    if (mkfifo(path_w, 0666) != 0) {
        int error = errno;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_create_named_pipe", "Error %d from mkfifo. %s", error, strerror(error));
        if (error != EEXIST) {
            unlink(path_r);
            return 0;
        }
    }
    object->is_server = 1;
    return 1;
}

uint8_t sys_open_named_pipe(sys_named_pipe_tp object, uint8_t flush) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_open_named_pipe", "Named pipe pointer is null");
        return 0;
    }

    sys_close_named_pipe(object);

    char path_r[0x100];
    sprintf(path_r, object->is_server ? "/tmp/%s_r" : "/tmp/%s_w", object->path);
    object->fd_r = open(path_r, O_RDWR | O_NONBLOCK);
    if (object->fd_r == -1) {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_named_pipe", "Error %d from open. %s", error, strerror(error));
        }
        object->fd_r = 0;
        return 0;
    }

    object->flush = flush;
    if (flush) {
        const int size = 0x400;
        char tmp[size];
        for (int i = 0; i < 10 && read(object->fd_r, tmp, size) > 0;);
    }

    char path_w[0x100];
    sprintf(path_w, object->is_server ? "/tmp/%s_w" : "/tmp/%s_r", object->path);
    object->fd_w = open(path_w, O_RDWR | O_NONBLOCK);
    if (object->fd_w == -1) {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_named_pipe", "Error %d from open. %s", error, strerror(error));
        }
        close(object->fd_r);
        object->fd_r = 0;
        object->fd_w = 0;
        return 0;
    }

    if (object->error != 0) {
        object->error = 0;
    }
    if (object->on_connect) {
        object->on_connect(object->on_connect_eparam);
    }
    return 1;
}

void sys_close_named_pipe(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_close_named_pipe", "Named pipe pointer is null");
        return;
    }

    if (object->fd_r > 0) {
        close(object->fd_r);
        object->fd_r = 0;
    }
    if (object->fd_w > 0) {
        close(object->fd_w);
        object->fd_w = 0;
    }
    if (object->on_disconnect) {
        object->on_disconnect(object->on_disconnect_eparam);
    }
}

void sys_delete_named_pipe(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_delete_named_pipe", "Named pipe pointer is null");
        return;
    }

    /*char path[0x47];
    sprintf(path, "/tmp/%s_r", object->path);
    unlink(path);
    sprintf(path, "/tmp/%s_w", object->path);
    unlink(path);*/
}

uint8_t sys_named_pipe_opened(sys_named_pipe_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_named_pipe_opened", "Named pipe pointer is null");
        return 0;
    }

    return object->fd_r ? 1 : 0;
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
    if (!object->fd_r) return 0;

    struct pollfd fd;
    fd.fd = object->fd_r;
    fd.events = POLLIN | POLLERR | POLLHUP;
    fd.revents = 0;

    int result = poll(&fd, 1, 0);
    if (result == 0) {
        if (object->error != 0)
            object->error = 0;
        return 0;
    } else if (result < 0) {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Error %d from poll. %s", error, strerror(error));
        }
        return 0;
    } else if (!(fd.revents & POLLIN)) {
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "sys_read_named_pipe", "Events: %d", (int)fd.revents);
        return 0;
    }

    int n = read(object->fd_r, data, size);
    if (n >= 0) {
        if (object->error != 0) {
            object->error = 0;
        }
    } else {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_named_pipe", "Error %d from read. %s", error, strerror(error));
        }
        return 0;
    }

    #ifdef SYSTEM_TRACE_NAMED_PIPE
    if (n > 0) {
        sys_trace_array("sys_read_named_pipe", data, n);
    }
    #endif // SYSTEM_TRACE_NAMED_PIPE

    return (size_t)n;
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
    if (!object->fd_w) {
        sys_open_named_pipe(object, object->flush);
        return length;
    }

    struct pollfd fd;
    fd.fd = object->fd_w;
    fd.events = POLLOUT;
    fd.revents = 0;

    int result = poll(&fd, 1, 0);
    if (result == 0) {
        if (object->error != 0)
            object->error = 0;
        return length;
    } else if (result < 0) {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "Error %d from poll. %s", error, strerror(error));
        }
        return length;
    } else if (!(fd.revents & POLLOUT)) {
        return length;
    }

    int n = write(object->fd_w, data, length);
    if (n >= 0) {
        if (object->error != 0) {
            object->error = 0;
        }
    } else {
        int error = errno;
        if (object->error != error) {
            object->error = error;
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_named_pipe", "Error %d from write. %s", error, strerror(error));
        }
        sys_close_named_pipe(object);
        sys_open_named_pipe(object, object->flush);
        return 0;
    }

    #ifdef SYSTEM_TRACE_NAMED_PIPE
    if (n > 0) {
        sys_trace_array("sys_write_named_pipe", data, n);
    }
    #endif // SYSTEM_TRACE_NAMED_PIPE

    return (size_t)n;
}
