#ifndef __SYLAR__MACRO_H__
#define __SYLAR__MACRO_H__

#include "log.h"
#include "util.h"
#include<string.h>
#include<assert.h>

/// 断言宏封装
#define SYLAR_ASSERT(x) \
    if((!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

/// 断言宏封装
#define SYLAR_ASSERT2(x, w) \
    if((!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
    
#endif