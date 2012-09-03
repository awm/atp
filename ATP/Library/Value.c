#include "Value.inc"

void Value_changeType(Value *p_value, ATP_ValueType p_newType)
{
    if (p_value->m_type != p_newType)
    {
        switch (p_value->m_type)
        {
            case e_ATP_ValueType_string:
                utstring_done(&p_value->m_value.m_string);
                break;
            case e_ATP_ValueType_dict:
                ATP_dictionaryDestroy(&p_value->m_value.m_dict);
                break;
            case e_ATP_ValueType_array:
                ATP_arrayDestroy(&p_value->m_value.m_array);
                break;
            default:
                break;
        }

        p_value->m_type = p_newType;
        switch (p_value->m_type)
        {
            case e_ATP_ValueType_string:
                utstring_init(&p_value->m_value.m_string);
                break;
            case e_ATP_ValueType_dict:
                ATP_dictionaryInit(&p_value->m_value.m_dict);
                break;
            case e_ATP_ValueType_array:
                ATP_arrayInit(&p_value->m_value.m_array);
                break;
            default:
                break;
        }
    }
}

void Value_copy(Value *p_dest, const Value *p_source)
{
    Value_changeType(p_dest, p_source->m_type);
    switch (p_dest->m_type)
    {
        case e_ATP_ValueType_string:
            utstring_concat(&p_dest->m_value.m_string, &p_source->m_value.m_string);
            break;
        case e_ATP_ValueType_uint:
            p_dest->m_value.m_uint = p_source->m_value.m_uint;
            break;
        case e_ATP_ValueType_int:
            p_dest->m_value.m_int = p_source->m_value.m_int;
            break;
        case e_ATP_ValueType_double:
            p_dest->m_value.m_double = p_source->m_value.m_double;
            break;
        case e_ATP_ValueType_bool:
            p_dest->m_value.m_bool = p_source->m_value.m_bool;
            break;
        case e_ATP_ValueType_dict:
            p_dest->m_value.m_dict = ATP_dictionaryDuplicate(&p_source->m_value.m_dict);
            break;
        case e_ATP_ValueType_array:
            p_dest->m_value.m_array = ATP_arrayDuplicate(&p_source->m_value.m_array);
            break;
        default:
            break;
    }
}

const char *ATP_valueTypeToString(ATP_ValueType p_type)
{
    switch (p_type)
    {
        case e_ATP_ValueType_none:      return "none";
        case e_ATP_ValueType_string:    return "string";
        case e_ATP_ValueType_uint:      return "unsigned integer";
        case e_ATP_ValueType_int:       return "integer";
        case e_ATP_ValueType_double:    return "double";
        case e_ATP_ValueType_bool:      return "boolean";
        case e_ATP_ValueType_dict:      return "dictionary";
        case e_ATP_ValueType_array:     return "array";
    }

    return "<unknown>";
}
