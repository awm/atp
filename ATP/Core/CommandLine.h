/* File: CommandLine.h
Command line parsing routines for ATP.
*/
#ifndef _ATP_CORE_COMMANDLINE_H_
#define _ATP_CORE_COMMANDLINE_H_

#include "ATP/External/UT/utlist.h"

/* Structure: ATP_CmdLineParam
Linked list item for a single command line parameter.
*/
typedef struct ATP_CmdLineParam
{
    /* Variable: m_parameter
    The command line parameter.
    */
    const char *m_parameter;
    /* Variable: next
    The next item in the linked list.
    */
    struct ATP_CmdLineParam *next;
} ATP_CmdLineParam;

/* Function: ATP_commandLineGet
Get the command line parameters for a single processor.

Parameters:
    argc         - The number of entries in argv.
    argv         - The array of command line parameters.
    p_proc       - The index of the processor being handled now.
    p_name       - A pointer to set to the name of the processor, as parsed from the command line.
    p_parameters - A pointer to set to the linked list of parameters for this processor.

Returns:
    1 if parsing was sucessful, 0 if it was not.
*/
int ATP_commandLineGet(int argc, char **argv, int p_proc, char **p_name, ATP_CmdLineParam **p_parameters);

#endif /* _ATP_CORE_COMMANDLINE_H_ */
