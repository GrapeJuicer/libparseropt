#include <stdio.h>
#include "parseropt.h"

enum
{
    ID_HELP,
    ID_HELP2,
    ID_NO_ARG,
    ID_REQ_ARG,
    ID_OPT_ARG,
    ID_SHORT_ONLY,
    ID_LONG_ONLY,
};

int main(int argc, char *argv[])
{
    // declare options
    PsrArgumentObject_t options[] = {
        {.id = ID_HELP      , .short_opt = 'h'           , .long_opt = "help"       , .has_arg = NO_ARGUMENT      , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_HELP2      , .short_opt = 'H'           , .long_opt = "help2"      , .has_arg = NO_ARGUMENT      , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_NO_ARG    , .short_opt = 'n'           , .long_opt = "no"         , .has_arg = NO_ARGUMENT      , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_REQ_ARG   , .short_opt = 'r'           , .long_opt = "require"    , .has_arg = REQUIRE_ARGUMENT , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_OPT_ARG   , .short_opt = 'o'           , .long_opt = "optional"   , .has_arg = OPTIONAL_ARGUMENT, .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_SHORT_ONLY, .short_opt = 's'           , .long_opt = NONE_LONG_OPT, .has_arg = NO_ARGUMENT      , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        {.id = ID_LONG_ONLY , .short_opt = NONE_SHORT_OPT, .long_opt = "long"       , .has_arg = NO_ARGUMENT      , .priority = NONE_PRIORITY, .callfunc = NONE_CALLFUNC},
        PSR_ARG_END
    };

    PsrDescription_t descs[] = {
        {.id = ID_HELP      , .desc = "Help option."                           },
        {.id = ID_HELP      , .desc = "Help option with Custom config."        },
        {.id = ID_NO_ARG    , .desc = NULL                                     }, // not shown
        {.id = ID_REQ_ARG   , .desc = "Require argument.\nYou can use \'\\n\'."},
        {.id = ID_OPT_ARG   , .desc = "Optional argument."                     },
        {.id = ID_SHORT_ONLY, .desc = "Short option only."                     },
    //  {.id = ID_LONG_ONLY , .desc = "Long option only."                      }, // not shown
        {.id = 999999       , .desc = "Unknown."                               }, // not used
        PSR_DESC_END
    };

    PsrHelpConfig_t help_conf = {
        .indent     = 10    ,
        .sep        = " | " ,
        .margin     = 2     ,
        .desc_width = 20    ,
    };

    int id;         // Found option's ID. (PsrArgumentObject_t.id)
    char *optarg;   // REQUIRE_ARGUMENT/OPTIONAL_ARGUMENT option's argument. if no argument is given, this values is NULL.
    int optind = 0; // Start index of non-option arguments (after calling parseropt() function). You must be set 0 or 1.
    while((id = parseropt(argc, argv, options, &optarg, &optind)) != PSR_NOT_FOUND)
    {
        // find option
        switch (id)
        {
        case ID_HELP:
            psrHelp(options, descs, "help [options...]", NULL, NULL);
            return 0;
        case ID_HELP2:
            psrHelpWithConfig(options, descs, "help [options...]", "This is a prefix text.", "This is a suffix text.", &help_conf);
            return 0;
        case ID_NO_ARG:
            printf("No Argument Option\n");
            break;
        case ID_REQ_ARG:
            printf("Require Argument Option: %s\n", optarg);
            break;
        case ID_OPT_ARG:
            printf("Optional Argument Option: %s\n", optarg);
            break;
        case ID_SHORT_ONLY:
            printf("Short only.\n");
            break;
        case ID_LONG_ONLY:
            printf("Long only.\n");
            break;
        case PSR_ERROR:
            printf("Process error\n");
            break;
        default: // PSR_UNKNOWN_OPTION, PSR_NO_ARG_HAS_ARG, PSR_REQ_ARG_HAS_NO_ARG, PSR_ERROR_HAS_ARG
            printf("Invalid Argument: %s\n", argv[optind]);
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