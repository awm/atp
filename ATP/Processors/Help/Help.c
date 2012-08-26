#include "ATP/Core/Processor.h"
#include "ATP/Common/Log.h"

#include <stdlib.h>

#ifndef VERSION
    #define VERSION "DEV"
#endif
#ifndef REVISION
    #define REVISION "0"
#endif

extern void ATP_processorsSetHelpFlag(void);

static void usage(void)
{
    LOG("atp v%s %s (%s)\n\n", VERSION, __DATE__, REVISION);
    LOG("Usage: atp @<processor> [processor args] [@<processor> [processor args] ...]\n");
}

static int run(int p_count, const ATP_Dictionary *p_input, ATP_Dictionary **p_output, void *p_token)
{
    if (p_count == 0)
    {
        usage();
    }
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
