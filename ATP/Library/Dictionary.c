#include "Dictionary.h"
#include "Log.h"
#include "Exit.h"

#include <stdio.h>
//#include "ATP/ThirdParty/UT/utstring.h"
//#include "ATP/ThirdParty/UT/utarray.h"
#include "ATP/ThirdParty/UT/uthash.h"

typedef struct Value
{
    ATP_ValueType m_type;
    union
    {
        //UT_string m_string;
        unsigned long long m_uint;
        signed long long m_int;
        double m_double;
        int m_bool;
        //struct ATP_DictionaryEntry *m_dict;
        //UT_array *m_array;
    } m_value;
} Value;

typedef struct ATP_DictionaryImpl
{
    char m_key[c_ATP_Dictionary_keySize + 1];
    Value m_value;
    UT_hash_handle hh;
} ATP_DictionaryImpl;

void ATP_dictionaryInit(ATP_Dictionary *p_dict)
{
    *p_dict = NULL;
}

static void changeValueType(ATP_DictionaryImpl *p_entry, ATP_ValueType p_newType)
{
    if (p_entry->m_value.m_type != p_newType)
    {
        switch (p_entry->m_value.m_type)
        {
            // TODO: add support for other types
            default:
                break;
        }
    }
}

void ATP_dictionaryDestroy(ATP_Dictionary *p_dict)
{
    if (p_dict != NULL)
    {
        ATP_DictionaryImpl *it;
        ATP_DictionaryImpl *l_tmp;

        HASH_ITER(hh, *p_dict, it, l_tmp)
        {
            HASH_DEL(*p_dict, it);
            changeValueType(it, e_ATP_ValueType_none);
            free(it);
        }

        *p_dict = NULL;
    }
}

void ATP_dictionaryRemove(ATP_Dictionary *p_dict, const char *p_key)
{
    ATP_DictionaryImpl *l_entry = NULL;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry != NULL)
    {
        // remove existing entry
        HASH_DEL(*p_dict, l_entry);
        changeValueType(l_entry, e_ATP_ValueType_none);
        free(l_entry);
    }
}

unsigned int ATP_dictionaryCount(ATP_Dictionary *p_dict)
{
    return HASH_COUNT(*p_dict);
}

static ATP_DictionaryImpl *createEntry(const char *p_key)
{
    ATP_DictionaryImpl *l_entry;
    if (strlen(p_key) > c_ATP_Dictionary_keySize)
    {
        ERR("Key '%s' is too long (max. %u characters)\n", p_key, c_ATP_Dictionary_keySize);
        return NULL;
    }

    l_entry = malloc(sizeof(ATP_DictionaryImpl));
    if (l_entry == NULL)
    {
        PERR();
        exit(EX_OSERR);
    }

    strncpy(l_entry->m_key, p_key, c_ATP_Dictionary_keySize + 1);
    l_entry->m_value.m_type = e_ATP_ValueType_none;
    return l_entry;
}

ATP_Dictionary ATP_dictionaryDuplicate(ATP_Dictionary *p_dict)
{
    if (p_dict != NULL)
    {
        ATP_DictionaryImpl *it;
        ATP_DictionaryImpl *l_tmp;
        ATP_Dictionary l_result;

        ATP_dictionaryInit(&l_result);
        HASH_ITER(hh, *p_dict, it, l_tmp)
        {
            ATP_DictionaryImpl *l_copy = createEntry(it->m_key);
            if (l_copy == NULL)
            {
                ATP_dictionaryDestroy(&l_result);
                return NULL;
            }

            changeValueType(l_copy, it->m_value.m_type);
            switch (l_copy->m_value.m_type)
            {
                // TODO: add support for other types
                case e_ATP_ValueType_uint:
                    l_copy->m_value.m_value.m_uint = it->m_value.m_value.m_uint;
                    break;
                case e_ATP_ValueType_int:
                    l_copy->m_value.m_value.m_int = it->m_value.m_value.m_int;
                    break;
                case e_ATP_ValueType_double:
                    l_copy->m_value.m_value.m_double = it->m_value.m_value.m_double;
                    break;
                case e_ATP_ValueType_bool:
                    l_copy->m_value.m_value.m_bool = it->m_value.m_value.m_bool;
                    break;
                default:
                    break;
            }

            HASH_ADD_STR(l_result, m_key, l_copy);
        }

        return l_result;
    }

    return NULL;
}

int ATP_dictionarySetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        l_entry = createEntry(p_key);
        if (l_entry == NULL)
        {
            return 0;
        }
    }

    changeValueType(l_entry, e_ATP_ValueType_uint);
    l_entry->m_value.m_value.m_uint = p_value;
    HASH_ADD_STR(*p_dict, m_key, l_entry);
    return 1;
}

int ATP_dictionarySetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        l_entry = createEntry(p_key);
        if (l_entry == NULL)
        {
            return 0;
        }
    }

    changeValueType(l_entry, e_ATP_ValueType_int);
    l_entry->m_value.m_value.m_int = p_value;
    HASH_ADD_STR(*p_dict, m_key, l_entry);
    return 1;
}

int ATP_dictionarySetDouble(ATP_Dictionary *p_dict, const char *p_key, double p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        l_entry = createEntry(p_key);
        if (l_entry == NULL)
        {
            return 0;
        }
    }

    changeValueType(l_entry, e_ATP_ValueType_double);
    l_entry->m_value.m_value.m_double = p_value;
    HASH_ADD_STR(*p_dict, m_key, l_entry);
    return 1;
}

int ATP_dictionarySetBool(ATP_Dictionary *p_dict, const char *p_key, int p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        l_entry = createEntry(p_key);
        if (l_entry == NULL)
        {
            return 0;
        }
    }

    changeValueType(l_entry, e_ATP_ValueType_bool);
    l_entry->m_value.m_value.m_bool = p_value;
    HASH_ADD_STR(*p_dict, m_key, l_entry);
    return 1;
}

int ATP_dictionaryGetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    if (l_entry->m_value.m_type == e_ATP_ValueType_uint)
    {
        *p_value = l_entry->m_value.m_value.m_uint;
        return 1;
    }

    return 0;
}

int ATP_dictionaryGetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    if (l_entry->m_value.m_type == e_ATP_ValueType_int)
    {
        *p_value = l_entry->m_value.m_value.m_int;
        return 1;
    }

    return 0;
}

int ATP_dictionaryGetDouble(ATP_Dictionary *p_dict, const char *p_key, double *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    if (l_entry->m_value.m_type == e_ATP_ValueType_double)
    {
        *p_value = l_entry->m_value.m_value.m_double;
        return 1;
    }

    return 0;
}

int ATP_dictionaryGetBool(ATP_Dictionary *p_dict, const char *p_key, int *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    if (l_entry->m_value.m_type == e_ATP_ValueType_bool)
    {
        *p_value = l_entry->m_value.m_value.m_bool;
        return 1;
    }

    return 0;
}
