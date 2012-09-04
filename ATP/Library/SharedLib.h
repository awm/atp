/* File: SharedLib.h
Generic interface for loading symbols from shared libraries.
*/
#ifndef _ATP_LIBRARY_SHAREDLIB_H_
#define _ATP_LIBRARY_SHAREDLIB_H_

#include "Export.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Function: ATP_sharedLibLoad
Load a shared library with the given path.

Parameters:
    p_path - The path to the library.

Returns:
    An opaque library reference on success, or NULL if the library could not be loaded.
*/
EXPORT void *ATP_sharedLibLoad(const char *p_path);
/* Function: ATP_sharedLibSymbol
Load a symbol from a shared library.

Parameters:
    p_lib  - The previously loaded shared library.
    p_name - The name of the symbol to load.

Returns:
    A pointer to the symbol, or NULL if the symbol could not be loaded.
*/
EXPORT void *ATP_sharedLibSymbol(void *p_lib, const char *p_symbol);
/* Function: ATP_sharedLibUnload
Unload a shared library.

Parameters:
    p_lib  - The previously loaded shared library.
*/
EXPORT void ATP_sharedLibUnload(void *p_lib);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* _ATP_LIBRARY_SHAREDLIB_H_ */
