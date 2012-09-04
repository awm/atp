/* File: CommandLine.h
Command line parsing routines for ATP.
*/
#ifndef _ATP_LIBRARY_COMMANDLINE_H_
#define _ATP_LIBRARY_COMMANDLINE_H_

#include "Export.h"
#include "Array.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Function: ATP_commandLineGet
Get the command line parameters for a single processor.

Parameters:
    argc         - The number of entries in argv.
    argv         - The array of command line parameters.
    p_proc       - The index of the processor being handled now.
    p_name       - A pointer to set to the name of the processor, as parsed from the command line.
    p_parameters - An initialized array to fill with the parameters for this processor.

Returns:
    1 if parsing was successful, 0 if it was not.
*/
EXPORT int ATP_commandLineGet(int argc, char **argv, unsigned int p_proc, char **p_name, ATP_Array *p_parameters);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* _ATP_LIBRARY_COMMANDLINE_H_ */
