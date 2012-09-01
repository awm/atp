#include "SharedLib.h"

#include <stdlib.h>
#if _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef WIN32_LEAN_AND_MEAN
#else
    #include <dlfcn.h>
#endif

void *ATP_sharedLibLoad(const char *p_path)
{
#if _WIN32
    return (void *) LoadLibrary(p_path);
#else // NOTE: assume POSIX for now
    return dlopen(p_path, RTLD_LAZY|RTLD_LOCAL);
#endif
}

void *ATP_sharedLibSymbol(void *p_lib, const char *p_symbol)
{
#if _WIN32
    return (void *) GetProcAddress(p_lib, p_symbol);
#else // NOTE: assume POSIX for now
    return dlsym(p_lib, p_symbol);
#endif
}

void ATP_sharedLibUnload(void *p_lib)
{
#if _WIN32
    FreeLibrary(p_lib);
#else // NOTE: assume POSIX for now
    dlclose(p_lib);
#endif
}
