#ifndef _PARSEROPT_H
#define _PARSEROPT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// includes --------------------

// defines --------------------

#define PSR_BUF_SIZE 512

#define PSR_ARG_NONE_ID        0
#define PSR_ARG_NONE_SHORT_OPT (char)0
#define PSR_ARG_NONE_LONG_OPT  ""
#define PSR_ARG_NONE_HAS_ARG   NO_ARGUMENT
#define PSR_ARG_NONE_PRIORITY  0
#define PSR_ARG_NONE_CALLFUNC  NULL

#define PSR_ARG_END {       \
    PSR_ARG_NONE_ID,        \
    PSR_ARG_NONE_SHORT_OPT, \
    PSR_ARG_NONE_LONG_OPT,  \
    PSR_ARG_NONE_HAS_ARG,   \
    PSR_ARG_NONE_PRIORITY,  \
    PSR_ARG_NONE_CALLFUNC   \
}
#define OPT_HEADER_SHORT "-"
#define OPT_HEADER_LONG "--"

// Rsr Result
#define PSR_NOT_FOUND           -1
#define PSR_UNKNOWN_OPTION      -2
#define PSR_NO_ARG_HAS_ARG      -3
#define PSR_REQ_ARG_HAS_NO_ARG  -4
#define PSR_ERROR               -10

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

// prototype declarations --------------------

int persoropt(
    int   argc                        ,
    char  **argv                      ,
    const PsrArgumentObject_t *options,
    char  optarg[PSR_BUF_SIZE]        ,
    int   *pos
);

// --------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PARSEROPT_H
