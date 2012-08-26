/* File: Array.h
Macros for manipulating C arrays.
*/
#ifndef _ATP_COMMON_ARRAY_H_
#define _ATP_COMMON_ARRAY_H_

/* Macro: ARRAYLEN
Calculate the number of elements in a statically allocated array.
*/
#define ARRAYLEN(array) (sizeof(array) / sizeof((array)[0]))

#endif /* _ATP_COMMON_ARRAY_H_ */
