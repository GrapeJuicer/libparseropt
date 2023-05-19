#include "parseropt.h"
#include <string.h>
#include <stdio.h>

// prototype declarations --------------------

int __shortOptionIndex (char s, const PsrArgumentObject_t *options);
int __isPsrArgumentEnd (const PsrArgumentObject_t *options);
int __slideArgument    (int argc, char *argv[], size_t index, size_t n);


// void setPriority(int argc, char **argv, PsrArgumentObject_t *options)
// {
// }


/**
 * @brief Parse the arguments of main() function and return one of argument infomation.
 * @param argc `argv` array length.
 * @param argv Commandline parameter.
 * @param options Option list.
 * @param optarg Found option's argument. If option has no argument, this value is "".
 * @param optind Parsed parameter index. After parsed, this means the head of the non-option's parameter. At first, this must be 0.
 * @return Found option's ID (= PsrArgumentObject_t.id). If something occured, this returns "Rsr Result" item.
 **/
int persoropt(int argc, char **argv, const PsrArgumentObject_t *options, char optarg[PSR_BUF_SIZE], int *optind)
{
    int p = *optind;
    const size_t OPT_HEADER_LEN_S = strlen(OPT_HEADER_SHORT);
    const size_t OPT_HEADER_LEN_L = strlen(OPT_HEADER_LONG);
    int param_cnt;
    int idx;
    size_t arg_length;
    char *param;
    char buf[PSR_BUF_SIZE] = "";
    int ret;

    // out of range
    if (p < 0)
    {
        return PSR_ERROR;
    }

    // end
    if (p >= argc)
    {
        return PSR_NOT_FOUND;
    }

    // skip argv[0]
    if (p == 0)
    {
        p = 1;
        *optind = 1;
    }

    // find option
    while (p < argc)
    {
        arg_length = strlen(argv[p]);
        // is valid short option format
        if (!strncmp(argv[p], OPT_HEADER_SHORT, OPT_HEADER_LEN_S) && arg_length > OPT_HEADER_LEN_S)
        {
            // is valid option
            idx = __shortOptionIndex(argv[p][OPT_HEADER_LEN_S], options);

            if (idx < 0)
            {
                // invalid option
                return PSR_UNKNOWN_OPTION;
            }

            // get width and option's argument
            switch (options[idx].has_arg)
            {
            case NO_ARGUMENT:
                param_cnt = 1;
                if (arg_length != OPT_HEADER_LEN_S + 1)
                {
                    return PSR_NO_ARG_HAS_ARG;
                }
                break;
            case REQUIRE_ARGUMENT:
                param_cnt = (arg_length == OPT_HEADER_LEN_S + 1 ? 2 : 1);
                if (param_cnt == 2)
                {
                    if (p + 1 >= argc)
                    {
                        // last element
                        return PSR_REQ_ARG_HAS_NO_ARG;
                    }
                    strcpy(buf, argv[p + 1]);
                }
                else // param_cnt == 1
                {
                    strcpy(buf, &argv[p][OPT_HEADER_LEN_S + 1]);
                }
                break;
            case OPTIONAL_ARGUMENT:
                param_cnt = 1;
                if (arg_length > OPT_HEADER_LEN_S + 1)
                {
                    // -aXXX
                    strcpy(buf, &argv[p][OPT_HEADER_LEN_S + 1]);
                }
                break;
            default:
                return PSR_ERROR;
            }

            break;
        }
        // is valid long option format
        else if (!strncmp(argv[p], OPT_HEADER_LONG, OPT_HEADER_LEN_L) && arg_length > OPT_HEADER_LEN_L)
        {
            // is valid option
        }
        // not option
        else
        {
            // next element
            p++;
            // no more arguments
            if (p >= argc)
            {
                return PSR_NOT_FOUND;
            }
        }
    }

    // slide arguments
    // case.1
    // aaa.exe A B -a -b   ------------>   aaa.exe -a A B -b
    //         ^    ^  ^                            ^ ^
    //      optind  p  p+param_cnt             optind optind+param_cnt...next-optind
    //
    // case.2
    // aaa.exe A B -a X -b   ------------>   aaa.exe -a X A B -b
    //         ^    ^    ^                            ^   ^
    //      optind  p    p+param_cnt             optind   optind+param_cnt...next-optind
    //
    // case.3
    // aaa.exe A B -aX -b   ------------>   aaa.exe -aX A B -b
    //         ^    ^   ^                            ^  ^
    //      optind  p   p+param_cnt             optind  optind+param_cnt...next-optind
    int i;
    for (i = 0; i < param_cnt; i++)
    {
        if (__slideArgument(argc, argv, p + i, p - *optind))
        {
            return PSR_ERROR;
        }
    }

    // get id and option's argument
    ret = options[idx].id;
    // update optind
    *optind += param_cnt;
    // update optarg
    strcpy(optarg, buf);

    // callfunc
    if (options[idx].callfunc != NULL)
    {
        options[idx].callfunc(strcmp(optarg, "") == 0 ? NULL : optarg);
    }

    return ret;
}


/**
 * @brief Get option index which is matched with variable `s`.
 * @param s Short option you want to search.
 * @param options Option list.
 * @return Found: Matched option's index / Not Found: -1
 **/
int __shortOptionIndex(char s, const PsrArgumentObject_t *options)
{
    int i;
    for (i = 0; __isPsrArgumentEnd(&options[i]) == 0; i++)
    {
        if (s == options[i].short_opt)
        {
            // found
            return i;
        }
    }
    // not found
    return -1;
}


/**
 * @brief Determines if it is the last element in the options
 * @param options Item's address of options. Not array.
 * @return End: 1 / Not end: 0
 **/
int __isPsrArgumentEnd(const PsrArgumentObject_t *options)
{
    return (options->id        == PSR_ARG_NONE_ID                 &&
            options->short_opt == PSR_ARG_NONE_SHORT_OPT          &&
            strcmp(options->long_opt, PSR_ARG_NONE_LONG_OPT) == 0 &&
            options->has_arg   == PSR_ARG_NONE_HAS_ARG            &&
            options->priority  == PSR_ARG_NONE_PRIORITY           &&
            options->callfunc  == PSR_ARG_NONE_CALLFUNC) ? 1 : 0;
}


/**
 * @brief Slide argument to left. e.g. (p = optind+2) EXE A B -a -b -----> EXE -a A B -b
 * @param argc `argv` array length.
 * @param argv Commandline parameter.
 * @param index Index of the argument item you want to slide.
 * @param n Slide step size.
 * @return 0: success / 1: failure
 **/
int __slideArgument(int argc, char *argv[], size_t index, size_t n)
{
    if (index == 0 || index >= argc || n + 1 > index)
    {
        return 1;
    }

    int i;
    for (i = 0; i < n; i++)
    {
        char *tmp = argv[index - i];
        argv[index - i] = argv[index - i - 1];
        argv[index - i - 1] = tmp;
    }

    return 0;
}
