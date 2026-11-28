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
[[noreturn]] void assertFail(const char* expr, const char* file, int line, const char* function, const char* fmt, ...)
{
    const char* shown = (expr != nullptr && expr[0] != '\0') ? expr : "assertion";

    char extra[1024];
    extra[0] = '\0';

    if (fmt != nullptr && fmt[0] != '\0')
    {
        va_list args;
        va_start(args, fmt);
        std::vsnprintf(extra, sizeof(extra), fmt, args);
        va_end(args);
    }

    if (extra[0] != '\0')
    {
        log(LogLevel::Error, file, line, function, "assertion failed: %s (%s)", shown, extra);
    }
    else
    {
        log(LogLevel::Error, file, line, function, "assertion failed: %s", shown);
    }

    raise(SIGTRAP);
    std::abort();
}

} // namespace util
} // namespace opengam
