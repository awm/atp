#ifndef _ATP_COMMON_DICTIONARY_H_
#define _ATP_COMMON_DICTIONARY_H_

#include "Export.h"

#define c_ATP_Dictionary_keySize    127

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

struct ATP_DictionaryImpl;
typedef struct ATP_DictionaryImpl *ATP_Dictionary;

EXPORT void ATP_dictionaryInit(ATP_Dictionary *p_dict);
EXPORT void ATP_dictionaryDestroy(ATP_Dictionary *p_dict);
EXPORT unsigned int ATP_dictionaryCount(ATP_Dictionary *p_dict);

EXPORT ATP_Dictionary ATP_dictionaryDuplicate(ATP_Dictionary *p_dict);

EXPORT void ATP_dictionaryRemove(ATP_Dictionary *p_dict, const char *p_key);

EXPORT int ATP_dictionarySetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long p_value);
EXPORT int ATP_dictionarySetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long p_value);
EXPORT int ATP_dictionarySetDouble(ATP_Dictionary *p_dict, const char *p_key, double p_value);
EXPORT int ATP_dictionarySetBool(ATP_Dictionary *p_dict, const char *p_key, int p_value);

EXPORT int ATP_dictionaryGetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long *p_value);
EXPORT int ATP_dictionaryGetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long *p_value);
EXPORT int ATP_dictionaryGetDouble(ATP_Dictionary *p_dict, const char *p_key, double *p_value);
EXPORT int ATP_dictionaryGetBool(ATP_Dictionary *p_dict, const char *p_key, int *p_value);

#endif /* _ATP_COMMON_DICTIONARY_H_ */
