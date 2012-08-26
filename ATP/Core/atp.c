#include "CommandLine.h"
#include "Processor.h"

#include "ATP/Common/Exit.h"
#include "ATP/Common/Log.h"

#include <stdlib.h>
#include <string.h>

static void cleanupParams(ATP_CmdLineParam *p_parameters)
{
    ATP_CmdLineParam *it = NULL;
    ATP_CmdLineParam *l_tmp = NULL;

    if (p_parameters != NULL)
    {
        LL_FOREACH_SAFE(p_parameters, it, l_tmp)
        {
            LL_DELETE(p_parameters, it);
            free(it);
        }
    }
}

static void cleanupProcessors(ATP_Processor *p_processors)
{
    ATP_Processor *it = NULL;
    ATP_Processor *l_tmp = NULL;

    if (p_processors != NULL)
    {
        LL_FOREACH_SAFE(p_processors, it, l_tmp)
        {
            LL_DELETE(p_processors, it);
            ATP_processorUnload(it);
        }
    }
}

int main(int argc, char **argv)
{
    int l_count = 0;
    char *l_name = NULL;
    ATP_CmdLineParam *l_parameters = NULL;
    ATP_Processor *l_processors = NULL;
    ATP_Processor *it = NULL;
    ATP_Dictionary *l_input = NULL;
    ATP_Dictionary *l_output = NULL;
    char l_pipeline[1024] = "";

    if (argc > 1)
    {
        LOG("Starting up...\n");
    }

    // parse command line and load processors
    while (ATP_commandLineGet(argc, argv, l_count, &l_name, &l_parameters))
    {
        ATP_Processor *l_proc;
        strncat(l_pipeline, (l_count > 0 ? " > " : " "), sizeof(l_pipeline) - strlen(l_pipeline) - 1);
        strncat(l_pipeline, l_name, sizeof(l_pipeline) - strlen(l_pipeline) - 1);

        l_proc = ATP_processorLoad(l_count, l_name, l_parameters);
        if (l_proc == NULL)
        {
            cleanupParams(l_parameters);
            cleanupProcessors(l_processors);
            return EX_USAGE;
        }
        LL_APPEND(l_processors, l_proc);

        ++l_count;
        cleanupParams(l_parameters);
    }
    if (l_count == 0)
    {
        // load help if nothing else is specified
        ATP_Processor *l_proc = ATP_processorLoad(l_count++, "help", NULL);
        if (l_proc == NULL)
        {
            cleanupProcessors(l_processors);
            return EX_USAGE;
        }
        LL_APPEND(l_processors, l_proc);
    }
    if (argc > 1)
    {
        LOG("Requested pipeline: %s\n", l_pipeline);
    }

    // now run the processors in order
    LL_FOREACH(l_processors, it)
    {
        if (!ATP_processorRun(it, l_count, l_input, &l_output))
        {
            // just return, since the processor should log its own error
            cleanupProcessors(l_processors);
            // TODO: delete dictionaries
            return EX_SOFTWARE;
        }
        // TODO: delete input dictionary
        l_input = l_output;
    }
    // TODO: delete output dictionary

    // clean up and quit
    cleanupProcessors(l_processors);
    return EX_OK;
}
