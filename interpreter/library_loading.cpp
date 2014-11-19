#include "library_loading.h"

#ifdef _WIN32

#include <windows.h>

void * dataflow::library_load(const std::string& libraryName)
{
    return LoadLibrary((libraryName + ".dll").c_str());
}

void * dataflow::library_procedure(void * library, const std::string& name)
{
    return GetProcAddress(library, name.c_str());
}

bool dataflow::library_free(void * library)
{
    return FreeLibrary(library);
}

#else

#include <dlfcn.h>

#if __APPLE__ && __MACH__
#   define DATAFLOW_LIBRARY_EXTENSION ".dylib"
#else
#   define DATAFLOW_LIBRARY_EXTENSION ".so"
#endif

void * dataflow::library_load(const std::string& libraryName)
{
    return dlopen(("lib" + libraryName + DATAFLOW_LIBRARY_EXTENSION).c_str(), RTLD_LAZY);
}

void * dataflow::library_procedure(void * library, const std::string& name)
{
    return dlsym(library, name.c_str());
}

bool dataflow::library_free(void * library)
{
    return dlclose(library) == 0;
}

#endif