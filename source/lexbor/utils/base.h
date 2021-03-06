/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_UTILS_BASE_H
#define LEXBOR_UTILS_BASE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/core/base.h"


#define LXB_UTILS_VERSION_MAJOR 0
#define LXB_UTILS_VERSION_MINOR 2
#define LXB_UTILS_VERSION_PATCH 1

#define LXB_UTILS_VERSION_STRING                                               \
    LEXBOR_STRINGIZE(LXB_UTILS_VERSION_MAJOR) "."                              \
    LEXBOR_STRINGIZE(LXB_UTILS_VERSION_MINOR) "."                              \
    LEXBOR_STRINGIZE(LXB_UTILS_VERSION_PATCH)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_UTILS_BASE_H */
