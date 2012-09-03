#include "ATP/Library/Processor.h"
#include "ATP/Library/Log.h"
#include "ATP/Library/Exit.h"

#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define PROCNAME "random"

typedef struct Settings
{
    unsigned int m_minEntries;
    unsigned int m_maxEntries;
    unsigned int m_maxDepth;
} Settings;

// forward declarations
static int randomDictionary(ATP_Dictionary *p_dict, const Settings *p_settings, unsigned int p_depth);
static int randomArray(ATP_Array *p_array, const Settings *p_settings, unsigned int p_depth);

static FILE *gs_randFile = NULL;

static const char cs_characters[] = " \"'.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

static void usage(void)
{
    LOG(
"Processor: " PROCNAME "\n");
    LOG(
"    Generates a " PROCNAME " dictionary.\n\n");
    LOG(
"    Usage: @" PROCNAME " <min_entries> <max_entries> <max_depth>\n\n");
    LOG(
"        <min_entries> The minimum number of dictionary keys/array entries to\n"
"                      create at each level\n");
    LOG(
"        <max_entries> The maximum number of dictionary keys/array entries to\n"
"                      create at each level\n");
    LOG(
"          <max_depth> The maximum nested dictionary/array depth to use\n\n");
}

static void getRandomBytes(void *p_buffer, unsigned int p_count)
{
#ifdef _WIN32
    // TODO: Windows implementation
#else   // NOTE: assume something with /dev/urandom for now
    if (gs_randFile == NULL)
    {
        gs_randFile = fopen("/dev/urandom", "rb");
        if (gs_randFile == NULL)
        {
            PERR();
            exit(EX_OSFILE);
        }
    }

    fread(p_buffer, 1, p_count, gs_randFile);
#endif
}

static int randomBool(void)
{
    int l_rand;
    getRandomBytes(&l_rand, sizeof(l_rand));
    return ((l_rand % 2) * (l_rand % 2));
}

static int randomSign(void)
{
    return (randomBool() ? 1 : -1);
}

static double randomDouble(void)
{
    unsigned int l_rand;
    getRandomBytes(&l_rand, sizeof(l_rand));
    return ((double) l_rand / (double) UINT_MAX);
}

static unsigned long long randomUint(unsigned long long p_lower, unsigned long long p_upper)
{
    unsigned long long l_rand;
    getRandomBytes(&l_rand, sizeof(l_rand));
    l_rand = p_lower + (l_rand % (p_upper - p_lower));
    // DBG("random unsigned integer: %llu\n", l_rand);
    return l_rand;
}

static signed long long randomInt(signed long long p_lower, signed long long p_upper)
{
    signed long long l_rand = (p_lower + randomUint(0, p_upper - p_lower));
    // DBG("random signed integer: %lld\n", l_rand);
    return l_rand;
}

static void randomString(char p_buffer[c_ATP_Dictionary_keySize + 1])
{
    unsigned int i;
    unsigned int l_count = randomUint(1, c_ATP_Dictionary_keySize);

    memset(p_buffer, 0, c_ATP_Dictionary_keySize + 1);
    for (i = 0; i < l_count; ++i)
    {
        unsigned int c = randomUint(0, strlen(cs_characters));
        p_buffer[i] = cs_characters[c];
    }
}

static int randomArray(ATP_Array *p_array, const Settings *p_settings, unsigned int p_depth)
{
    unsigned int i;
    ATP_ValueType l_type = randomUint(e_ATP_ValueType_string, e_ATP_ValueType_array);
    unsigned int l_entries = randomUint(p_settings->m_minEntries, p_settings->m_maxEntries + 1);
    for (i = 0; i < l_entries; ++i)
    {
        char l_randStr[c_ATP_Dictionary_keySize + 1];
        ATP_Dictionary l_randDict;
        ATP_Array l_randArray;

        switch (l_type)
        {
            case e_ATP_ValueType_string:
                randomString(l_randStr);
                if (!ATP_arraySetString(p_array, i, l_randStr))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_uint:
                if (!ATP_arraySetUint(p_array, i, randomUint(0, ULLONG_MAX)))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_int:
                if (!ATP_arraySetInt(p_array, i, randomInt(LLONG_MIN, LLONG_MAX)))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_double:
                if (!ATP_arraySetDouble(p_array, i, randomDouble()))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_bool:
                if (!ATP_arraySetBool(p_array, i, randomBool()))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_dict:
                ATP_dictionaryInit(&l_randDict);
                if (p_depth + 1 < p_settings->m_maxDepth)
                {
                    if (!randomDictionary(&l_randDict, p_settings, p_depth + 1))
                    {
                        ATP_dictionaryDestroy(&l_randDict);
                        return 0;
                    }
                }
                if (!ATP_arraySetDict(p_array, i, l_randDict))
                {
                    ATP_dictionaryDestroy(&l_randDict);
                    return 0;
                }
                break;
            case e_ATP_ValueType_array:
                ATP_arrayInit(&l_randArray);
                if (p_depth + 1 < p_settings->m_maxDepth)
                {
                    if (!randomArray(&l_randArray, p_settings, p_depth + 1))
                    {
                        ATP_arrayDestroy(&l_randArray);
                        return 0;
                    }
                }
                if (!ATP_arraySetArray(p_array, i, l_randArray))
                {
                    ATP_arrayDestroy(&l_randArray);
                    return 0;
                }
                break;
            default:
                break;
        }
    }

    DBG("finished creating random array\n");
    return 1;
}

