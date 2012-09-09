Hello {{NAME}}!

This is a very basic test.

The following section does not exist:

{{#NOT_FOUND}}
I will never be printed.
{{/NOT_FOUND}}

The following section will appear three times:
{{#ITEMS}}
 * This is the last item: {{IS_LAST}}
{{/ITEMS}}

The following section will appear once:
{{#EMPTY_SECTION}}
I will be printed once.
{{/EMPTY_SECTION}}
