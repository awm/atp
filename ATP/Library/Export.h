/* File: Export.h
Macros for setting the visiblility of functions.
*/
#ifndef _ATP_COMMON_EXPORT_H_
#define _ATP_COMMON_EXPORT_H_

/* Macro: EXPORT
Declare that a function should be exported from the module into which it was linked.
*/
#ifdef __GNUC__
    #ifdef _WIN32
        #define EXPORT __declspec(dllexport) __attribute__((visibility("default")))
    #else
        #define EXPORT __attribute__((visibility("default")))
    #endif
#else
    #define EXPORT
#endif

#endif /* _ATP_COMMON_EXPORT_H_ */
