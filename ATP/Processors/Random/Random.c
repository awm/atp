#include "ATP/Library/Processor.h"
#include "ATP/Library/Log.h"
#include "ATP/Library/Exit.h"

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>

typedef struct Settings
{
    unsigned int m_minEntries;
    unsigned int m_maxEntries;
    unsigned int m_maxDepth;
} Settings;

static const char cs_characters[] = " \"'.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

static void usage(void)
{
    LOG(
"Processor: random\n");
    LOG(
"    Generates a random dictionary.\n\n");
    LOG(
"    Usage: @random <min_entries> <max_entries> <max_depth>\n");
    LOG(
"        <min_entries> The minimum number of dictionary keys/array entries to\n"
"                      create at each level\n");
    LOG(
"        <max_entries> The maximum number of dictionary keys/array entries to\n"
"                      create at each level\n");
    LOG(
"          <max_depth> The maximum nested dictionary/array depth to use\n\n");
}

static unsigned long long randomUint(unsigned long long p_lower, unsigned long long p_upper)
{
    return (((unsigned long long) rand() * (unsigned long long) rand()) % (p_upper - p_lower)) + p_lower;
}

static signed long long randomInt(signed long long p_lower, signed long long p_upper)
{
    return (((signed long long) rand() * (signed long long) rand()) % (p_upper - p_lower)) + p_lower;
}

static int randomBool(void)
{
    return (rand() % 2);
}

static double randomDouble(void)
{
    return ((randomBool() * -1.0) * (double) rand() / (double) RAND_MAX);
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

static int run(int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token)
{
    Settings *l_settings = p_token;

    if (ATP_processorHelpRequested())
    {
        usage();
        *p_output = NULL;
    }
    else
    {
        unsigned int l_entries;
        time_t l_now = time(NULL);
        srand(l_now);

        ATP_dictionaryInit(p_output);
        l_entries = randomUint(l_settings->m_minEntries, l_settings->m_maxEntries + 1);
        while (l_entries-- > 0)
        {
            char l_key[c_ATP_Dictionary_keySize + 1];
            randomString(l_key);

            switch (randomUint(e_ATP_ValueType_string, e_ATP_ValueType_array))
            {
                // TODO: add other types
                case e_ATP_ValueType_uint:
                    if (!ATP_dictionarySetUint(p_output, l_key, randomUint(0, ULLONG_MAX)))
                    {
                        ATP_dictionaryDestroy(p_output);
                        return 0;
                    }
                    break;
                case e_ATP_ValueType_int:
                    if (!ATP_dictionarySetInt(p_output, l_key, randomInt(LLONG_MIN, LLONG_MAX)))
                    {
                        ATP_dictionaryDestroy(p_output);
                        return 0;
                    }
                    break;
                case e_ATP_ValueType_double:
                    if (!ATP_dictionarySetDouble(p_output, l_key, randomDouble()))
                    {
                        ATP_dictionaryDestroy(p_output);
                        return 0;
                    }
                    break;
                case e_ATP_ValueType_bool:
                default:
                    if (!ATP_dictionarySetBool(p_output, l_key, randomBool()))
                    {
                        ATP_dictionaryDestroy(p_output);
                        return 0;
                    }
                    break;
            }
        }
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
int random_load(int p_index, const ATP_CmdLineParam *p_parameters, struct ATP_ProcessorInterface *p_interface)
#else
int load(int p_index, const ATP_CmdLineParam *p_parameters, struct ATP_ProcessorInterface *p_interface)
#endif
{
    unsigned int l_argc = 0;
    const ATP_CmdLineParam *it;

    Settings *l_settings = malloc(sizeof(Settings));
    if (l_settings == NULL)
    {
        PERR();
        exit(EX_OSERR);
    }

    LL_FOREACH(p_parameters, it)
    {
        if (!stringIsNumber(it->m_parameter))
        {
            free(l_settings);
            ERR("random: '%s' is not a valid parameter\n", it->m_parameter);
            usage();
            return 0;
        }

        switch (l_argc)
        {
            case 0:
                l_settings->m_minEntries = atoi(it->m_parameter);
                break;
            case 1:
                l_settings->m_maxEntries = atoi(it->m_parameter);
                break;
            case 2:
                l_settings->m_maxDepth = atoi(it->m_parameter);
                break;
        }

        ++l_argc;
    }
    if (l_argc != 3)
    {
        free(l_settings);
        ERR("random: wrong number of parameters\n");
        usage();
        return 0;
    }

    p_interface->m_token = l_settings;
    p_interface->run = &run;
    p_interface->unload = &unload;
    return 1;
}
