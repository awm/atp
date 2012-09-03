#include "Array.h"
#include "Log.h"
#include "Value.inc"

#include "ATP/ThirdParty/UT/utarray.h"

#define CAST(array)     (*((ATP_ArrayImpl **) (array)))
#define CCAST(array)    (*((const ATP_ArrayImpl **) (array)))

typedef UT_array ATP_ArrayImpl;

// forward references
static void icdDtor(void *p_element);
static void icdCopy(void *p_dest, const void *p_source);

static const UT_icd cs_valueIcd = { sizeof(Value), NULL, &icdCopy, &icdDtor };

static void icdDtor(void *p_element)
{
    Value_changeType((Value *) p_element, e_ATP_ValueType_none);
}

static void icdCopy(void *p_dest, const void *p_source)
{
    Value_copy((Value *) p_dest, (const Value *) p_source);
}

void ATP_arrayInit(ATP_Array *p_array)
{
    utarray_new(CAST(p_array), &cs_valueIcd);
}

void ATP_arrayDestroy(ATP_Array *p_array)
{
    utarray_free(CAST(p_array));
}

unsigned int ATP_arrayLength(ATP_Array *p_array)
{
    return utarray_len(CAST(p_array));
}

int ATP_arrayErase(ATP_Array *p_array, unsigned int p_index)
{
    if (p_index >= utarray_len(CAST(p_array)))
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    utarray_erase(CAST(p_array), p_index, 1);
    return 1;
}

ATP_ValueType ATP_arrayGetType(ATP_Array *p_array, unsigned int p_index)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return e_ATP_ValueType_none;
    }

    return l_entry->m_type;
}

ATP_Array ATP_arrayDuplicate(const ATP_Array *p_array)
{
    UT_array *l_copy;
    utarray_new(l_copy, &cs_valueIcd);
    utarray_concat(l_copy, CCAST(p_array));
    return (struct ATP_ArrayImpl *) l_copy;
}

static Value *findOrCreateEntry(ATP_Array *p_array, unsigned int p_index)
{
    Value *l_entry;
    unsigned int l_length = utarray_len(CAST(p_array));
    if (p_index > l_length)
    {
        ERR("Index out of bounds\n");
        return NULL;
    }
    else if (p_index == l_length)
    {
        utarray_extend_back(CAST(p_array));
        l_entry = (Value *) utarray_back(CAST(p_array));
    }
    else
    {
        l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    }

    return l_entry;
}

int ATP_arraySetString(ATP_Array *p_array, unsigned int p_index, const char *p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = '%s'\n", p_index, p_value);
    Value_changeType(l_entry, e_ATP_ValueType_string);
    utstring_bincpy(&l_entry->m_value.m_string, p_value, strlen(p_value));
    return 1;
}

int ATP_arraySetUint(ATP_Array *p_array, unsigned int p_index, unsigned long long p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = %llu\n", p_index, p_value);
    Value_changeType(l_entry, e_ATP_ValueType_uint);
    l_entry->m_value.m_uint = p_value;
    return 1;
}

int ATP_arraySetInt(ATP_Array *p_array, unsigned int p_index, signed long long p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = %lld\n", p_index, p_value);
    Value_changeType(l_entry, e_ATP_ValueType_int);
    l_entry->m_value.m_int = p_value;
    return 1;
}

int ATP_arraySetDouble(ATP_Array *p_array, unsigned int p_index, double p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = %f\n", p_index, p_value);
    Value_changeType(l_entry, e_ATP_ValueType_double);
    l_entry->m_value.m_double = p_value;
    return 1;
}

int ATP_arraySetBool(ATP_Array *p_array, unsigned int p_index, int p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = %s\n", p_index, (p_value ? "true" : "false"));
    Value_changeType(l_entry, e_ATP_ValueType_bool);
    l_entry->m_value.m_bool = (p_value != 0);
    return 1;
}

int ATP_arraySetDict(ATP_Array *p_array, unsigned int p_index, ATP_Dictionary p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = <dictionary>\n", p_index);
    Value_changeType(l_entry, e_ATP_ValueType_dict);
    l_entry->m_value.m_dict = p_value;
    return 1;
}

int ATP_arraySetArray(ATP_Array *p_array, unsigned int p_index, ATP_Array p_value)
{
    Value *l_entry = findOrCreateEntry(p_array, p_index);
    if (l_entry == NULL)
    {
        return 0;
    }

    DBG("setting array[%u] = <array>\n", p_index);
    Value_changeType(l_entry, e_ATP_ValueType_array);
    l_entry->m_value.m_array = p_value;
    return 1;
}

int ATP_arrayGetString(ATP_Array *p_array, unsigned int p_index, const char **p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_string)
    {
        *p_value = utstring_body(&l_entry->m_value.m_string);
        return 1;
    }
    return 0;
}

int ATP_arrayGetUint(ATP_Array *p_array, unsigned int p_index, unsigned long long *p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_uint)
    {
        *p_value = l_entry->m_value.m_uint;
        return 1;
    }
    return 0;
}

int ATP_arrayGetInt(ATP_Array *p_array, unsigned int p_index, signed long long *p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_int)
    {
        *p_value = l_entry->m_value.m_int;
        return 1;
    }
    return 0;
}

int ATP_arrayGetDouble(ATP_Array *p_array, unsigned int p_index, double *p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_double)
    {
        *p_value = l_entry->m_value.m_double;
        return 1;
    }
    return 0;
}

int ATP_arrayGetBool(ATP_Array *p_array, unsigned int p_index, int *p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_bool)
    {
        *p_value = l_entry->m_value.m_bool;
        return 1;
    }
    return 0;
}

int ATP_arrayGetDict(ATP_Array *p_array, unsigned int p_index, ATP_Dictionary **p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_dict)
    {
        *p_value = &l_entry->m_value.m_dict;
        return 1;
    }
    return 0;
}

int ATP_arrayGetArray(ATP_Array *p_array, unsigned int p_index, ATP_Array **p_value)
{
    Value *l_entry = (Value *) utarray_eltptr(CAST(p_array), p_index);
    if (l_entry == NULL)
    {
        ERR("Index out of bounds\n");
        return 0;
    }

    if (l_entry->m_type == e_ATP_ValueType_array)
    {
        *p_value = &l_entry->m_value.m_array;
        return 1;
    }
    return 0;
}
