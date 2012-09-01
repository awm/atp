# ATP

## Usage

    atp @<processor> [processor args] [@<processor> [processor args] ...]

## Examples

Display ATP help:

    atp @help

Display help for the `random` and `print` processors:

    atp @help @random @print

Generate a random dictionary and print it:

    atp @random 5 10 2 @print

Load JSON file `some_data.json` and use its data together with the ctemplate template `my_template.tpl` to produce `output.txt`:

    atp @json some_data.json @ctemplate my_template.tpl output.txt
