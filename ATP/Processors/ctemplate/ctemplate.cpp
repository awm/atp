#include "ATP/Library/Processor.h"
#include "ATP/Library/Log.h"
#include "ATP/Library/Exit.h"
#include "ATP/Library/Export.h"

#include "ATP/ThirdParty/ctemplate/ctemplate/template.h"

#include <string>
#include <sstream>
#include <fstream>

#define PROCNAME "ctemplate"

struct Settings
{
    std::string m_template;
    std::string m_output;
    bool m_annotate;
    ctemplate::Strip m_strip;

    Settings(void): m_annotate(false), m_strip(ctemplate::DO_NOT_STRIP)
    {
        // do nothing
    }
};

// forward references
static bool atpArrayToCtemplateDicts(ctemplate::TemplateDictionary &p_tplDict, ATP_Array *p_atpArray, const std::string &p_key);
static bool atpDictToCtemplateDict(ctemplate::TemplateDictionary &p_tplDict, ATP_Dictionary *p_atpDict);

static void usage(void)
{
    LOG(
"Processor: " PROCNAME "\n");
    LOG(
"    Applies the incoming dictionary to the provided template file to produce\n"
"    the output file\n\n");
    LOG(
"    Usage: @" PROCNAME " <template_file> <output_file> [annotate]\n"
"               [striplines|stripspace]\n\n");
    LOG(
"        <template_file> The name of the template file to process\n");
    LOG(
"          <output_file> The name of the file to write the template result to\n");
    LOG(
"               annotate Annotate the output file with debug information\n");
    LOG(
"             striplines Strip blank lines from the template\n");
    LOG(
"             stripspace Strip leading and trailing white space from the template\n\n");
}

template <typename T>
static std::string toString(const T &p_item)
{
    std::stringstream l_stream;
    l_stream << p_item;
    return l_stream.str();
}

static bool atpArrayToCtemplateDicts(ctemplate::TemplateDictionary &p_tplDict, ATP_Array *p_atpArray, const std::string &p_key)
{
    unsigned int i;
    DBG("array %p has %u entries\n", *p_atpArray, ATP_arrayLength(p_atpArray));
    for (i = 0; i < ATP_arrayLength(p_atpArray); ++i)
    {
        DBG("reading from array %p entry (type %s) %u\n", *p_atpArray, ATP_valueTypeToString(ATP_arrayGetType(p_atpArray, i)), i);
        switch (ATP_arrayGetType(p_atpArray, i))
        {
            case e_ATP_ValueType_dict:
                {
                    ATP_Dictionary *l_value = NULL;
                    ctemplate::TemplateDictionary *l_subDict = p_tplDict.AddSectionDictionary(p_key);

                    ATP_arrayGetDict(p_atpArray, i, &l_value);
                    if (!atpDictToCtemplateDict(*l_subDict, l_value))
                    {
                        return false;
                    }
                    DBG("processed dictionary %p\n", *l_value);
                }
                break;
            default:
                // NOTE: ignore anything that is not a dictionary
                DBG(PROCNAME ": A non-dictionary array entry was ignored\n");
                break;
        }
    }

    return 1;
}