static int randomDictionary(ATP_Dictionary *p_dict, const Settings *p_settings, unsigned int p_depth)
{
    unsigned int l_entries = randomUint(p_settings->m_minEntries, p_settings->m_maxEntries + 1);
    while (l_entries-- > 0)
    {
        char l_key[c_ATP_Dictionary_keySize + 1];
        char l_randStr[c_ATP_Dictionary_keySize + 1];
        ATP_Dictionary l_randDict;
        ATP_Array l_randArray;
        randomString(l_key);

        switch (randomUint(e_ATP_ValueType_string, e_ATP_ValueType_array + 1))
        {
            case e_ATP_ValueType_string:
                randomString(l_randStr);
                if (!ATP_dictionarySetString(p_dict, l_key, l_randStr))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_uint:
                if (!ATP_dictionarySetUint(p_dict, l_key, randomUint(0, ULLONG_MAX)))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_int:
                if (!ATP_dictionarySetInt(p_dict, l_key, randomInt(LLONG_MIN, LLONG_MAX)))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_double:
                if (!ATP_dictionarySetDouble(p_dict, l_key, randomDouble()))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_bool:
                if (!ATP_dictionarySetBool(p_dict, l_key, randomBool()))
                {
                    return 0;
                }
                break;
            case e_ATP_ValueType_dict:
                ATP_dictionaryInit(&l_randDict);
                if (p_depth + 1 < p_settings->m_maxDepth)
                {
                    if (!randomDictionary(&l_randDict, p_settings, p_depth + 1))
                    {
                        ATP_dictionaryDestroy(&l_randDict);
                        return 0;
                    }
                }
                if (!ATP_dictionarySetDict(p_dict, l_key, l_randDict))
                {
                    ATP_dictionaryDestroy(&l_randDict);
                    return 0;
                }
                break;
            case e_ATP_ValueType_array:
                ATP_arrayInit(&l_randArray);
                if (p_depth + 1 < p_settings->m_maxDepth)
                {
                    if (!randomArray(&l_randArray, p_settings, p_depth + 1))
                    {
                        ATP_arrayDestroy(&l_randArray);
                        return 0;
                    }
                }
                if (!ATP_dictionarySetArray(p_dict, l_key, l_randArray))
                {
                    ATP_arrayDestroy(&l_randArray);
                    return 0;
                }
                break;
            default:
                break;
        }
    }

    DBG("finished creating random dictionary\n");
    return 1;
}

static int run(unsigned int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token)
{
    Settings *l_settings = p_token;
    if (ATP_processorHelpRequested())
    {
        usage();
    }
    else
    {
        return randomDictionary(p_output, l_settings, 0);
    }

    return 1;
}

static void unload(void *p_token)
{
    Settings *l_settings = p_token;
    free(l_settings);
}

static int stringIsNumber(const char *p_string)
{
    unsigned int i;
    unsigned int l_length = strlen(p_string);
    for (i = 0; i < l_length; ++i)
    {
        if (!isdigit(p_string[i]))
        {
            return 0;
        }
    }

    return 1;
}

#ifdef ATTR_STATIC_PROCESSORS
int random_load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#else
int load(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface)
#endif
{
    unsigned int i;
    Settings *l_settings;

    unsigned int l_count = ATP_arrayLength(p_parameters);
    if (!ATP_processorHelpRequested() && l_count != 3)
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
            free(l_settings);
            usage();
            return 0;
        }
        DBG(PROCNAME ": parameter %u is '%s'\n", i, l_parameter);

        if (!stringIsNumber(l_parameter))
        {
            free(l_settings);
            ERR(PROCNAME ": '%s' is not a valid parameter\n", l_parameter);
            usage();
            return 0;
        }

        switch (i)
        {
            case 0:
                l_settings->m_minEntries = atoi(l_parameter);
                break;
            case 1:
                l_settings->m_maxEntries = atoi(l_parameter);
                break;
            case 2:
                l_settings->m_maxDepth = atoi(l_parameter);
                break;
        }
    }

    p_interface->m_token = l_settings;
    p_interface->run = &run;
    p_interface->unload = &unload;
    return 1;
}
