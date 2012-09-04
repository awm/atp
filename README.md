# ATP

This document contains some preliminary musings and may not accurately reflect the state of the code at the moment.

## License

Licensed under the terms of the [Modified BSD License](http://opensource.org/licenses/BSD-3-Clause).  See LICENSE.txt for details.

## Usage

    atp @<processor> [processor args] [@<processor> [processor args] ...]

The `ATP_PROCESSOR_PATH` environment variable may be used to specify multiple alternative directories to search for processors in, overriding the built-in default.  The paths are separated by colons.  Empty paths are ignored.

## Examples

Display ATP help:

    atp @help

Display help for the `random` and `json` processors:

    atp @help @random @json

Generate a random dictionary and print as JSON to stdout:

    atp @random 5 10 2 @json write stdout

Load JSON file `some_data.json` and use its data together with the [ctemplate](http://code.google.com/p/ctemplate/) template `my_template.tpl` to produce `output.txt`:

    atp @json read some_data.json @ctemplate my_template.tpl output.txt
