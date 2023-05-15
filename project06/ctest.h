#ifndef CTEST_H
#define CTEST_H

#ifdef CTEST_ENABLE

#include <stdio.h>
#include <stdlib.h>

int ctest_pass_count = 0, ctest_fail_count = 0;

int ctest_verbose = 0, ctest_color = 1;

int ctest_status = 0;

const char *ctest_fptr = NULL;

#define CTEST_VERBOSE(v) \
do { \
    ctest_verbose = v; \
} while (0)

#define CTEST_COLOR(v) \
do { \
    ctest_color = v; \
} while (0)

#define CTEST_EXIT() \
do { \
    exit(ctest_status); \
} while (0)

#define CTEST_ASSERT(c, m) \
do { \
    if (ctest_fptr == NULL) \
        ctest_fptr = __func__; \
    char *nl = ctest_fptr != __func__? "\n": ""; \
    ctest_fptr = __func__; \
    if (!(c)) { \
        printf("%s%sFAIL%s: %s: " __FILE__ ":%d: %s: %s\n", \
               nl, \
               ctest_color? "\x1b[0;1;31m": "", \
               ctest_color? "\x1b[0m": "", \
               __func__, __LINE__, \
               #c, m); \
        ctest_fail_count++; \
        ctest_status = 1; \
    } else { \
        if (ctest_verbose) \
            printf("%s%s  OK%s: %s: " __FILE__ ":%d: %s: %s\n", \
                   nl, \
                   ctest_color? "\x1b[0;32m": "", \
                   ctest_color? "\x1b[0m": "", \
                   __func__, __LINE__, \
                   #c, m); \
        ctest_pass_count++; \
    } \
} while (0)

#define CTEST_RESULTS() \
do { \
    int t = ctest_pass_count + ctest_fail_count; \
    printf("%sResults: %d/%d passing (%.1f%%).\n", \
        ctest_fail_count > 0 || (t > 0 && ctest_verbose)? "\n": "", \
        ctest_pass_count, t, \
        100.0 * ctest_pass_count / t); \
} while (0)

#endif

#endif

