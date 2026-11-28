/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file gam_log.h
 * @brief Internal logging for OpenGAM.
 *
 * The logging macros are the intended interface:
 *
 *     GAM_LOG_ERROR("failed to map region %p", ptr);
 *     GAM_LOG_WARN("retrying after a short delay");
 *     GAM_LOG_INFO("loaded model version %d", version);
 *     GAM_LOG_TRACE("align %zu up to %zu", value, alignment);
 *
 * When `GAM_ENABLE_DEBUG` is defined (the `dbg` and `dbg_asan` builds),
 * each macro forwards to the logger with the source file, line, and
 * function of the call site. In `rel` the macros are compiled out and
 * do not evaluate their arguments.
 *
 * The default sink writes to stderr. ANSI color is applied only when
 * stderr is a TTY. Tests can install a callback with ::opengam::util::setLogSink
 * to capture output; a callback receives plain text, without color.
 * Passing a null callback, or calling ::opengam::util::resetLogSink,
 * restores the default sink. The callback must not log, because the
 * logger holds a mutex around the sink call.
 *
 * Emit is serialized with a POSIX mutex so later multi-threaded use is
 * safe. The log path does not use throwing C++ APIs.
 */

#ifndef OPENGAM_UTIL_GAM_LOG_H
#define OPENGAM_UTIL_GAM_LOG_H

namespace opengam
{
namespace util
{

/**
 * Severity of a log record, from most severe to most verbose.
 */
enum class LogLevel
{
    Error,
    Warn,
    Info,
    Trace
};

/**
 * Optional replacement for the default stderr sink.
 *
 * @param level   Severity of the record.
 * @param message Formatted record, including the level tag and the
 *                call-site location. Never null.
 * @param user    Pointer supplied to ::opengam::util::setLogSink.
 */
using LogSink = void (*)(LogLevel level, const char *message, void *user);

/**
 * Install a capture callback, or pass a null @p sink to restore the
 * default stderr sink.
 */
void setLogSink(LogSink sink, void *user);

/**
 * Restore the default stderr sink.
 */
void resetLogSink();

namespace detail
{

/**
 * Format and emit one log record. The `GAM_LOG_*` macros call this
 * when `GAM_ENABLE_DEBUG` is defined. @p fmt is a printf-style string.
 */
void log(LogLevel level,
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

#define GAM_LOG_ERROR(...) \
    ::opengam::util::detail::log(::opengam::util::LogLevel::Error, \
                                 __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_WARN(...) \
    ::opengam::util::detail::log(::opengam::util::LogLevel::Warn, \
                                 __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_INFO(...) \
    ::opengam::util::detail::log(::opengam::util::LogLevel::Info, \
                                 __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_TRACE(...) \
    ::opengam::util::detail::log(::opengam::util::LogLevel::Trace, \
                                 __FILE__, __LINE__, __func__, __VA_ARGS__)

#else

#define GAM_LOG_ERROR(...) ((void)0)
#define GAM_LOG_WARN(...) ((void)0)
#define GAM_LOG_INFO(...) ((void)0)
#define GAM_LOG_TRACE(...) ((void)0)

#endif

#endif /* OPENGAM_UTIL_GAM_LOG_H */
