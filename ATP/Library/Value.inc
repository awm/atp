/* File: Value.inc
Internal interface to the internal value type used by the dictionary and array implementations.
*/
#ifndef _ATP_LIBRARY_VALUE_INC_
#define _ATP_LIBRARY_VALUE_INC_

#include "Value.h"
#include "Dictionary.h"
#include "Array.h"

#include "ATP/ThirdParty/UT/utstring.h"

/* Structure: Value
Represents a value of variable type.
*/
typedef struct Value
{
    /* Variable: m_type
    The type identifier of the value.
    */
    ATP_ValueType m_type;
    /* Variable: m_value
    The value, the active member of which is determined by <m_type>.
    */
    union
    {
        UT_string m_string;
        unsigned long long m_uint;
        signed long long m_int;
        double m_double;
        int m_bool;
        ATP_Dictionary m_dict;
        ATP_Array m_array;
    } m_value;
} Value;

/* Function: Value_changeType
Change the type of the value, deleting any old data contained in it and initializing it to the new type.

Parameters:
    p_value   - The value to change the type of.
    p_newType - The type to switch to.
*/
void Value_changeType(Value *p_value, ATP_ValueType p_newType);
/* Function: Value_copy
Copy one value into another.  Both instances must be initialized.

Parameters:
    p_dest   - The value instance to copy into.
    p_source - The value instance to make a copy of.
*/
void Value_copy(Value *p_dest, const Value *p_source);

#endif /* _ATP_LIBRARY_VALUE_INC_ */
