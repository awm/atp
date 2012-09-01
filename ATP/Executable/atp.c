#include "ATP/Library/CommandLine.h"
#include "ATP/Library/Processor.h"
#include "ATP/Library/Array.h"
#include "ATP/Library/Exit.h"
#include "ATP/Library/Log.h"

#include <stdlib.h>
#include <string.h>

// import static processors
extern int help_load(int p_index, const ATP_CmdLineParam *p_parameters, ATP_ProcessorInterface *p_interface);
extern int random_load(int p_index, const ATP_CmdLineParam *p_parameters, ATP_ProcessorInterface *p_interface);

// static processor lookup table
static ATP_StaticProcessor gs_staticProcessors[] =
{
    { "help", &help_load },
    { "random", &random_load },
};

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
    ATP_Dictionary l_input;
    ATP_Dictionary l_output;
    char l_pipeline[1024] = "";

    if (argc > 1)
    {
        LOG("Starting up...\n");
    }

    // parse command line and load processors
    ATP_processorsSetStatic(gs_staticProcessors, ARRAYLEN(gs_staticProcessors));
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
    ATP_dictionaryInit(&l_input);
    ATP_dictionaryInit(&l_output);
    LL_FOREACH(l_processors, it)
    {
        int l_result = ATP_processorRun(it, l_count, &l_input, &l_output);
        if (l_input != l_output)
        {
            // input is no longer needed
            ATP_dictionaryDestroy(&l_input);
        }
        if (!l_result)
        {
            // just return, since the processor should log its own error
            ATP_dictionaryDestroy(&l_output);
            cleanupProcessors(l_processors);
            return EX_SOFTWARE;
        }

        l_input = l_output;
        ATP_dictionaryInit(&l_output);
    }

    // clean up and quit
    ATP_dictionaryDestroy(&l_output);
    cleanupProcessors(l_processors);
    return EX_OK;
}