static bool atpDictToCtemplateDict(ctemplate::TemplateDictionary &p_tplDict, ATP_Dictionary *p_atpDict)
{
    ATP_DictionaryIterator it;
    DBG("dictionary %p has %u entries\n", *p_atpDict, ATP_dictionaryCount(p_atpDict));
    for (it = ATP_dictionaryBegin(p_atpDict); ATP_dictionaryHasNext(it); it = ATP_dictionaryNext(it))
    {
        std::string l_key = ATP_dictionaryGetKey(it);

        DBG("reading from %p entry (type %s) '%s'\n", *p_atpDict, ATP_valueTypeToString(ATP_dictionaryGetType(it)), l_key.c_str());
        switch (ATP_dictionaryGetType(it))
        {
            case e_ATP_ValueType_string:
                {
                    const char *l_value = NULL;
                    ATP_dictionaryItGetString(it, &l_value);
                    p_tplDict.SetValue(l_key, l_value);
                }
                break;
            case e_ATP_ValueType_uint:
                {
                    unsigned long long l_value = 0;
                    ATP_dictionaryItGetUint(it, &l_value);
                    p_tplDict.SetValue(l_key, toString(l_value));
                }
                break;
            case e_ATP_ValueType_int:
                {
                    signed long long l_value = 0;
                    ATP_dictionaryItGetInt(it, &l_value);
                    p_tplDict.SetValue(l_key, toString(l_value));
                }
                break;
            case e_ATP_ValueType_double:
                {
                    double l_value = 0.0;
                    ATP_dictionaryItGetDouble(it, &l_value);
                    p_tplDict.SetValue(l_key, toString(l_value));
                }
                break;
            case e_ATP_ValueType_bool:
                {
                    int l_value = 0;
                    ATP_dictionaryItGetBool(it, &l_value);
                    p_tplDict.SetValue(l_key, (l_value ? "true" : "false"));
                }
                break;
            case e_ATP_ValueType_dict:
                {
                    ATP_Dictionary *l_value = NULL;
                    ctemplate::TemplateDictionary *l_subDict = p_tplDict.AddSectionDictionary(l_key);

                    ATP_dictionaryItGetDict(it, &l_value);
                    if (!atpDictToCtemplateDict(*l_subDict, l_value))
                    {
                        return false;
                    }
                    DBG("processed dictionary %p\n", *l_value);
                }
                break;
            case e_ATP_ValueType_array:
                {
                    ATP_Array *l_value = NULL;
                    ATP_dictionaryItGetArray(it, &l_value);
                    if (!atpArrayToCtemplateDicts(p_tplDict, l_value, l_key))
                    {
                        return false;
                    }
                    DBG("processed array %p\n", *l_value);
                }
                break;
            default:
                break;
        }
    }

    return true;
}

static int run(unsigned int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token)
{
    Settings *l_settings = (Settings *) p_token;
    if (ATP_processorHelpRequested())
    {
        usage();
    }
    else
    {
        ctemplate::TemplateDictionary l_dict("atp");
        if (!atpDictToCtemplateDict(l_dict, p_input))
        {
            return 0;
        }

        std::string l_result;
        if (l_settings->m_annotate)
        {
            ctemplate::PerExpandData l_data;
            l_data.SetAnnotateOutput("");
            if (!ctemplate::ExpandWithData(l_settings->m_template, l_settings->m_strip, &l_dict, &l_data, &l_result))
            {
                ERR(PROCNAME ": Template expansion failed\n");
                return 0;
            }
        }
        else
        {
            if (!ctemplate::ExpandTemplate(l_settings->m_template, l_settings->m_strip, &l_dict, &l_result))
            {
                ERR(PROCNAME ": Template expansion failed\n");
                return 0;
            }
        }

        std::ofstream l_outfile(l_settings->m_output.c_str(), std::ofstream::out|std::ofstream::binary);
        if (l_outfile.fail())
        {
            ERR(PROCNAME ": Failed to open output file\n");
            return 0;
        }

        l_outfile << l_result;
        l_outfile.close();

        *p_output = *p_input;
    }

    return 1;
}

static void unload(void *p_token)
{
    Settings *l_settings = (Settings *) p_token;
    delete l_settings;
}

#ifdef ATTR_STATIC_PROCESSORS
extern "C" int ctemplate_load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#else
extern "C" EXPORT int load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#endif
{
    unsigned int l_count = ATP_arrayLength(p_parameters);
    if (!ATP_processorHelpRequested() && (l_count < 2 || l_count > 4))
    {
        ERR(PROCNAME ": wrong number of parameters\n");
        usage();
        return 0;
    }

    Settings *l_settings = new Settings;
    for (unsigned int i = 0; i < l_count; ++i)
    {
        const char *l_parameter = NULL;
        if (!ATP_arrayGetString(p_parameters, i, &l_parameter))
        {
            delete l_settings;
            usage();
            return 0;
        }
        DBG(PROCNAME ": parameter %u is '%s'\n", i, l_parameter);

        switch (i)
        {
            case 0:
                l_settings->m_template = l_parameter;
                break;
            case 1:
                l_settings->m_output = l_parameter;
                break;
            default:
                if (strcmp("annotate", l_parameter) == 0)
                {
                    l_settings->m_annotate = true;
                }
                else if (strcmp("striplines", l_parameter) == 0)
                {
                    l_settings->m_strip = ctemplate::STRIP_BLANK_LINES;
                }
                else if (strcmp("stripspace", l_parameter) == 0)
                {
                    l_settings->m_strip = ctemplate::STRIP_WHITESPACE;
                }
                break;
        }
    }

    p_interface->m_token = l_settings;
    p_interface->run = &run;
    p_interface->unload = &unload;
    return 1;
}
