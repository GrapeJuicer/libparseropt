/**
 * To disable getTerminalSize(), write following code before include this file:
 *      #define TSZ_SYS 0
 **/

#include <stdio.h>

#ifndef __TERMSZ_H
#define __TERMSZ_H

#ifdef __cplusplus
extern "C" {
#endif // end of #ifdef __cplusplus

// defines --------------------

#define TSZ_SYS_UNKNOWN 0
#define TSZ_SYS_WIN32   1
#define TSZ_SYS_UNIX    2
#define TSZ_SYS_MAC     3

#ifndef TSZ_SYS
#if defined _WIN32 || defined _WIN64 // Windows
#define TSZ_SYS TSZ_SYS_WIN32
#elif defined __unix__ || defined __unix // UNIX
#define TSZ_SYS TSZ_SYS_UNIX
#elif defined __APPLE__ || defined __MACH__ // Mac
#define TSZ_SYS TSZ_SYS_MAC
#else // unknown
#define TSZ_SYS TSZ_SYS_UNKNOWN
#endif
#endif // end of #ifndef TSZ_SYS

// includes --------------------

#if TSZ_SYS == TSZ_SYS_WIN32
#include <windows.h>
#elif TSZ_SYS == TSZ_SYS_UNIX
#include <sys/ioctl.h>
#include <unistd.h>
#elif TSZ_SYS == TSZ_SYS_MAC
// no includes
#endif

// functions --------------------

/**
 * @brief Get teminal size.
 * @param row Address of a variable to store the result of row. Nullable.
 * @param col Address of a variable to store the result of col. Nullable.
 * @return 0: success / 1: failed
 **/
static inline int getTerminalSize(int *row, int *col)
{
#if TSZ_SYS == TSZ_SYS_WIN32 // Windows
    CONSOLE_SCREEN_BUFFER_INFO inf;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf) == 0)
    {
        return 1; // failed
    }
    if (row != NULL)
    {
        *row = inf.srWindow.Bottom - inf.srWindow.Top + 1;
    }
    if (col != NULL)
    {
        *col = inf.srWindow.Right - inf.srWindow.Left + 1;
    }
#elif TSZ_SYS == TSZ_SYS_UNIX || TSZ_SYS == TSZ_SYS_MAC // UNIX, Mac
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w))
    {
        return 1; // failed
    }
    if (row != NULL)
    {
        *row = w.ws_row;
    }
    if (col != NULL)
    {
        *col = w.ws_col;
    }
#else // unknown system
    // do nothing
    (void)row;
    (void)col;
    return 1;
#endif
    return 0;
}

#ifdef __cplusplus
}
#endif // end of #ifdef __cplusplus

#endif // end of ifndef __TERMSZ_H