#include "parseropt.h"
#include <string.h>
#include <stdio.h>

// variables --------------------

static char OPT_HEADER_LONG[PSR_BUF_SIZE] = OPT_DEFAULT_HEADER_LONG;
static char OPT_HEADER_SHORT[PSR_BUF_SIZE] = OPT_DEFAULT_HEADER_SHORT;

// prototype declarations --------------------

int __shortOptionIndex (char s, const PsrArgumentObject_t *options);
int __isPsrArgumentEnd (const PsrArgumentObject_t *options);
int __slideArgument    (int argc, char *argv[], size_t index, size_t n);
int __longOptionIndex  (char *s, const PsrArgumentObject_t *options);


// void setPriority(int argc, char **argv, PsrArgumentObject_t *options)
// {
// }



/**
 * @brief Set header string.
 * @param short New short option header string. If you don't set new header, set NULL to argument.
 * @param long New long option header string. If you don't set new header, set NULL to argument.
 * @return 0: success / 1: failure
 **/
int setHeader(char *__short, char *__long)
{
    if (__short != NULL && strlen(__short) > 0)
    {
        strcpy(OPT_HEADER_SHORT, __short);
    }

    if (__long != NULL && strlen(__long) > 0)
    {
        strcpy(OPT_HEADER_LONG, __long);
    }
}


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
    int param_cnt = 0;
    int idx;
    size_t arg_length;
    char *param;
    char buf[PSR_BUF_SIZE] = "";

    // out of range
    if (p < 0)
    {
        return PSR_ERROR;
    }

    // skip argv[0]
    if (p == 0)
    {
        p = 1;
        *optind = 1;
    }

    // no more arguments
    if (p >= argc)
    {
        // end: success
        return PSR_NOT_FOUND;
    }

    // find option
    while (p < argc)
    {
        arg_length = strlen(argv[p]);
        // is valid long option format
        if (strcmp(argv[p], OPT_HEADER_SHORT) && !strncmp(argv[p], OPT_HEADER_LONG, OPT_HEADER_LEN_L) && arg_length > OPT_HEADER_LEN_L)
        {
            // is valid option

            // search option
            char tmp[PSR_BUF_SIZE] = "";
            char *pa = strchr(argv[p] + OPT_HEADER_LEN_L, '=');
            if (pa == NULL)
            {
                // --alpha
                // --alpha XXX
                strcpy(tmp, argv[p] + OPT_HEADER_LEN_L);
            }
            else
            {
                // --alpha=XXX
                strncpy(tmp, argv[p] + OPT_HEADER_LEN_L, pa - argv[p] - OPT_HEADER_LEN_L);
            }

            // tmp is long option name
            idx = __longOptionIndex(tmp, options);

            if (idx < 0)
            {
                // invalid option
                *optind = p;
                return PSR_UNKNOWN_OPTION;
            }

            switch (options[idx].has_arg)
            {
            case NO_ARGUMENT:
                param_cnt = 1;
                if (pa != NULL)
                {
                    *optind = p;
                    return PSR_NO_ARG_HAS_ARG;
                }
                break;
            case REQUIRE_ARGUMENT:
                // OK  exe --alpha XXX   pa=null  param_cnt=2
                // OK  exe --alpha=XXX   pa=8     param_cnt=1
                // NG  exe --alpha       pa=null  param_cnt=2
                // NG  exe --alpha=      pa=8     param_cnt=1
                param_cnt = (pa == NULL ? 2 : 1);

                if (param_cnt == 1)
                {
                    // exe --alpha=
                    if (strlen(pa + 1) == 0)
                    {
                        *optind = p;
                        return PSR_REQ_ARG_HAS_NO_ARG;
                    }
                    // exe --alpha=XXX
                    strcpy(buf, pa + 1);
                }
                else // param_cnt == 2
                {
                    // exe --alpha
                    if (p + 1 >= argc)
                    {
                        *optind = p;
                        return PSR_REQ_ARG_HAS_NO_ARG;
                    }
                    // exe --alpha XXX
                    strcpy(buf, argv[p + 1]);
                }
                break;
            case OPTIONAL_ARGUMENT:
                param_cnt = 1;
                // --alpha=XXX, --alpha=
                if (pa != NULL)
                {
                    // --alpha=
                    if (strlen(pa + 1) == 0)
                    {
                        *optind = p;
                        return PSR_OPT_ARG_HAS_ONLY_EQ;
                    }
                    // --alpha=XXX
                    strcpy(buf, pa + 1);
                }
                // else --alpha
                break;
            default:
                *optind = p;
                return PSR_ERROR;
            }

            break;
        }
        // is valid short option format
        else if (strcmp(argv[p], OPT_HEADER_LONG) && !strncmp(argv[p], OPT_HEADER_SHORT, OPT_HEADER_LEN_S) && arg_length > OPT_HEADER_LEN_S)
        {
            // is valid option
            idx = __shortOptionIndex(argv[p][OPT_HEADER_LEN_S], options);

            if (idx < 0)
            {
                // invalid option
                *optind = p;
                return PSR_UNKNOWN_OPTION;
            }

            // get size and option's argument
            switch (options[idx].has_arg)
            {
            case NO_ARGUMENT:
                param_cnt = 1;
                if (arg_length != OPT_HEADER_LEN_S + 1)
                {
                    *optind = p;
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
                        *optind = p;
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
        // not option
        else
        {
            // next element
            p++;
            // no more arguments
            if (p >= argc)
            {
            // end: success
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

    // update optind
    *optind += param_cnt;
    // update optarg
    strcpy(optarg, buf);

    // callfunc
    if (options[idx].callfunc != NULL)
    {
        options[idx].callfunc(strcmp(optarg, "") == 0 ? NULL : optarg);
    }

    return options[idx].id;
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
        if (options[i].short_opt != NONE_SHORT_OPT && s == options[i].short_opt)
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
    return (options->id        == NONE_ID                 &&
            options->short_opt == NONE_SHORT_OPT          &&
            strcmp(options->long_opt, NONE_LONG_OPT) == 0 &&
            options->has_arg   == NONE_HAS_ARG            &&
            options->priority  == NONE_PRIORITY           &&
            options->callfunc  == NONE_CALLFUNC) ? 1 : 0;
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


/**
 * @brief Get short option index which is matched with variable `s`.
 * @param s Short option you want to search.
 * @param options Option list.
 * @return Found: Matched option's index / Not Found: -1
 **/
int __longOptionIndex(char *s, const PsrArgumentObject_t *options)
{
    int i;
    for (i = 0; __isPsrArgumentEnd(&options[i]) == 0; i++)
    {
        if (strcmp(options[i].long_opt, NONE_LONG_OPT) != 0 && strcmp(s, options[i].long_opt) == 0)
        {
            // found
            return i;
        }
    }
    // not found
    return -1;
}
