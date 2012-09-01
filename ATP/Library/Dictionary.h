/* File: Dictionary.h
The dictionary type for ATP.  This implementation is built on top of uthash.
*/
#ifndef _ATP_LIBRARY_DICTIONARY_H_
#define _ATP_LIBRARY_DICTIONARY_H_

#include "Export.h"

// forward declaration
struct ATP_DictionaryImpl;

/* Constant: c_ATP_Dictionary_keySize
The maximum string length of a dictionary key.
*/
#define c_ATP_Dictionary_keySize    127

/* Enumeration: ATP_ValueType
The data types that a dictionary entry may hold.

Values:
    e_ATP_ValueType_none   - Newly created or recently cleared entry.
    e_ATP_ValueType_string - Variable length string.
    e_ATP_ValueType_uint   - Unsigned integer.
    e_ATP_ValueType_int    - Signed integer.
    e_ATP_ValueType_double - Floating point value.
    e_ATP_ValueType_bool   - Boolean value.
    e_ATP_ValueType_dict   - Sub-dictionary.
    e_ATP_ValueType_array  - Array of other values.
*/
typedef enum ATP_ValueType
{
    e_ATP_ValueType_none,
    e_ATP_ValueType_string,
    e_ATP_ValueType_uint,
    e_ATP_ValueType_int,
    e_ATP_ValueType_double,
    e_ATP_ValueType_bool,
    e_ATP_ValueType_dict,
    e_ATP_ValueType_array
} ATP_ValueType;

/* Type: ATP_Dictionary
Reference to a dictionary implementation.
*/
typedef struct ATP_DictionaryImpl *ATP_Dictionary;

/* Function: ATP_dictionaryInit
Initialize a dictionary instance.

Parameters:
    p_dict - The dictionary handle.
*/
EXPORT void ATP_dictionaryInit(ATP_Dictionary *p_dict);
/* Function: ATP_dictionaryDestroy
Destroy a dictionary instance.  Remove and free all entries.

Parameters:
    p_dict - The dictionary handle.
*/
EXPORT void ATP_dictionaryDestroy(ATP_Dictionary *p_dict);
/* Function: ATP_dictionaryCount
Count the number of entries in the dictionary.

Parameters:
    p_dict - The dictionary handle.

Returns:
    The number of entries.
*/
EXPORT unsigned int ATP_dictionaryCount(ATP_Dictionary *p_dict);

/* Function: ATP_dictionaryDuplicate
Duplicate an existing dictionary.

Parameters:
    p_dict - The dictionary to duplicate.

Returns:
    The new copy of the dictionary.
*/
EXPORT ATP_Dictionary ATP_dictionaryDuplicate(ATP_Dictionary *p_dict);

/* Function: ATP_dictionaryRemove
Remove an entry from the dictionary.

Parameters:
    p_dict - The dictionary handle.
    p_key  - The key of the dictionary entry to remove.
*/
EXPORT void ATP_dictionaryRemove(ATP_Dictionary *p_dict, const char *p_key);

/* Function: ATP_dictionarySetUint
Set the value of a given entry to be the provided unsigned integer value.  The entry is created if it does not exist.
If it does exist then the old value and type are discarded and replaced with the new ones.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to set.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_dictionarySetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long p_value);
/* Function: ATP_dictionarySetInt
Set the value of a given entry to be the provided signed integer value.  The entry is created if it does not exist.
If it does exist then the old value and type are discarded and replaced with the new ones.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to set.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_dictionarySetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long p_value);
/* Function: ATP_dictionarySetDouble
Set the value of a given entry to be the provided floating point value.  The entry is created if it does not exist.
If it does exist then the old value and type are discarded and replaced with the new ones.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to set.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_dictionarySetDouble(ATP_Dictionary *p_dict, const char *p_key, double p_value);
/* Function: ATP_dictionarySetBool
Set the value of a given entry to be the provided boolean value.  The entry is created if it does not exist.
If it does exist then the old value and type are discarded and replaced with the new ones.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to set.
    p_value - The value to store in the dictionary entry.

Returns:
    1 on success, 0 on failure.
*/
EXPORT int ATP_dictionarySetBool(ATP_Dictionary *p_dict, const char *p_key, int p_value);

/* Function: ATP_dictionaryGetUint
Get the value of a given unsigned integer entry.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to get.
    p_value - The location to store the value from the dictionary entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_dictionaryGetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long *p_value);
/* Function: ATP_dictionaryGetInt
Get the value of a given signed integer entry.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to get.
    p_value - The location to store the value from the dictionary entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_dictionaryGetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long *p_value);
/* Function: ATP_dictionaryGetDouble
Get the value of a given floating point entry.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to get.
    p_value - The location to store the value from the dictionary entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_dictionaryGetDouble(ATP_Dictionary *p_dict, const char *p_key, double *p_value);
/* Function: ATP_dictionaryGetBool
Get the value of a given boolean entry.

Parameters:
    p_dict  - The dictionary handle.
    p_key   - The key of the dictionary entry to get.
    p_value - The location to store the value from the dictionary entry in.

Returns:
    1 on success, 0 on failure.  In particular, if the entry does not exist or is of the wrong type then 0 will be returned.
*/
EXPORT int ATP_dictionaryGetBool(ATP_Dictionary *p_dict, const char *p_key, int *p_value);

#endif /* _ATP_LIBRARY_DICTIONARY_H_ */
