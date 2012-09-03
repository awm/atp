/* File: Value.h
External interface to the internal value type used by the dictionary and array implementations.
*/
#ifndef _ATP_LIBRARY_VALUE_H_
#define _ATP_LIBRARY_VALUE_H_

#include "Export.h"

/* Enumeration: ATP_ValueType
The data types that a dictionary or array entry may hold.

Values:
    e_ATP_ValueType_none   - Newly created or recently cleared entry.
    e_ATP_ValueType_string - Variable length string.
    e_ATP_ValueType_uint   - Unsigned integer.
    e_ATP_ValueType_int    - Signed integer.
    e_ATP_ValueType_double - Floating point value.
    e_ATP_ValueType_bool   - Boolean value.
    e_ATP_ValueType_dict   - Dictionary.
    e_ATP_ValueType_array  - Array.
*/
typedef enum ATP_ValueType
{
    e_ATP_ValueType_none = 0,
    e_ATP_ValueType_string,
    e_ATP_ValueType_uint,
    e_ATP_ValueType_int,
    e_ATP_ValueType_double,
    e_ATP_ValueType_bool,
    e_ATP_ValueType_dict,
    e_ATP_ValueType_array
} ATP_ValueType;

/* Function: ATP_valueTypeToString
Convert a value type into a human readable name.

Parameters:
    p_type - The type enum value.

Returns:
    The corresponding name.
*/
EXPORT const char *ATP_valueTypeToString(ATP_ValueType p_type);


#endif /* _ATP_LIBRARY_VALUE_H_ */
