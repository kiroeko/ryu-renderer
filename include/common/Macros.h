#ifndef __MACROS_H__
#define __MACROS_H__

#include <iostream>

#define EXPAND(x) x

#define FAILTEST(boolExpression, statement) \
{ \
    if (!(boolExpression)) \
    { \
        statement; \
    } \
}

#define FAILTEST_RTN_1(boolExpression) FAILTEST(boolExpression, return)

#define FAILTEST_RTN_2(boolExpression, failedMessage) FAILTEST(boolExpression, std::cerr << failedMessage << std::endl; return)

#define FAILTEST_RTN_3(boolExpression, failedMessage, failedReturnValue) FAILTEST(boolExpression, std::cerr << failedMessage << std::endl; return failedReturnValue)

#define GET_MACRO3(_1, _2, _3, NAME, ...) NAME

#define FAILTEST_RTN(...) EXPAND(GET_MACRO3(__VA_ARGS__, FAILTEST_RTN_3, FAILTEST_RTN_2, FAILTEST_RTN_1)(__VA_ARGS__))

#endif