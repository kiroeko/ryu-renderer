#ifndef __MACROS_H__
#define __MACROS_H__

#ifdef RYU_RENDERER_MACROS

#include <iostream>

#define GET_MACRO2(_1,_2,NAME, ...) NAME

#define GET_MACRO3(_1,_2,_3,NAME, ...) NAME

#define FAILTEST_IMPL(boolExpression, failedStatement) \
{ \
    if (!boolExpression) \
    { \
        failedStatement; \
    } \
}

#define FAILTEST_RTN_IMPL1(boolExpression) FAILTEST_IMPL(boolExpression, return)

#define FAILTEST_RTN_IMPL2(boolExpression, failedReturnExpression) FAILTEST_IMPL(boolExpression, return failedReturnExpression)

#define FAILTEST_RTN(...) GET_MACRO2(__VA_ARGS__, FAILTEST_RTN_IMPL2, FAILTEST_RTN_IMPL1)(__VA_ARGS__)

#define FAILTEST_RTN_MSG_IMPL1(boolExpression) FAILTEST_IMPL(boolExpression, std::cerr << "FAILTEST_RTN_MSG_IMPL1: Macro Judgement Failed!" << std::endl; return)

#define FAILTEST_RTN_MSG_IMPL2(boolExpression, message) FAILTEST_IMPL(boolExpression, std::cerr << message << std::endl; return)

#define FAILTEST_RTN_MSG_IMPL3(boolExpression, failedReturnExpression, message) FAILTEST_IMPL(boolExpression, std::cerr << message << std::endl; return failedReturnExpression)

#define FAILTEST_RTN_MSG(...) GET_MACRO3(__VA_ARGS__, FAILTEST_RTN_MSG_IMPL3, FAILTEST_RTN_MSG_IMPL2, FAILTEST_RTN_MSG_IMPL1)(__VA_ARGS__)

#endif

#endif