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
 * When `GAM_ENABLE_DEBUG` is defined (the `dbg` and `dbg_asan` builds), each macro forwards to the logger with the
 * source file, line, and function of the call site. In `rel` the macros are compiled out and do not evaluate their
 * arguments.
 *
 * Live output goes to the destination in the `OGAM_LOG_FILE` environment variable: `stderr`, `stdout`, or a file path.
 * If the variable is unset or empty, records are written to both stderr and stdout. ANSI color is applied only when a
 * stream is a TTY. A later settings layer can override this choice.
 *
 * An optional callback, installed with ::opengam::util::registerLogCallback, also receives the plain record, without
 * color. At most one callback is registered at a time; a later register replaces the previous callback.
 * ::opengam::util::unregisterLogCallback removes it. The callback must not log, because the logger holds a mutex around
 * the callback.
 *
 * Accumulated records can be copied with ::opengam::util::dumpLogsToMemory or ::opengam::util::dumpLogsToFile. Those
 * calls take a snapshot of the log buffer and do not change the live destinations.
 *
 * Emit is serialized with a POSIX mutex so later multi-threaded use is safe. The log path does not use throwing C++
 * APIs.
 */

#ifndef OPENGAM_UTIL_GAM_LOG_H
#define OPENGAM_UTIL_GAM_LOG_H

#include <cstddef>

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
 * Optional callback invoked for each log record, in addition to the live destinations.
 *
 * @param level   Severity of the record.
 * @param pMessage Formatted record, including the level tag and the call-site location. Never null.
 * @param pUser   Pointer supplied to ::opengam::util::registerLogCallback.
 */
using LogCallback = void (*)(LogLevel level, const char* pMessage, void* pUser);

/**
 * Register a callback, replacing any callback already installed. Pass a null @p pCallback to unregister.
 */
void registerLogCallback(LogCallback pCallback, void* pUser);

/**
 * Remove the registered callback, if any.
 */
void unregisterLogCallback();

/**
 * Copy accumulated log records into @p pBuffer as a single NUL-terminated string, one record per line, without color.
 *
 * @p pSize is required. On entry it is the capacity of @p pBuffer in bytes. On exit it is the number of bytes written,
 * not including the NUL terminator. If @p pBuffer is null, *@p pSize is set to the number of bytes needed, not
 * including the NUL, and the function returns true. If the buffer is too small, as many complete records as will fit
 * are written, and the buffer is still NUL-terminated when the capacity is greater than zero.
 *
 * @return false if @p pSize is null.
 */
bool dumpLogsToMemory(char* pBuffer, std::size_t* pSize);

/**
 * Write the accumulated log records to @p pPath, replacing the file.
 *
 * @return false if @p pPath is null or the file cannot be written.
 */
bool dumpLogsToFile(const char* pPath);

/**
 * Format and emit one log record. The `GAM_LOG_*` macros call this when `GAM_ENABLE_DEBUG` is defined. @p pFmt is a
 * printf-style string.
 */
void log(LogLevel level, const char* pFile, int line, const char* pFunction, const char* pFmt, ...)
#if defined(__GNUC__)
    __attribute__((format(printf, 5, 6)))
#endif
    ;

} // namespace util
} // namespace opengam

#if defined(GAM_ENABLE_DEBUG)

#define GAM_LOG_ERROR(...)                                                                                             \
    ::opengam::util::log(::opengam::util::LogLevel::Error, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_WARN(...)                                                                                              \
    ::opengam::util::log(::opengam::util::LogLevel::Warn, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_INFO(...)                                                                                              \
    ::opengam::util::log(::opengam::util::LogLevel::Info, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define GAM_LOG_TRACE(...)                                                                                             \
    ::opengam::util::log(::opengam::util::LogLevel::Trace, __FILE__, __LINE__, __func__, __VA_ARGS__)

#else

#define GAM_LOG_ERROR(...) ((void)0)
#define GAM_LOG_WARN(...)  ((void)0)
#define GAM_LOG_INFO(...)  ((void)0)
#define GAM_LOG_TRACE(...) ((void)0)

#endif

#endif /* OPENGAM_UTIL_GAM_LOG_H */
