#include <stdio.h>
#include "parseropt.h"

enum
{
    ID_NO_ARG,
    ID_REQ_ARG,
    ID_OPT_ARG,
};

int main(int argc, char *argv[])
{
    // declare options
    PsrArgumentObject_t options[] = {
        {.id = ID_NO_ARG  , .short_opt = 'n', .long_opt = PSR_ARG_NONE_LONG_OPT, .has_arg = NO_ARGUMENT      , .priority = PSR_ARG_NONE_PRIORITY, .callfunc = PSR_ARG_NONE_CALLFUNC},
        {.id = ID_REQ_ARG , .short_opt = 'r', .long_opt = PSR_ARG_NONE_LONG_OPT, .has_arg = REQUIRE_ARGUMENT , .priority = PSR_ARG_NONE_PRIORITY, .callfunc = PSR_ARG_NONE_CALLFUNC},
        {.id = ID_OPT_ARG , .short_opt = 'o', .long_opt = PSR_ARG_NONE_LONG_OPT, .has_arg = OPTIONAL_ARGUMENT, .priority = PSR_ARG_NONE_PRIORITY, .callfunc = PSR_ARG_NONE_CALLFUNC},
        PSR_ARG_END
    };

    int id;                     // Found option's ID. (PsrArgumentObject_t.id)
    char optarg[PSR_BUF_SIZE];  // REQUIRE_ARGUMENT/OPTIONAL_ARGUMENT option's argument. if no argument is given, this values is "".
    int optind = 0;             // Start index of non-option arguments (after calling persoropt() function). You must be set 0 or 1.
    while((id = persoropt(argc, argv, options, optarg, &optind)) != PSR_NOT_FOUND)
    {
        // find option
        switch (id)
        {
        case ID_NO_ARG:
            printf("No Argument Option\n");
            break;
        case ID_REQ_ARG:
            printf("Require Argument Option: %s\n", optarg);
            break;
        case ID_OPT_ARG:
            printf("Optional Argument Option: %s\n", optarg);
            break;
        default: // PSR_UNKNOWN_OPTION, PSR_NO_ARG_HAS_ARG, PSR_REQ_ARG_HAS_NO_ARG, PSR_ERROR
            printf("Invalid Argument\n");
            return -1;
        }
    }

    printf("Remain arguments -----\n");
    int i;
    for (i = optind; i < argc; i++)
    {
        printf("\t%s\n", argv[i]);
    }

    return 0;
}