#include "CommandLine.h"

#include "ATP/Common/Exit.h"
#include "ATP/Common/Log.h"

#include <stdlib.h>

#define c_procDelimiter '@'

int ATP_commandLineGet(int argc, char **argv, int p_proc, char **p_name, ATP_CmdLineParam **p_parameters)
{
    int i;
    int l_count = 0;

    DBG("Request for parameters for processor %d\n", p_proc);
    *p_parameters = NULL;
    for (i = 1; i < argc; ++i)
    {
        DBG("Searching arguments for processors, argv[%d] = \"%s\"\n", i, argv[i]);
        if (argv[i][0] == c_procDelimiter)
        {
            if (l_count == p_proc)
            {
                int j;

                *p_name = &argv[i][1];
                for (j = i + 1; j < argc; ++j)
                {
                    ATP_CmdLineParam *l_item;
                    if (argv[j][0] == c_procDelimiter)
                    {
                        return 1;
                    }

                    l_item = malloc(sizeof(ATP_CmdLineParam));
                    if (l_item == NULL)
                    {
                        PERR();
                        exit(EX_OSERR);
                    }
                    l_item->m_parameter = argv[j];

                    LL_APPEND(*p_parameters, l_item);
                }

                return 1;
            }

            ++l_count;
        }
    }

    return 0;
}
