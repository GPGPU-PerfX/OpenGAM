/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_assert.h"

#include "gam_log.h"

#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace opengam
{
namespace util
{

// =====================================================================================================================
[[noreturn]] void assertFail(const char* pExpr,
                             const char* pFile,
                             int         line,
                             const char* pFunction,
                             const char* pFmt,
                             ...)
{
    const char* pShown = (pExpr != nullptr && pExpr[0] != '\0') ? pExpr : "assertion";

    char extra[1024];
    extra[0] = '\0';

    if (pFmt != nullptr && pFmt[0] != '\0')
    {
        va_list args;
        va_start(args, pFmt);
        std::vsnprintf(extra, sizeof(extra), pFmt, args);
        va_end(args);
    }

    if (extra[0] != '\0')
    {
        log(LogLevel::Error, pFile, line, pFunction, "assertion failed: %s (%s)", pShown, extra);
    }
    else
    {
        log(LogLevel::Error, pFile, line, pFunction, "assertion failed: %s", pShown);
    }

    raise(SIGTRAP);
    std::abort();
}

} // namespace util
} // namespace opengam
