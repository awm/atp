#include "ATP/Library/Processor.h"
#include "ATP/Library/Log.h"

#include <stdlib.h>

#ifndef VERSION
    #define VERSION "DEV"
#endif
#ifndef REVISION
    #define REVISION "0"
#endif

extern void ATP_processorsSetHelpFlag(void);
extern void ATP_processorsList(void);

static void usage(void)
{
    LOG(
"atp v" VERSION " (" REVISION ") " __DATE__ "\n\n");
    LOG(
"Usage: atp @<processor> [processor args] [@<processor> [processor args] ...]\n\n");

    ATP_processorsList();
}

static int run(int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token)
{
    if (p_count == 1)
    {
        // display general usage information if help is the only loaded processor
        usage();
    }

    *p_output = NULL;
    return 1;
}

static void unload(void *p_token)
{
    // do nothing
}

int help_load(int p_index, const ATP_CmdLineParam *p_parameters, struct ATP_ProcessorInterface *p_interface)
{
    p_interface->m_token = NULL;
    p_interface->run = &run;
    p_interface->unload = &unload;

    ATP_processorsSetHelpFlag();
    return 1;
}
