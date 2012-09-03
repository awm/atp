#include "Dictionary.h"
#include "Log.h"
#include "Exit.h"
#include "Value.inc"

#include "ATP/ThirdParty/UT/uthash.h"

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

void ATP_dictionaryDestroy(ATP_Dictionary *p_dict)
{
    if (p_dict != NULL)
    {
        ATP_DictionaryImpl *it;
        ATP_DictionaryImpl *l_tmp;

        HASH_ITER(hh, *p_dict, it, l_tmp)
        {
            HASH_DEL(*p_dict, it);
            Value_changeType(&it->m_value, e_ATP_ValueType_none);
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
        ATP_dictionaryErase(p_dict, l_entry);
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

ATP_Dictionary ATP_dictionaryDuplicate(const ATP_Dictionary *p_dict)
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

            Value_copy(&l_copy->m_value, &it->m_value);
            HASH_ADD_STR(l_result, m_key, l_copy);
        }

        return l_result;
    }

    return NULL;
}

static ATP_DictionaryImpl *findOrCreateEntry(ATP_Dictionary *p_dict, const char *p_key)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        l_entry = createEntry(p_key);
        if (l_entry == NULL)
        {
            return NULL;
        }

        HASH_ADD_STR(*p_dict, m_key, l_entry);
    }

    return l_entry;
}

int ATP_dictionarySetString(ATP_Dictionary *p_dict, const char *p_key, const char *p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetString(l_entry, p_value);
}

int ATP_dictionarySetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetUint(l_entry, p_value);
}

int ATP_dictionarySetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetInt(l_entry, p_value);
}

int ATP_dictionarySetDouble(ATP_Dictionary *p_dict, const char *p_key, double p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetDouble(l_entry, p_value);
}

int ATP_dictionarySetBool(ATP_Dictionary *p_dict, const char *p_key, int p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetBool(l_entry, p_value);
}

int ATP_dictionarySetDict(ATP_Dictionary *p_dict, const char *p_key, ATP_Dictionary p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetDict(l_entry, p_value);
}

int ATP_dictionarySetArray(ATP_Dictionary *p_dict, const char *p_key, ATP_Array p_value)
{
    ATP_DictionaryImpl *l_entry = findOrCreateEntry(p_dict, p_key);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItSetArray(l_entry, p_value);
}

int ATP_dictionaryGetString(ATP_Dictionary *p_dict, const char *p_key, const char **p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetString(l_entry, p_value);
}

int ATP_dictionaryGetUint(ATP_Dictionary *p_dict, const char *p_key, unsigned long long *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetUint(l_entry, p_value);
}

int ATP_dictionaryGetInt(ATP_Dictionary *p_dict, const char *p_key, signed long long *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetInt(l_entry, p_value);
}

int ATP_dictionaryGetDouble(ATP_Dictionary *p_dict, const char *p_key, double *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetDouble(l_entry, p_value);
}

int ATP_dictionaryGetBool(ATP_Dictionary *p_dict, const char *p_key, int *p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetBool(l_entry, p_value);
}

int ATP_dictionaryGetDict(ATP_Dictionary *p_dict, const char *p_key, ATP_Dictionary **p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetDict(l_entry, p_value);
}

int ATP_dictionaryGetArray(ATP_Dictionary *p_dict, const char *p_key, ATP_Array **p_value)
{
    ATP_DictionaryImpl *l_entry;
    HASH_FIND_STR(*p_dict, p_key, l_entry);
    if (l_entry == NULL)
    {
        return 0;
    }

    return ATP_dictionaryItGetArray(l_entry, p_value);
}

ATP_DictionaryIterator ATP_dictionaryBegin(ATP_Dictionary *p_dict)
{
    return *p_dict;
}

int ATP_dictionaryHasNext(ATP_DictionaryIterator p_iterator)
{
    return (p_iterator != NULL);
}

ATP_DictionaryIterator ATP_dictionaryNext(ATP_DictionaryIterator p_iterator)
{
    return p_iterator->hh.next;
}

