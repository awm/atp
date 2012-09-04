/* File: Array.h
The dynamic array type for ATP.

Important:
    It is not a requirement that every entry in the array be of the same type.
*/
#ifndef _ATP_LIBRARY_ARRAY_H_
#define _ATP_LIBRARY_ARRAY_H_

#include "Export.h"
#include "Value.h"

// forward declaration
struct ATP_ArrayImpl;

/* Type: ATP_Array
Reference to an array implementation.
*/
typedef struct ATP_ArrayImpl *ATP_Array;

// include this after declaring ATP_Array
#include "Dictionary.h"

/* Macro: ARRAYLEN
Calculate the number of elements in a statically allocated array.
*/
#define ARRAYLEN(array) (sizeof(array) / sizeof((array)[0]))

#ifdef __cplusplus
extern "C"
{
#endif

/* Function: ATP_arrayInit
Initialize an array instance.

Parameters:
    p_array - The array handle.
*/
EXPORT void ATP_arrayInit(ATP_Array *p_array);
/* Function: ATP_arrayDestroy
Destroy an array instance.  Remove and free all entries.

Parameters:
    p_array - The array handle.
*/
EXPORT void ATP_arrayDestroy(ATP_Array *p_array);
/* Function: ATP_arrayClear
Remove and free all entries in the list, but leave it initialized.  The state after calling this is the same as just after
calling <ATP_arrayInit> on an uninitialized array.

Parameters:
    p_array - The array handle.
*/
EXPORT void ATP_arrayClear(ATP_Array *p_array);
/* Function: ATP_arrayLength
Count the number of entries in the array.

Parameters:
    p_array - The array handle.

Returns:
    The number of entries.
*/
EXPORT unsigned int ATP_arrayLength(const ATP_Array *p_array);

/* Function: ATP_arrayErase
Erase the array entry at the given index.

Parameters:
    p_array - The array handle.
    p_index - The index of the entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arrayErase(ATP_Array *p_array, unsigned int p_index);
/* Function: ATP_arrayGetType
Get the value type for the given entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the item to get the type of.

Returns:
    The type of the entry.  If the entry was not found, then <e_ATP_ValueType_none> will be returned and an error printed.
*/
EXPORT ATP_ValueType ATP_arrayGetType(const ATP_Array *p_array, unsigned int p_index);

/* Function: ATP_arrayDuplicate
Duplicate an existing array.

Parameters:
    p_array - The array to duplicate.

Returns:
    The new copy of the array.
*/
EXPORT ATP_Array ATP_arrayDuplicate(const ATP_Array *p_array);

/* Function: ATP_arraySetString
Set the value of a given entry to be the provided character string.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetString(ATP_Array *p_array, unsigned int p_index, const char *p_value);
/* Function: ATP_arraySetUint
Set the value of a given entry to be the provided unsigned integer.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetUint(ATP_Array *p_array, unsigned int p_index, unsigned long long p_value);
/* Function: ATP_arraySetInt
Set the value of a given entry to be the provided signed integer.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetInt(ATP_Array *p_array, unsigned int p_index, signed long long p_value);
/* Function: ATP_arraySetDouble
Set the value of a given entry to be the provided floating point value.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetDouble(ATP_Array *p_array, unsigned int p_index, double p_value);
/* Function: ATP_arraySetBool
Set the value of a given entry to be the provided boolean value.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetBool(ATP_Array *p_array, unsigned int p_index, int p_value);
/* Function: ATP_arraySetDict
Set the value of a given entry to be the provided dictionary.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Note:
    The array takes over ownership of the dictionary.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetDict(ATP_Array *p_array, unsigned int p_index, ATP_Dictionary p_value);
/* Function: ATP_arraySetArray
Set the value of a given entry to be the provided array.  The index may be equal to the current value returned by <ATP_arrayLength>,
in which case a new entry will be appended to the array.  If the entry does exist then the old value and type are discarded and replaced with
the new ones.

Note:
    The array takes over ownership of the sub-array.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.  This may be the value returned by <ATP_arrayLength>.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_arraySetArray(ATP_Array *p_array, unsigned int p_index, ATP_Array p_value);

/* Function: ATP_arrayGetString
Get the value of a given string entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.  The string pointed to is only valid while the
              array and its contents remain unchanged.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetString(const ATP_Array *p_array, unsigned int p_index, const char **p_value);
/* Function: ATP_arrayGetUint
Get the value of a given unsigned integer entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetUint(const ATP_Array *p_array, unsigned int p_index, unsigned long long *p_value);
/* Function: ATP_arrayGetInt
Get the value of a given signed integer entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetInt(const ATP_Array *p_array, unsigned int p_index, signed long long *p_value);
/* Function: ATP_arrayGetDouble
Get the value of a given floating point entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetDouble(const ATP_Array *p_array, unsigned int p_index, double *p_value);
/* Function: ATP_arrayGetBool
Get the value of a given boolean entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetBool(const ATP_Array *p_array, unsigned int p_index, int *p_value);
/* Function: ATP_arrayGetDict
Get the value of a given dictionary entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetDict(const ATP_Array *p_array, unsigned int p_index, ATP_Dictionary **p_value);
/* Function: ATP_arrayGetArray
Get the value of a given sub-array entry.

Parameters:
    p_array - The array handle.
    p_index - The index of the array entry to set.
    p_value - The location to store the value from the array entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_arrayGetArray(const ATP_Array *p_array, unsigned int p_index, ATP_Array **p_value);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* _ATP_LIBRARY_ARRAY_H_ */
