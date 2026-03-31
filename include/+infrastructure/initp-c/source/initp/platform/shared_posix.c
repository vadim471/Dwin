#include <initp/platform/shared.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

const char* sys_default_module_extention = "so";
const char* sys_default_module_prefix = "";

void* sys_load_module(const char* path) {
    return dlopen(path, RTLD_LAZY);
}

void* sys_get_address(void* handle, const char* name) {
    return dlsym(handle, name);
}

void sys_free_module(void* handle) {
    dlclose(handle);
}

void sys_global_free(void* address) {
    free(address);
}
