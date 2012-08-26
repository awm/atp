#include "ATP/Core/Processor.h"
#include "ATP/Common/Log.h"

#include <stdlib.h>

static int run(int p_count, const ATP_Dictionary *p_input, ATP_Dictionary **p_output, void *p_token)
{
    LOG("Running print...\n");
    return 1;
}

static void unload(void *p_token)
{
    // do nothing
}

#ifdef ATTR_STATIC_PROCESSORS
int print_load(int p_index, const ATP_CmdLineParam *p_parameters, struct ATP_ProcessorInterface *p_interface)
#else
int load(int p_index, const ATP_CmdLineParam *p_parameters, struct ATP_ProcessorInterface *p_interface)
#endif
{
    p_interface->m_token = NULL;
    p_interface->run = &run;
    p_interface->unload = &unload;
    return 1;
}