ATP_DictionaryIterator ATP_dictionaryErase(ATP_Dictionary *p_dict, ATP_DictionaryIterator p_iterator)
{
    ATP_DictionaryIterator l_next = p_iterator->hh.next;

    HASH_DEL(*p_dict, p_iterator);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_none);
    free(p_iterator);

    return l_next;
}

const char *ATP_dictionaryGetKey(ATP_DictionaryIterator p_iterator)
{
    return p_iterator->m_key;
}

ATP_ValueType ATP_dictionaryGetType(ATP_DictionaryIterator p_iterator)
{
    return p_iterator->m_value.m_type;
}

int ATP_dictionaryItSetString(ATP_DictionaryIterator p_iterator, const char *p_value)
{
    DBG("setting '%s': '%s'\n", p_iterator->m_key, p_value);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_string);
    utstring_bincpy(&p_iterator->m_value.m_value.m_string, p_value, strlen(p_value));
    return 1;
}

int ATP_dictionaryItSetUint(ATP_DictionaryIterator p_iterator, unsigned long long p_value)
{
    DBG("setting '%s': %llu\n", p_iterator->m_key, p_value);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_uint);
    p_iterator->m_value.m_value.m_uint = p_value;
    return 1;
}

int ATP_dictionaryItSetInt(ATP_DictionaryIterator p_iterator, signed long long p_value)
{
    DBG("setting '%s': %lld\n", p_iterator->m_key, p_value);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_int);
    p_iterator->m_value.m_value.m_int = p_value;
    return 1;
}

int ATP_dictionaryItSetDouble(ATP_DictionaryIterator p_iterator, double p_value)
{
    DBG("setting '%s': %f\n", p_iterator->m_key, p_value);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_double);
    p_iterator->m_value.m_value.m_double = p_value;
    return 1;
}

int ATP_dictionaryItSetBool(ATP_DictionaryIterator p_iterator, int p_value)
{
    DBG("setting '%s': %s\n", p_iterator->m_key, (p_value ? "true" : "false"));
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_bool);
    p_iterator->m_value.m_value.m_bool = (p_value != 0);
    return 1;
}

int ATP_dictionaryItSetDict(ATP_DictionaryIterator p_iterator, ATP_Dictionary p_value)
{
    DBG("setting '%s': <dictionary>\n", p_iterator->m_key);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_dict);
    p_iterator->m_value.m_value.m_dict = p_value;
    return 1;
}

int ATP_dictionaryItSetArray(ATP_DictionaryIterator p_iterator, ATP_Array p_value)
{
    DBG("setting '%s': <array>\n", p_iterator->m_key);
    Value_changeType(&p_iterator->m_value, e_ATP_ValueType_array);
    p_iterator->m_value.m_value.m_array = p_value;
    return 1;
}

int ATP_dictionaryItGetString(ATP_DictionaryIterator p_iterator, const char **p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_string)
    {
        *p_value = utstring_body(&p_iterator->m_value.m_value.m_string);
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetUint(ATP_DictionaryIterator p_iterator, unsigned long long *p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_uint)
    {
        *p_value = p_iterator->m_value.m_value.m_uint;
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetInt(ATP_DictionaryIterator p_iterator, signed long long *p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_int)
    {
        *p_value = p_iterator->m_value.m_value.m_int;
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetDouble(ATP_DictionaryIterator p_iterator, double *p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_double)
    {
        *p_value = p_iterator->m_value.m_value.m_double;
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetBool(ATP_DictionaryIterator p_iterator, int *p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_bool)
    {
        *p_value = p_iterator->m_value.m_value.m_bool;
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetDict(ATP_DictionaryIterator p_iterator, ATP_Dictionary **p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_dict)
    {
        *p_value = &p_iterator->m_value.m_value.m_dict;
        DBG("dictionary member is %p\n", p_iterator->m_value.m_value.m_dict);
        return 1;
    }

    return 0;
}

int ATP_dictionaryItGetArray(ATP_DictionaryIterator p_iterator, ATP_Array **p_value)
{
    if (p_iterator->m_value.m_type == e_ATP_ValueType_array)
    {
        *p_value = &p_iterator->m_value.m_value.m_array;
        DBG("array member is %p\n", p_iterator->m_value.m_value.m_array);
        return 1;
    }

    return 0;
}
