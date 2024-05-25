#ifndef __SYLAR__MACRO_H__
#define __SYLAR__MACRO_H__

#include "log.h"
#include "util.h"
#include<string.h>
#include<assert.h>

#if defined __GNUC__ || defined __llvm__
#define SYLAR_LIKELY(x)  __builtin_expect(!!(x), 1)
#define SYLAR_UNLIKELY(x)  __builtin_expect(!!(x), 0)
#else
#define SYLAR_LIKELY(x)
#define SYLAR_UNLIKELY(x)
#endif

/// 断言宏封装
#define SYLAR_ASSERT(x) \
    if(SYLAR_UNLIKELY(!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

/// 断言宏封装
#define SYLAR_ASSERT2(x, w) \
    if(SYLAR_UNLIKELY(!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
    



#endif