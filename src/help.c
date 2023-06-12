#include "parseropt.h"

#include <stdio.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


int __isPsrDescEnd(const PsrDescription_t *desc);


void psrHDesc(const PsrArgumentObject_t *options, const PsrDescription_t *descs)
{
    int swidth = 0;
    int lwidth = 0;
    const int indent = 2;
    const int margin = 8;
    // const desc_width = 60;

    // |    -a, --alpha        This is a description.
    //  <--> indent
    //                 <------> margin
    //                         <---------------------------------> desc_width

    // calc width
    for (int i = 0; isPsrArgumentEnd(&options[i]) == 0; i++)
    {
        if (options[i].short_opt != NONE_SHORT_OPT)
        {
            // -a           ... no arg      2
            // -b ARG       ... req arg     6
            // -c[ARG]     ... opt arg     7
            switch (options[i].has_arg)
            {
            case NO_ARGUMENT:
                swidth = MAX(swidth, 2);
                break;
            case REQUIRE_ARGUMENT:
                swidth = MAX(swidth, 6);
                break;
            case OPTIONAL_ARGUMENT:
                swidth = MAX(swidth, 7);
                break;
            default:
                return;
            }
        }
        if (strcmp(options[i].long_opt, NONE_LONG_OPT))
        {
            // --alpha           ... no arg      n + 2
            // --alpha ARG       ... req arg     n + 6
            // --alpha=[ARG]     ... opt arg     n + 8
            int tmp = strlen(options[i].long_opt);
            switch (options[i].has_arg)
            {
            case NO_ARGUMENT:
                lwidth = MAX(lwidth, tmp + 2);
                break;
            case REQUIRE_ARGUMENT:
                lwidth = MAX(lwidth, tmp + 6);
                break;
            case OPTIONAL_ARGUMENT:
                lwidth = MAX(lwidth, tmp + 8);
                break;
            default:
                return;
            }
        }
    }

    for (int i = 0; isPsrArgumentEnd(&options[i]) == 0; i++)
    {
        int sexist = options[i].short_opt != NONE_SHORT_OPT;
        int lexist = strcmp(options[i].long_opt, NONE_LONG_OPT);

        // find description
        int desc_idx = -1;
        for (int j = 0; __isPsrDescEnd(&descs[j]) == 0; j++)
        {
            if (descs[j].id == options[i].id && descs[j].desc != NULL)
            {
                desc_idx = j;
                break;
            }
        }
        if (desc_idx == -1)
        {
            // do not show anything if description is not found.
            continue;
        }

        // indent
        printf("%*s", indent, "");

        // short option
        if (swidth > 0)
        {
            if (sexist)
            {
                // -a
                // -a ARG
                // -a[ARG]
                printf(
                    "-%c%-*s",
                    options[i].short_opt,
                    swidth - 2,
                    (options[i].has_arg == REQUIRE_ARGUMENT ? " ARG" : (options[i].has_arg == OPTIONAL_ARGUMENT ? "[ARG]" : ""))
                );
            }
            else
            {
                // none
                printf("%*s", swidth, "");
            }
        }

        // comma
        if (swidth > 0 && lwidth > 0)
        {
            printf("%s", (sexist && lexist ? ", " : "  "));
        }

        // long option
        if (lwidth > 0)
        {
            if (lexist)
            {
                // --alpha
                // --alpha ARG
                // --alpha=[ARG]
                printf(
                    "--%s%-*s",
                    options[i].long_opt,
                    lwidth - (int)strlen(options[i].long_opt) - 2,
                    (options[i].has_arg == REQUIRE_ARGUMENT ? " ARG" : (options[i].has_arg == OPTIONAL_ARGUMENT ? "=[ARG]" : ""))
                );
            }
            else
            {
                // none
                printf("%*s", lwidth, "");
            }
        }

        // margin
        printf("%*s", margin, "");

        // description
        //      future function: override '\n' to support newline in description.
        printf("%s\n", descs[desc_idx].desc);
    }
}


void psrHOptionNote(void)
{
    printf("It is also possible to specify the options in the following:\n");
    printf("  -a ARG       --> -aARG\n");
    printf("  --alpha ARG  --> --alpha=ARG\n");
}


/**
 * @brief Determines if it is the last element in the descriptions
 * @param options Item's address of descriptions. Not array.
 * @return End: 1 / Not end: 0
 **/
int __isPsrDescEnd(const PsrDescription_t *desc)
{
    return (desc->id == NONE_ID && desc->desc == NONE_DESC) ? 1 : 0;
}
