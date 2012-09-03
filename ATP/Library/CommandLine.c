#include "CommandLine.h"
#include "Exit.h"
#include "Log.h"

#include <stdlib.h>

#define c_procDelimiter '@'

int ATP_commandLineGet(int argc, char **argv, unsigned int p_proc, char **p_name, ATP_Array *p_parameters)
{
    int i;
    unsigned int l_count = 0;

    DBG("Request for parameters for processor %d\n", p_proc);
    for (i = 1; i < argc; ++i)
    {
        DBG("Searching arguments for processors, argv[%u] = \"%s\"\n", i, argv[i]);
        if (argv[i][0] == c_procDelimiter)
        {
            if (l_count == p_proc)
            {
                int j;

                *p_name = &argv[i][1];
                for (j = i + 1; j < argc; ++j)
                {
                    if (argv[j][0] == c_procDelimiter)
                    {
                        return 1;
                    }

                    if (!ATP_arraySetString(p_parameters, ATP_arrayLength(p_parameters), argv[j]))
                    {
                        return 0;
                    }
                }

                return 1;
            }

            ++l_count;
        }
    }

    return 0;
}
