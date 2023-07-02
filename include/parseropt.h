#ifndef _PARSEROPT_H
#define _PARSEROPT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// includes --------------------

// defines --------------------

#define PSR_BUF_SIZE 512

#define NONE_ID        -1
#define NONE_SHORT_OPT (char)0
#define NONE_LONG_OPT  ""
#define NONE_HAS_ARG   NO_ARGUMENT
#define NONE_PRIORITY  0
#define NONE_CALLFUNC  NULL

#define NONE_DESC      NULL

#define PSR_ARG_END { \
    NONE_ID,          \
    NONE_SHORT_OPT,   \
    NONE_LONG_OPT,    \
    NONE_HAS_ARG,     \
    NONE_PRIORITY,    \
    NONE_CALLFUNC     \
}

#define PSR_DESC_END { \
    NONE_ID,           \
    NONE_DESC          \
}

#define OPT_DEFAULT_HEADER_SHORT "-"
#define OPT_DEFAULT_HEADER_LONG "--"

// Rsr Result
#define PSR_NOT_FOUND           -1
#define PSR_UNKNOWN_OPTION      -2
#define PSR_NO_ARG_HAS_ARG      -3
#define PSR_REQ_ARG_HAS_NO_ARG  -4
#define PSR_OPT_ARG_HAS_ONLY_EQ -5
#define PSR_ERROR               -10
#define PSR_ERROR_HAS_ARG       -11

// function macro
#define psrHDesc(options, descs) psrHDescWithConfig(options, descs, &DEFAULT_PSR_CONFIG)
#define psrHelp(options, descs, usage, prefix, suffix) psrHelpWithConfig(options, descs, usage, prefix, suffix, &DEFAULT_PSR_CONFIG)

// types --------------------

typedef enum
{
    NO_ARGUMENT      ,
    REQUIRE_ARGUMENT ,
    OPTIONAL_ARGUMENT,
} PsrArgument_t;

typedef struct
{
    int           id                          ; // >=0
    char          short_opt                   ;
    char          long_opt[PSR_BUF_SIZE]      ;
    PsrArgument_t has_arg                     ;
    int           priority                    ;
    void          (*callfunc)(const char *arg);
} PsrArgumentObject_t;

typedef struct
{
    int id;
    char *desc;
} PsrDescription_t;

// |    -a, --alpha        This is a description.
//  <--> indent
//                 <------> margin
//                         <---------------------------------> desc_width
typedef struct
{
    unsigned int indent    ;
    char *       sep       ;
    unsigned int margin    ;
    unsigned int desc_width;
} PsrHelpConfig_t;

// variables --------------------

extern const PsrHelpConfig_t DEFAULT_PSR_CONFIG;

// prototype declarations --------------------

int setHeader(char *__short, char *__long);

int parseropt(
    int   argc                        ,
    char  **argv                      ,
    const PsrArgumentObject_t *options,
    char  **optarg                    ,
    int   *optind
);

void psrHelpWithConfig(
    const PsrArgumentObject_t *options,
    const PsrDescription_t *descs,
    const char *usage,
    const char *prefix,
    const char *suffix,
    const PsrHelpConfig_t *config
);

void psrHDescWithConfig(
    const PsrArgumentObject_t *options,
    const PsrDescription_t *descs,
    const PsrHelpConfig_t *config
);

void psrHOptionNote(void);

int isPsrArgumentEnd(
    const PsrArgumentObject_t *options
);

int psrHDescOptionWidth(
    const PsrArgumentObject_t *options,
    const PsrHelpConfig_t *config,
    int *swidth,
    int *lwidth
);

// --------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PARSEROPT_H
