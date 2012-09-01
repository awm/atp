#include "Processor.h"

#include "SharedLib.h"
#include "Exit.h"
#include "Log.h"

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef WIN32_LEAN_AND_MEAN
#else
    #include <glob.h>
    #include <libgen.h>
#endif

#ifndef PREFIX
    #define PREFIX "/usr/local"
#endif

static int gs_helpRequested = 0;

static ATP_StaticProcessor *gs_staticProcessors = NULL;
static unsigned int gs_staticProcessorCount = 0;

static void processorPath(const char *p_name, char *p_buffer, unsigned int p_size)
{
    char *l_procDir = getenv("ATP_PROCESSORS");
#ifdef _WIN32
    if (l_procDir == NULL)
    {
        char l_path[MAX_PATH + 1];
        char l_drive[_MAX_DRIVE + 1];
        char l_dir[_MAX_DIR + 1];

        GetModuleFileName(NULL, l_path, MAX_PATH);
        _splitpath(l_path, l_drive, l_dir, NULL, NULL);

        snprintf(p_buffer, p_size, "%s%s\\processors\\%s.atp", l_drive, l_dir, p_name);
    }
    else
    {
        snprintf(p_buffer, p_size, "%s\\%s.atp", l_procDir, p_name);
    }
#else // NOTE: assume POSIX for now
    if (l_procDir == NULL)
    {
        snprintf(p_buffer, p_size, PREFIX "/lib/atp/%s.atp", p_name);
    }
    else
    {
        snprintf(p_buffer, p_size, "%s/%s.atp", l_procDir, p_name);
    }
#endif
}

void ATP_processorsSetStatic(ATP_StaticProcessor p_table[], unsigned int p_count)
{
    gs_staticProcessors = p_table;
    gs_staticProcessorCount = p_count;
}

ATP_Processor *ATP_processorLoad(int p_index, const char *p_name, const ATP_CmdLineParam *p_parameters)
{
    unsigned int i;

    // first try to find a shared library containing the processor
    char l_libPath[1024];
    void *l_lib;

    processorPath(p_name, l_libPath, sizeof(l_libPath));
    l_lib = ATP_sharedLibLoad(l_libPath);
    if (l_lib != NULL)
    {
        // try to load the "load" function
        ATP_ProcessorLoadCallback load = ATP_sharedLibSymbol(l_lib, "load");
        if (load != NULL)
        {
            ATP_Processor *l_proc = malloc(sizeof(ATP_Processor));
            if (l_proc == NULL)
            {
                PERR();
                ATP_sharedLibUnload(l_lib);
                exit(EX_OSERR);
            }

            if (!load(p_index, p_parameters, &l_proc->m_interface))
            {
                ATP_sharedLibUnload(l_lib);
                free(l_proc);
                return NULL;
            }
            return l_proc;
        }
    }

    // search for it in the staticly linked processors
    for (i = 0; i < gs_staticProcessorCount; ++i)
    {
        if (strcmp(gs_staticProcessors[i].m_name, p_name) == 0)
        {
            ATP_Processor *l_proc = malloc(sizeof(ATP_Processor));
            if (l_proc == NULL)
            {
                PERR();
                exit(EX_OSERR);
            }

            if (!gs_staticProcessors[i].load(p_index, p_parameters, &l_proc->m_interface))
            {
                free(l_proc);
                return NULL;
            }
            return l_proc;
        }
    }

    ERR("No matching processor for '%s' found.\n", p_name);
    return NULL;
}

int ATP_processorRun(ATP_Processor *p_proc, int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output)
{
    if (p_proc != NULL)
    {
        return p_proc->m_interface.run(p_count, p_input, p_output, p_proc->m_interface.m_token);
    }

    ERR("Invalid processor.\n");
    return 0;
}

void ATP_processorUnload(ATP_Processor *p_proc)
{
    if (p_proc != NULL)
    {
        p_proc->m_interface.unload(p_proc->m_interface.m_token);
        free(p_proc);
    }
}

/* Function: ATP_processorsSetHelpFlag
Set the flag indicating that processors should provide help rather than processing data.
*/
void ATP_processorsSetHelpFlag(void)
{
    gs_helpRequested = 1;
}

int ATP_processorHelpRequested(void)
{
    return gs_helpRequested;
}

static void listExternalProcessors(const char *p_pattern)
{
#ifdef _WIN32
    WIN32_FIND_DATA l_data;
    HANDLE l_find = FindFirstFile(p_pattern, &l_data);
    if (l_find == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        char l_name[_MAX_FNAME + 1];
        _splitpath(l_data.cFileName, NULL, NULL, l_name, NULL);
        LOG("    %s\n", l_name);
    }
    while (FindNextFile(l_find, &l_data) != 0);

    FindClose(l_find);
#else // NOTE: assume POSIX for now
    unsigned int i;
    glob_t l_glob;
    if (glob(p_pattern, 0, NULL, &l_glob) != 0)
    {
        return;
    }

    for (i = 0; i < l_glob.gl_pathc; ++i)
    {
        char *l_dot;
        char l_name[1024];
        char *l_file = basename(l_glob.gl_pathv[i]);
        strncpy(l_name, l_file, sizeof(l_name));
        l_name[sizeof(l_name) - 1] = '\0';

        l_dot = strrchr(l_name, '.');
        if (l_dot != NULL)
        {
            *l_dot = '\0';
            LOG("    %s\n", l_name);
        }
    }

    globfree(&l_glob);
#endif
}

/* Function: ATP_processorsList
List the available processors.
*/
void ATP_processorsList(void)
{
    unsigned int i;
    char l_libPattern[1024];

    LOG("External Processors:\n");
    processorPath("*", l_libPattern, sizeof(l_libPattern));
    listExternalProcessors(l_libPattern);

    LOG("Built-in Processors:\n");
    for (i = 0; i < gs_staticProcessorCount; ++i)
    {
        LOG("    %s\n", gs_staticProcessors[i].m_name);
    }
}
