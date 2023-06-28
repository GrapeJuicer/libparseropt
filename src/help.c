#include "parseropt.h"

#include <stdio.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


const PsrHelpConfig_t DEFAULT_PSR_CONFIG = {
    .indent     =  2,
    .sep     =  ", ",
    .margin     =  8,
    .desc_width = 60,
};


int __isPsrDescEnd(const PsrDescription_t *desc);


// usage, prefix, suffix is nullable
void psrHelpWithConfig(int argc, const char **argv, const PsrArgumentObject_t *options, const PsrDescription_t *descs, const char *usage, const char *prefix, const char *suffix, const PsrHelpConfig_t *config)
{
    // usage
    if (usage == NULL)
    {
        // printf("");
    }
    else
    {
        printf("Usage: %s\n\n", usage);
    }

    // prefix
    if (prefix != NULL)
    {
        printf("%s\n\n", prefix);
    }

    // options
    psrHDescWithConfig(options, descs, config);

    // suffix
    if (suffix != NULL)
    {
        printf("%s\n", suffix);
    }
}


void psrHDescWithConfig(const PsrArgumentObject_t *options, const PsrDescription_t *descs, const PsrHelpConfig_t *config)
{
    int swidth = 0;
    int lwidth = 0;
    const unsigned int indent = config->indent;
    const char *sep = config->sep;
    const unsigned int margin = config->margin;
    const unsigned int desc_width = config->desc_width;

    if (sep == NULL)
    {
        // fail
        return;
    }

    // |    -a, --alpha        This is a description.
    //  <--> indent
    //        <> sep
    //                 <------> margin
    //                         <---------------------------------> desc_width

    // header
    printf("Options:\n");

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

        // option separator
        if (swidth > 0 && lwidth > 0)
        {
            if (sexist && lexist)
            {
                printf("%s", sep);
            }
            else
            {
                printf("%*s", (int)strlen(sep), "");
            }
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
