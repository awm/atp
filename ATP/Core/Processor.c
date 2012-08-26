#include "Processor.h"

#include "ATP/Common/SharedLib.h"
#include "ATP/Common/Array.h"
#include "ATP/Common/Exit.h"
#include "ATP/Common/Log.h"

#include <stdlib.h>

#ifndef PREFIX
    #define PREFIX "/usr/local"
#endif

typedef struct StaticProcessor
{
    const char *m_name;
    ATP_ProcessorLoadCallback load;
} StaticProcessor;

static int gs_helpRequested = 0;

// import static processors
extern int help_load(int p_index, const ATP_CmdLineParam *p_parameters, ATP_ProcessorInterface *p_interface);
extern int print_load(int p_index, const ATP_CmdLineParam *p_parameters, ATP_ProcessorInterface *p_interface);

// static processor lookup table
static StaticProcessor gs_staticProcessors[] =
{
    { "help", &help_load },
    { "print", &print_load }
};

static void processorPath(const char *p_name, char *p_buffer, unsigned int p_size)
{
#ifdef _WIN32
    char l_path[MAX_PATH + 1];
    char l_drive[_MAX_DRIVE + 1];
    char l_dir[_MAX_DIR + 1];

    GetModuleFileName(NULL, l_path, MAX_PATH);
    _splitpath(l_path, l_drive, l_dir, NULL, NULL);

    snprintf(p_buffer, p_size, "%s%s\\processors\\%s.atp", l_drive, l_dir, p_name);
#else // NOTE: assume POSIX for now
    snprintf(p_buffer, p_size, PREFIX "/lib/atp/%s.atp", p_name);
#endif
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
    for (i = 0; i < ARRAYLEN(gs_staticProcessors); ++i)
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

int ATP_processorRun(ATP_Processor *p_proc, int p_count, const ATP_Dictionary *p_input, ATP_Dictionary **p_output)
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

/* Function: ATP_processorsList
List the available processors.
*/
void ATP_processorsList(void)
{
    unsigned int i;

    // TODO: print shared library processors

    LOG("Internal Processors:\n");
    for (i = 0; i < ARRAYLEN(gs_staticProcessors); ++i)
    {
        LOG("    %s\n", gs_staticProcessors[i].m_name);
    }
}
