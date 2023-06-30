#include "parseropt.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// defines --------------------

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// variables --------------------

const PsrHelpConfig_t DEFAULT_PSR_CONFIG = {
    .indent     =  2   ,
    .sep        =  ", ",
    .margin     =  8   ,
    .desc_width = 60   ,
};

// prototype declarations --------------------

int __isPsrDescEnd(const PsrDescription_t *desc);
int __printHDescDescriotion(const char *s, int desc_indent, int width);

// functions --------------------

/**
 * @brief Print help message with config.
 * @param options Option list.
 * @param descs Description list.
 * @param usage command usage: e.g. "command [options...] source target"
 * @param prefix prifix text. Written before the description.
 * @param prefix suffix text. Written at the end.
 * @param config Help message config.
 **/
// usage, prefix, suffix is nullable
void psrHelpWithConfig(const PsrArgumentObject_t *options, const PsrDescription_t *descs, const char *usage, const char *prefix, const char *suffix, const PsrHelpConfig_t *config)
{
    if (usage == NULL)
    {
        fprintf(stderr, "failed to print help.\n");
        return;
    }

    // usage
    printf("Usage: %s\n", usage);

    // prefix
    if (prefix != NULL)
    {
        printf("\n%s\n\n", prefix);
    }

    // options
    psrHDescWithConfig(options, descs, config);

    // note
    psrHOptionNote();

    // suffix
    if (suffix != NULL)
    {
        printf("\n%s\n", suffix);
    }
}


/**
 * @brief Print description (help message item) with config.
 * @param options Option list.
 * @param descs Description list.
 * @param config Help message config.
 **/
void psrHDescWithConfig(const PsrArgumentObject_t *options, const PsrDescription_t *descs, const PsrHelpConfig_t *config)
{
    int swidth = 0;
    int lwidth = 0;
    const unsigned int indent = config->indent;
    const char *sep = config->sep;
    const unsigned int margin = config->margin;
    const unsigned int desc_width = config->desc_width;
    int desc_total_indent;

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

    // set desc indent size
    desc_total_indent = indent + swidth + (swidth > 0 && lwidth > 0 ? strlen(sep) : 0) + lwidth + margin;

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
        if (__printHDescDescriotion(descs[desc_idx].desc, desc_total_indent, config->desc_width))
        {
            fprintf(stderr, "Error: Failed to print description.");
            return;
        }
    }
}


/**
 * @brief Print description.
 * @param s Description string.
 * @param desc_indent description indent size.
 **/
int __printHDescDescriotion(const char *s, int desc_indent, int width)
{
    char *current = (char *)s;
    int len = 0, i = 0;
    bool lf_returned = false;
    bool prev_space = false;

    if (s == NULL || width < 2)
    {
        return 1;
    }

    // get last index
    char *last = current + strlen(current) - 1;
    while(last >= current && last[0] == ' ')
    {
        last--;
    }

    while (1)
    {
        if (i == 0 && prev_space && current[i] == ' ')
        {
            current++;
            continue;
        }

        if (i == 0 && current > s && current[i] != '\n')
        {
            // indent
            printf("%*s", desc_indent, "");
        }

        // word sep / LF / last character detected
        if ((!lf_returned && current[i] == ' ') || current[i] == '\n' || &current[i] >= last)
        {
            // not width overflow
            if (i <= width)
            {
                // is end character
                if (&current[i] >= last)
                {
                    // end
                    printf("%.*s\n", (int)(last - current + 1), current);
                    break;
                }

                // is \n
                if ((lf_returned = (current[i] == '\n')))
                {
                    printf("%.*s\n", i, current);

                    // newline
                    current = &current[i + 1];
                    i = 0;
                    len = 0;
                }
                // is space
                else
                {
                    // set length without bottom space
                    if (!prev_space)
                    {
                        len = i;
                        prev_space = true;
                    }
                    // next index
                    i++;
                }
            }
            // width overflow
            else
            {
                // If the number of characters in a word exceeds the specified width
                if (len == 0)
                {
                    printf("%.*s-\n", width - 1, current);
                    current = &current[width - 1];
                }
                // normally auto newline
                else
                {
                    printf("%.*s\n", len, current);
                    current = &current[len + 1];
                    prev_space = true;
                }

                // newline
                i = 0;
                len = 0;
            }
        }
        // non-specific character
        else
        {
            // next index
            lf_returned = false;
            prev_space = false;
            i++;
        }
    }

    return 0;
}


/**
 * @brief Print note about option.
 **/
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
