#include "ATP/Library/Processor.h"
#include "ATP/Library/Log.h"
#include "ATP/Library/Exit.h"
#include "ATP/Library/Export.h"

#include "ATP/ThirdParty/libjson/libjson.h"

#include <stdlib.h>
#include <math.h>

#define PROCNAME "json"

typedef struct Settings
{
    int m_fileIsOutput;
    char *m_filePath;
} Settings;

// forward references
static int writeJsonArray(ATP_Array *p_source, JSONNODE *p_node);
static int writeJsonDictionary(ATP_Dictionary *p_source, JSONNODE *p_node);
static int readJsonArray(JSONNODE *p_node, ATP_Array *p_dest);
static int readJsonDictionary(JSONNODE *p_node, ATP_Dictionary *p_dest);

static void usage(void)
{
    LOG(
"Processor: " PROCNAME "\n");
    LOG(
"    Reads or writes the working dictionary in JSON format.  When writing to a\n"
"    file, the dictionary is also passed on to the next pipeline stage, if any.\n\n");
    LOG(
"    Usage: @" PROCNAME " read stdin|<filename>\n"
"           @" PROCNAME " write stdout|<filename>\n\n");
    LOG(
"             stdin Indicates that the JSON source should be read from stdin\n"
"                   rather than a file\n");
    LOG(
"            stdout Indicates that the JSON source should be written to stdout\n"
"                   rather than a file\n");
    LOG(
"        <filename> The name of a file to read the working dictionary from or\n"
"                   write it to\n\n");
}

static int writeJsonArray(ATP_Array *p_source, JSONNODE *p_node)
{
    unsigned int i;
    DBG("array %p has %u entries\n", *p_source, ATP_arrayLength(p_source));
    for (i = 0; i < ATP_arrayLength(p_source); ++i)
    {
        JSONNODE *l_child = NULL;

        DBG("reading from array %p entry (type %s) %u\n", *p_source, ATP_valueTypeToString(ATP_arrayGetType(p_source, i)), i);
        switch (ATP_arrayGetType(p_source, i))
        {
            case e_ATP_ValueType_string:
                {
                    const char *l_value = NULL;
                    ATP_arrayGetString(p_source, i, &l_value);
                    l_child = json_new_a(NULL, l_value);
                }
                break;
            case e_ATP_ValueType_uint:
                {
                    unsigned long long l_value = 0;
                    ATP_arrayGetUint(p_source, i, &l_value);
                    l_child = json_new_i(NULL, l_value);
                }
                break;
            case e_ATP_ValueType_int:
                {
                    signed long long l_value = 0;
                    ATP_arrayGetInt(p_source, i, &l_value);
                    l_child = json_new_i(NULL, l_value);
                }
                break;
            case e_ATP_ValueType_double:
                {
                    double l_value = 0.0;
                    ATP_arrayGetDouble(p_source, i, &l_value);
                    l_child = json_new_f(NULL, l_value);
                }
                break;
            case e_ATP_ValueType_bool:
                {
                    int l_value = 0;
                    ATP_arrayGetBool(p_source, i, &l_value);
                    l_child = json_new_b(NULL, l_value);
                }
                break;
            case e_ATP_ValueType_dict:
                {
                    ATP_Dictionary *l_value = NULL;
                    l_child = json_new(JSON_NODE);
                    if (l_child == NULL)
                    {
                        ERR(PROCNAME ": could not allocate JSON node\n");
                        exit(EX_SOFTWARE);
                    }

                    ATP_arrayGetDict(p_source, i, &l_value);
                    if (!writeJsonDictionary(l_value, l_child))
                    {
                        json_free(l_child);
                        return 0;
                    }
                    DBG("processed dictionary %p\n", *l_value);
                }
                break;
            case e_ATP_ValueType_array:
                {
                    ATP_Array *l_value = NULL;
                    l_child = json_new(JSON_ARRAY);
                    if (l_child == NULL)
                    {
                        ERR(PROCNAME ": could not allocate JSON node\n");
                        exit(EX_SOFTWARE);
                    }

                    ATP_arrayGetArray(p_source, i, &l_value);
                    if (!writeJsonArray(l_value, l_child))
                    {
                        json_free(l_child);
                        return 0;
                    }
                    DBG("processed array %p\n", *l_value);
                }
                break;
            default:
                break;
        }

        if (l_child != NULL)
        {
            json_push_back(p_node, l_child);
        }
    }

    return 1;
}

