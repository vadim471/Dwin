#include <initp/platform/shared.h>
#include <initp/system/debug.h>

#include <windows.h>

const char* sys_default_module_extention = "dll";
const char* sys_default_module_prefix = "_";

void* sys_load_module(const char* path) {
    void* handle = (void*)LoadLibrary(path);
    if (!handle) {
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "sys_load_module", "LoadLibrary failed with %d", (int)GetLastError());
    }
    return handle;
}

void* sys_get_address(void* handle, const char* name) {
    return (void*)GetProcAddress(handle, name);
}

void sys_free_module(void* handle) {
    FreeLibrary(handle);
}

void sys_global_free(void* address) {
    GlobalFree((HGLOBAL)address);
}
