/* File: Processor.h
Definition of the template processor interface.
*/
#ifndef _ATP_LIBRARY_PROCESSOR_H_
#define _ATP_LIBRARY_PROCESSOR_H_

#include "CommandLine.h"
#include "Export.h"
#include "Dictionary.h"

// forward reference
struct ATP_ProcessorInterface;

/* Callback: ATP_ProcessorLoadCallback
Invoked to initialize the processor.

Parameters:
    p_index      - The index of the processor in the pipeline.
    p_parameters - The command line parameters for the processor.
    p_interface  - An interface structure to be populated by the processor.

Returns:
    1 if the processor was loaded successfully, 0 otherwise.
*/
typedef int (*ATP_ProcessorLoadCallback)(unsigned int p_index, const ATP_Array *p_parameters, struct ATP_ProcessorInterface *p_interface);
/* Callback: ATP_ProcessorRunCallback
Invoked to run the processor.  The processor takes in a dictionary (which may be empty), and uses its contents to populate another dictionary
(which may also be empty).  The processor may also use the command line parameters passed to it in <ATP_ProcessorLoadCallback>, and data loaded
from external sources (such as files) to perform its task.  The processor may also generate data to external sinks (such as files).

Parameters:
    p_count  - The total number of processors loaded.
    p_input  - The dictionary of input data for the processor.  This may be empty.
    p_output - The dictionary of output data from the processor.  This may be left empty.
    p_token  - The value of <ATP_ProcessorInterface.m_token> as set by the processor in the load callback.

Returns:
    1 if the processor ran successfully, 0 if it did not.
*/
typedef int (*ATP_ProcessorRunCallback)(unsigned int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output, void *p_token);
/* Callback: ATP_ProcessorUnloadCallback
Invoked to unload the processor.  This callback should free resources used internally by the processor.

Parameters:
    p_token  - The value of <ATP_ProcessorInterface.m_token> as set by the processor in the load callback.
*/
typedef void (*ATP_ProcessorUnloadCallback)(void *p_token);

typedef struct ATP_StaticProcessor
{
    const char *m_name;
    ATP_ProcessorLoadCallback load;
} ATP_StaticProcessor;

/* Structure: ATP_ProcessorInterface
The interface for interacting with a loaded processor.
*/
typedef struct ATP_ProcessorInterface
{
    /* Variable: m_name
    The name of the processor.
    */
    char m_name[128];
    /* Variable: m_token
    Arbitrary data set by the processor.
    */
    void *m_token;
    /* Callback: run
    See <ATP_ProcessorRunCallback>.
    */
    ATP_ProcessorRunCallback run;
    /* Callback: unload
    See <ATP_ProcessorUnloadCallback>.
    */
    ATP_ProcessorUnloadCallback unload;
} ATP_ProcessorInterface;

/* Structure: ATP_Processor
Representation of a loaded template processor.
*/
typedef struct ATP_Processor
{
    /* Variable: m_interface
    The processor interface for this instance.
    */
    ATP_ProcessorInterface m_interface;
    /* Variable: next
    The next processor in the pipeline.
    */
    struct ATP_Processor *next;
} ATP_Processor;

EXPORT void ATP_processorsSetStatic(ATP_StaticProcessor p_table[], unsigned int p_count);

/* Function: ATP_processorLoad
Load a template processor and provide it with any available command line arguments.

Parameters:
    p_index      - The index number indicating the position of this processor in the pipeline.
    p_name       - The name of the processor.  This is used to find and load the library containing the processor.
    p_parameters - The command line parameters for the processor.  The processor is responsible for parsing them further, as necessary.

Returns:
    A new processor instance.
*/
EXPORT ATP_Processor *ATP_processorLoad(unsigned int p_index, const char *p_name, const ATP_Array *p_parameters);
/* Function: ATP_processorRun
Run the processor, handling the given input and producing output.

Parameters:
    p_proc   - The processor instance.
    p_count  - The total number of processors loaded.
    p_input  - A dictionary containing all of the input data for the processor.  This may be empty.
    p_output - An empty dictionary to fill with processor.  This may be left empty.

Returns:
    1 if the processor ran successfully, 0 if it did not.
*/
EXPORT int ATP_processorRun(ATP_Processor *p_proc, unsigned int p_count, ATP_Dictionary *p_input, ATP_Dictionary *p_output);
/* Function: ATP_processorUnload
Unload the processor, freeing resources as necessary and deleting the processor instance structure.

Parameters:
    p_proc   - The processor instance.
*/
EXPORT void ATP_processorUnload(ATP_Processor *p_proc);

/* Function: ATP_processorHelpRequested
Determine if help rather than actual data processing is request.  Processors should check this flag when they run to determine if they
should display help rather than process incoming data.

Returns:
    1 if help is requested, 0 if it is not.
*/
EXPORT int ATP_processorHelpRequested(void);

#endif /* _ATP_LIBRARY_PROCESSOR_H_ */
