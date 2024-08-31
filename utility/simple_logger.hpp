#ifndef _SIMPLE_LOGGER_HPP_
#define _SIMPLE_LOGGER_HPP_

#include <cstdio>
#include <iostream>

#define debug_print(format, ...)                                               \
    do {                                                                       \
        printf("DEBUG: %s:%d: ", __FILE__, __LINE__);                          \
        printf(format __VA_OPT__(, ) __VA_ARGS__);                             \
    } while (false)
#define error_print(format, ...)                                               \
    do {                                                                       \
        printf("ERROR: %s:%d: ", __FILE__, __LINE__);                          \
        printf(format __VA_OPT__(, ) __VA_ARGS__);                             \
    } while (false)

#endif // _SIMPLE_LOGGER_HPP_