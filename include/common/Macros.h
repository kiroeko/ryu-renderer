#ifndef __MACROS_H__
#define __MACROS_H__

#ifdef RYU_RENDERER_MACROS

#include <iostream>

#define EXPAND(x) x

#define FAILTEST_RTN_MSG_1(boolExpression) \
{ \
    if (!(boolExpression)) \
    { \
        return; \
    } \
}

#define FAILTEST_RTN_MSG_2(boolExpression, failedMessage) \
{ \
    if (!(boolExpression)) \
    { \
        std::cerr << failedMessage << std::endl; \
        return; \
    } \
}

#define FAILTEST_RTN_MSG_3(boolExpression, failedMessage, failedReturnValue) \
{ \
    if (!(boolExpression)) \
    { \
        std::cerr << failedMessage << std::endl; \
        return failedReturnValue; \
    } \
}

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define FAILTEST_RTN_MSG(...) EXPAND(GET_MACRO(__VA_ARGS__, FAILTEST_RTN_MSG_3, FAILTEST_RTN_MSG_2, FAILTEST_RTN_MSG_1)(__VA_ARGS__))

#endif

#endif