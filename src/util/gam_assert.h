/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file gam_assert.h
 * @brief Internal assertions for OpenGAM.
 *
 * The assertion macros are the intended interface:
 *
 *     GAM_ASSERT(ptr != nullptr);
 *     GAM_ASSERT_MSG(size > 0, "size must be positive, got %zu", size);
 *     GAM_NOT_IMPLEMENTED();
 *     GAM_NEVER_REACHED();
 *
 * When `GAM_ENABLE_DEBUG` is defined (the `dbg` and `dbg_asan` builds),
 * a failed assertion writes an error log at the call site and then
 * breaks into the debugger with `raise(SIGTRAP)`. If that signal
 * returns, the process is aborted so execution cannot continue past a
 * failed check.
 *
 * In `rel` these macros are compiled out and do not evaluate their
 * arguments. Do not place required work in an assertion expression.
 */

#ifndef OPENGAM_UTIL_GAM_ASSERT_H
#define OPENGAM_UTIL_GAM_ASSERT_H

namespace opengam
{
namespace util
{
namespace detail
{

/**
 * Log a failed assertion and break into the debugger.
 *
 * @p expr is the failed condition, or the name of a
 * `GAM_NOT_IMPLEMENTED` / `GAM_NEVER_REACHED` site.
 * @p fmt may be null; when present it is a printf-style message
 * appended to the log record.
 */
[[noreturn]] void assertFail(const char *expr,
                             const char *file,
                             int line,
                             const char *function,
                             const char *fmt,
                             ...)
#if defined(__GNUC__)
    __attribute__((format(printf, 5, 6)))
#endif
    ;

} // namespace detail
} // namespace util
} // namespace opengam

#if defined(GAM_ENABLE_DEBUG)

#define GAM_ASSERT(expr) \
    do \
    { \
        if (!(expr)) \
        { \
            ::opengam::util::detail::assertFail(#expr, \
                                                __FILE__, \
                                                __LINE__, \
                                                __func__, \
                                                nullptr); \
        } \
    } while (0)

#define GAM_ASSERT_MSG(expr, ...) \
    do \
    { \
        if (!(expr)) \
        { \
            ::opengam::util::detail::assertFail(#expr, \
                                                __FILE__, \
                                                __LINE__, \
                                                __func__, \
                                                __VA_ARGS__); \
        } \
    } while (0)

#define GAM_NOT_IMPLEMENTED() \
    ::opengam::util::detail::assertFail("GAM_NOT_IMPLEMENTED", \
                                        __FILE__, \
                                        __LINE__, \
                                        __func__, \
                                        nullptr)

#define GAM_NEVER_REACHED() \
    ::opengam::util::detail::assertFail("GAM_NEVER_REACHED", \
                                        __FILE__, \
                                        __LINE__, \
                                        __func__, \
                                        nullptr)

#else

#define GAM_ASSERT(expr) ((void)0)
#define GAM_ASSERT_MSG(expr, ...) ((void)0)
#define GAM_NOT_IMPLEMENTED() ((void)0)
#define GAM_NEVER_REACHED() ((void)0)

#endif

#endif /* OPENGAM_UTIL_GAM_ASSERT_H */