static int writeJsonDictionary(ATP_Dictionary *p_source, JSONNODE *p_node)
{
    ATP_DictionaryIterator it;
    DBG("dictionary %p has %u entries\n", *p_source, ATP_dictionaryCount(p_source));
    for (it = ATP_dictionaryBegin(p_source); ATP_dictionaryHasNext(it); it = ATP_dictionaryNext(it))
    {
        JSONNODE *l_child = NULL;
        const char *l_key = ATP_dictionaryGetKey(it);

        DBG("reading from %p entry (type %s) '%s'\n", *p_source, ATP_valueTypeToString(ATP_dictionaryGetType(it)), l_key);
        switch (ATP_dictionaryGetType(it))
        {
            case e_ATP_ValueType_string:
                {
                    const char *l_value = NULL;
                    ATP_dictionaryItGetString(it, &l_value);
                    l_child = json_new_a(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_uint:
                {
                    unsigned long long l_value = 0;
                    ATP_dictionaryItGetUint(it, &l_value);
                    l_child = json_new_i(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_int:
                {
                    signed long long l_value = 0;
                    ATP_dictionaryItGetInt(it, &l_value);
                    l_child = json_new_i(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_double:
                {
                    double l_value = 0.0;
                    ATP_dictionaryItGetDouble(it, &l_value);
                    l_child = json_new_f(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_bool:
                {
                    int l_value = 0;
                    ATP_dictionaryItGetBool(it, &l_value);
                    l_child = json_new_b(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_dict:
                {
                    ATP_Dictionary *l_value = NULL;
                    l_child = json_new(JSON_NODE);
                    if (l_child == NULL)
                    {
                        ERR(PROCNAME ": could not allocate JSON node\n");
                        exit(EX_SOFTWARE);
                    }

                    ATP_dictionaryItGetDict(it, &l_value);
                    if (!writeJsonDictionary(l_value, l_child))
                    {
                        json_free(l_child);
                        return 0;
                    }

                    json_set_name(l_child, l_key);
                    DBG("processed dictionary %p\n", *l_value);
                }
                break;
            case e_ATP_ValueType_array:
                {
                    ATP_Array *l_value = NULL;
                    l_child = json_new(JSON_ARRAY);
                    if (l_child == NULL)
                    {
                        ERR(PROCNAME ": could not allocate JSON node\n");
                        exit(EX_SOFTWARE);
                    }

                    ATP_dictionaryItGetArray(it, &l_value);
                    if (!writeJsonArray(l_value, l_child))
                    {
                        json_free(l_child);
                        return 0;
                    }

                    json_set_name(l_child, l_key);
                    DBG("processed array %p\n", *l_value);
                }
                break;
            default:
                break;
        }

        if (l_child != NULL)
        {
            json_push_back(p_node, l_child);
        }
    }

    return 1;
}

static int writeJson(ATP_Dictionary *p_source, const char *p_filename)
{
    FILE *l_file = NULL;
    JSONNODE *l_node = NULL;

    if (strcmp("stdout", p_filename) == 0)
    {
        l_file = stdout;
    }
    else
    {
        l_file = fopen(p_filename, "wb");
        if (l_file == NULL)
        {
            PERR();
            return 0;
        }
    }

    l_node = json_new(JSON_NODE);
    if (l_node == NULL)
    {
        ERR(PROCNAME ": could not allocate JSON instance\n");
        if (l_file != stdout)
        {
            fclose(l_file);
        }
        exit(EX_SOFTWARE);
    }

    // convert the dictionary to a json structure
    if (writeJsonDictionary(p_source, l_node))
    {
        // write out the json string
        json_char *l_json = json_write_formatted(l_node);
        if (l_json == NULL)
        {
            ERR(PROCNAME ": could not format JSON\n");
            json_delete(l_node);
            if (l_file != stdout)
            {
                fclose(l_file);
            }
            return 0;
        }

        fputs(l_json, l_file);
        fputs("\n", l_file);
        json_free(l_json);
        json_delete(l_node);
        if (l_file != stdout)
        {
            fclose(l_file);
        }
        return 1;
    }
    else
    {
        json_delete(l_node);
        if (l_file != stdout)
        {
            fclose(l_file);
        }
        return 0;
    }
}

static int readJsonArray(JSONNODE *p_node, ATP_Array *p_dest)
{
    JSONNODE_ITERATOR it;
    unsigned int i = 0;

    for (it = json_begin(p_node); it != json_end(p_node); ++it)
    {
        if (json_type(*it) == JSON_NODE)
        {
            ATP_Dictionary l_subDict;
            ATP_dictionaryInit(&l_subDict);
            if (!readJsonDictionary(*it, &l_subDict))
            {
                ATP_dictionaryDestroy(&l_subDict);
                return 0;
            }
            if (!ATP_arraySetDict(p_dest, i, l_subDict))
            {
                ATP_dictionaryDestroy(&l_subDict);
                return 0;
            }
        }
        else if (json_type(*it) == JSON_ARRAY)
        {
            ATP_Array l_subArray;
            ATP_arrayInit(&l_subArray);
            if (!readJsonArray(*it, &l_subArray))
            {
                ATP_arrayDestroy(&l_subArray);
                return 0;
            }
            if (!ATP_arraySetArray(p_dest, i, l_subArray))
            {
                ATP_arrayDestroy(&l_subArray);
                return 0;
            }
        }
        else if (json_type(*it) == JSON_NUMBER)
        {
            double l_number = json_as_float(*it);
            double l_integer = 0.0;
            // if the number is integral, create the dictionary entry as an integer, otherwise create it as a double
            if (modf(l_number, &l_integer) == 0.0f)
            {
                if (!ATP_arraySetInt(p_dest, i, (signed long long) l_integer))
                {
                    return 0;
                }
            }
            else
            {
                if (!ATP_arraySetDouble(p_dest, i, l_number))
                {
                    return 0;
                }
            }
        }
        else if (json_type(*it) == JSON_BOOL)
        {
            int l_bool = json_as_bool(*it);
            if (!ATP_arraySetBool(p_dest, i, l_bool))
            {
                return 0;
            }
        }
        else if (json_type(*it) == JSON_STRING)
        {
            json_char *l_str = json_as_string(*it);
            if (!ATP_arraySetString(p_dest, i, l_str))
            {
                json_free(l_str);
                return 0;
            }
            json_free(l_str);
        }

        ++i;
    }

    return 1;
}

static int readJsonDictionary(JSONNODE *p_node, ATP_Dictionary *p_dest)
{
    JSONNODE_ITERATOR it;
    for (it = json_begin(p_node); it != json_end(p_node); ++it)
    {
        json_char *l_name = json_name(*it);
        if (json_type(*it) == JSON_NODE)
        {
            ATP_Dictionary l_subDict;
            ATP_dictionaryInit(&l_subDict);
            if (!readJsonDictionary(*it, &l_subDict))
            {
                ATP_dictionaryDestroy(&l_subDict);
                json_free(l_name);
                return 0;
            }
            if (!ATP_dictionarySetDict(p_dest, l_name, l_subDict))
            {
                ATP_dictionaryDestroy(&l_subDict);
                json_free(l_name);
                return 0;
            }
        }
        else if (json_type(*it) == JSON_ARRAY)
        {
            ATP_Array l_array;
            ATP_arrayInit(&l_array);
            if (!readJsonArray(*it, &l_array))
            {
                ATP_arrayDestroy(&l_array);
                json_free(l_name);
                return 0;
            }
            if (!ATP_dictionarySetArray(p_dest, l_name, l_array))
            {
                ATP_arrayDestroy(&l_array);
                json_free(l_name);
                return 0;
            }
        }
        else if (json_type(*it) == JSON_NUMBER)
        {
            double l_number = json_as_float(*it);
            double l_integer = 0.0;
            // if the number is integral, create the dictionary entry as an integer, otherwise create it as a double
            if (modf(l_number, &l_integer) == 0.0f)
            {
                if (!ATP_dictionarySetInt(p_dest, l_name, (signed long long) l_integer))
                {
                    json_free(l_name);
                    return 0;
                }
            }
            else
            {
                if (!ATP_dictionarySetDouble(p_dest, l_name, l_number))
                {
                    json_free(l_name);
                    return 0;
                }
            }
        }
        else if (json_type(*it) == JSON_BOOL)
        {
            int l_bool = json_as_bool(*it);
            if (!ATP_dictionarySetBool(p_dest, l_name, l_bool))
            {
                json_free(l_name);
                return 0;
            }
        }
        else if (json_type(*it) == JSON_STRING)
        {
            json_char *l_str = json_as_string(*it);
            if (!ATP_dictionarySetString(p_dest, l_name, l_str))
            {
                json_free(l_str);
                json_free(l_name);
                return 0;
            }
            json_free(l_str);
        }

        json_free(l_name);
    }

    return 1;
}

static int readJson(const char *p_filename, ATP_Dictionary *p_dest)
{
    FILE *l_file = NULL;
    JSONNODE *l_node = NULL;
    json_char *l_json = NULL;
    size_t l_fileSize;
    int l_return;

    if (strcmp("stdin", p_filename) == 0)
    {
        l_file = stdout;
    }
    else
    {
        l_file = fopen(p_filename, "rb");
        if (l_file == NULL)
        {
            PERR();
            return 0;
        }
    }

    // read in the entire file
    fseek(l_file, 0L, SEEK_END);
    l_fileSize = ftell(l_file);
    fseek(l_file, 0L, SEEK_SET);
    l_json = malloc(l_fileSize + 1);
    if (l_json == NULL)
    {
        PERR();
        exit(EX_OSERR);
    }
    if (fread(l_json, 1, l_fileSize, l_file) != l_fileSize)
    {
        PERR();
        free(l_json);
        fclose(l_file);
        return 0;
    }
    fclose(l_file);
    l_json[l_fileSize] = '\0';
    DBG("raw JSON: %s\n", l_json);

    // parse the json
    l_node = json_parse(l_json);
    if (json_type(l_node) != JSON_NODE)
    {
        ERR(PROCNAME ": Invalid JSON file: %s (type %d is not JSON_NODE)\n", p_filename, json_type(l_node));
        json_delete(l_node);
        free(l_json);
        return 0;
    }

    // convert the json structure to a dictionary
    l_return = readJsonDictionary(l_node, p_dest);
    json_delete(l_node);
    free(l_json);

    return l_return;
}

static int run(unsigned int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token)
{
    Settings *l_settings = p_token;

    DBG("in json run, %s\n", (l_settings->m_fileIsOutput ? "writing" : "reading"));
    if (ATP_processorHelpRequested())
    {
        usage();
    }
    else if (l_settings->m_fileIsOutput)
    {
        *p_output = *p_input;
        DBG("writing JSON to %s...\n", l_settings->m_filePath);
        return writeJson(p_input, l_settings->m_filePath);
    }
    else
    {
        return readJson(l_settings->m_filePath, p_output);
    }
    return 1;
}

static void unload(void *p_token)
{
    Settings *l_settings = p_token;
    free(l_settings->m_filePath);
    free(l_settings);
}

#ifdef ATTR_STATIC_PROCESSORS
int json_load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#else
EXPORT int load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#endif
{
    unsigned int i;
    Settings *l_settings;

    unsigned int l_count = ATP_arrayLength(p_parameters);
    if (!ATP_processorHelpRequested() && l_count != 2)
    {
        ERR(PROCNAME ": wrong number of parameters\n");
        usage();
        return 0;
    }

    l_settings = malloc(sizeof(Settings));
    if (l_settings == NULL)
    {
        PERR();
        exit(EX_OSERR);
    }
    memset(l_settings, 0, sizeof(Settings));

    for (i = 0; i < l_count; ++i)
    {
        const char *l_parameter = NULL;
        if (!ATP_arrayGetString(p_parameters, i, &l_parameter))
        {
            free(l_settings->m_filePath);
            free(l_settings);
            usage();
            return 0;
        }
        DBG(PROCNAME ": parameter %u is '%s'\n", i, l_parameter);

        switch (i)
        {
            case 0:
                if (strcmp("read", l_parameter) == 0)
                {
                    l_settings->m_fileIsOutput = 0;
                }
                else if (strcmp("write", l_parameter) == 0)
                {
                    l_settings->m_fileIsOutput = 1;
                }
                else
                {
                    free(l_settings->m_filePath);
                    free(l_settings);
                    ERR(PROCNAME ": '%s' is not a valid parameter\n", l_parameter);
                    usage();
                    return 0;
                }
                break;
            case 1:
                l_settings->m_filePath = strdup(l_parameter);
                break;
        }
    }

    p_interface->m_token = l_settings;
    p_interface->run = &run;
    p_interface->unload = &unload;
    return 1;
}
