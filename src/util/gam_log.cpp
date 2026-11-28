/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_log.h"

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <limits.h>
#include <pthread.h>
#include <unistd.h>

namespace opengam
{
namespace util
{
namespace
{

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

LogCallback           g_pCallback = nullptr;
void*                 g_pUser     = nullptr;
constexpr const char* kColorReset = "\033[0m";

constexpr std::size_t kLogDumpCapacity = 64 * 1024;

char        g_dump[kLogDumpCapacity];
std::size_t g_dumpUsed = 0;

// =====================================================================================================================
const char* levelTag(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Trace:
        return "TRACE";
    }

    return "LOG";
}

// =====================================================================================================================
const char* levelColor(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "\033[31m";
    case LogLevel::Warn:
        return "\033[33m";
    case LogLevel::Info:
        return "\033[32m";
    case LogLevel::Trace:
        return "\033[90m";
    }

    return "\033[0m";
}

// =====================================================================================================================
const char* fileName(const char* pPath)
{
    if (pPath == nullptr || pPath[0] == '\0')
    {
        return "?";
    }

    const char* pSlash = std::strrchr(pPath, '/');
    if (pSlash != nullptr && pSlash[1] != '\0')
    {
        return pSlash + 1;
    }

    return pPath;
}

// =====================================================================================================================
void dropOldestRecord()
{
    if (g_dumpUsed == 0)
    {
        return;
    }

    const char* pNewline = static_cast<const char*>(std::memchr(g_dump, '\n', g_dumpUsed));
    if (pNewline == nullptr)
    {
        g_dumpUsed = 0;
        return;
    }

    const std::size_t drop = static_cast<std::size_t>(pNewline - g_dump) + 1;
    g_dumpUsed -= drop;
    if (g_dumpUsed > 0)
    {
        std::memmove(g_dump, g_dump + drop, g_dumpUsed);
    }
}

// =====================================================================================================================
void appendDump(const char* pMessage)
{
    if (pMessage == nullptr)
    {
        return;
    }

    const std::size_t textLen = std::strlen(pMessage);
    const std::size_t need    = textLen + 1;
    if (need > kLogDumpCapacity)
    {
        const std::size_t keep = kLogDumpCapacity - 1;
        std::memcpy(g_dump, pMessage + (textLen - keep), keep);
        g_dump[keep] = '\n';
        g_dumpUsed   = kLogDumpCapacity;
        return;
    }

    while (g_dumpUsed + need > kLogDumpCapacity)
    {
        dropOldestRecord();
    }

    std::memcpy(g_dump + g_dumpUsed, pMessage, textLen);
    g_dump[g_dumpUsed + textLen] = '\n';
    g_dumpUsed += need;
}

// =====================================================================================================================
bool tokenEquals(const char* pBegin, const char* pEnd, const char* pExpected)
{
    const std::size_t length = static_cast<std::size_t>(pEnd - pBegin);
    return std::strlen(pExpected) == length && std::strncmp(pBegin, pExpected, length) == 0;
}

// =====================================================================================================================
void writeStream(FILE* pStream, int fd, LogLevel level, const char* pMessage)
{
    const bool color = (::isatty(fd) != 0);
    if (color)
    {
        std::fprintf(pStream, "%s%s%s\n", levelColor(level), pMessage, kColorReset);
    }
    else
    {
        std::fprintf(pStream, "%s\n", pMessage);
    }

    std::fflush(pStream);
}

// =====================================================================================================================
void writeFilePath(const char* pPath, const char* pMessage)
{
    if (pPath == nullptr || pPath[0] == '\0')
    {
        return;
    }

    FILE* pFile = std::fopen(pPath, "a");
    if (pFile == nullptr)
    {
        return;
    }

    std::fprintf(pFile, "%s\n", pMessage);
    std::fflush(pFile);
    std::fclose(pFile);
}

// =====================================================================================================================
void writeToDestination(LogLevel level, const char* pMessage)
{
    const char* pDest = std::getenv("OPENGAM_LOG_FILE");
    if (pDest == nullptr || pDest[0] == '\0')
    {
        writeStream(stderr, STDERR_FILENO, level, pMessage);
        writeStream(stdout, STDOUT_FILENO, level, pMessage);
        return;
    }

    const char* pBegin = pDest;
    while (*pBegin != '\0' && std::isspace(static_cast<unsigned char>(*pBegin)) != 0)
    {
        ++pBegin;
    }

    const char* pEnd = pBegin + std::strlen(pBegin);
    while (pEnd > pBegin && std::isspace(static_cast<unsigned char>(pEnd[-1])) != 0)
    {
        --pEnd;
    }

    if (tokenEquals(pBegin, pEnd, "stderr"))
    {
        writeStream(stderr, STDERR_FILENO, level, pMessage);
        return;
    }

    if (tokenEquals(pBegin, pEnd, "stdout"))
    {
        writeStream(stdout, STDOUT_FILENO, level, pMessage);
        return;
    }

    const std::size_t pathLen = static_cast<std::size_t>(pEnd - pBegin);
    if (pathLen > 0 && pathLen < PATH_MAX)
    {
        char path[PATH_MAX];
        std::memcpy(path, pBegin, pathLen);
        path[pathLen] = '\0';
        writeFilePath(path, pMessage);
    }
}

} // namespace

// =====================================================================================================================
void registerLogCallback(LogCallback pCallback, void* pUser)
{
    pthread_mutex_lock(&g_mutex);
    g_pCallback = pCallback;
    g_pUser     = pUser;
    pthread_mutex_unlock(&g_mutex);
}

// =====================================================================================================================
void unregisterLogCallback()
{
    registerLogCallback(nullptr, nullptr);
}

// =====================================================================================================================
bool dumpLogsToMemory(char* pBuffer, std::size_t* pSize)
{
    if (pSize == nullptr)
    {
        return false;
    }

    pthread_mutex_lock(&g_mutex);

    if (pBuffer == nullptr)
    {
        *pSize = g_dumpUsed;
        pthread_mutex_unlock(&g_mutex);
        return true;
    }

    if (*pSize == 0)
    {
        pthread_mutex_unlock(&g_mutex);
        return true;
    }

    std::size_t copy = g_dumpUsed;
    if (copy >= *pSize)
    {
        copy = *pSize - 1;

        while (copy > 0 && g_dump[copy - 1] != '\n')
        {
            --copy;
        }
    }

    if (copy > 0)
    {
        std::memcpy(pBuffer, g_dump, copy);
    }

    pBuffer[copy] = '\0';
    *pSize        = copy;
    pthread_mutex_unlock(&g_mutex);
    return true;
}

// =====================================================================================================================
bool dumpLogsToFile(const char* pPath)
{
    if (pPath == nullptr || pPath[0] == '\0')
    {
        return false;
    }

    FILE* pFile = std::fopen(pPath, "w");
    if (pFile == nullptr)
    {
        return false;
    }

    pthread_mutex_lock(&g_mutex);
    const bool ok = (std::fwrite(g_dump, 1, g_dumpUsed, pFile) == g_dumpUsed);
    pthread_mutex_unlock(&g_mutex);

    const int closed = std::fclose(pFile);
    return ok && closed == 0;
}

// =====================================================================================================================
void log(LogLevel level, const char* pFile, int line, const char* pFunction, const char* pFmt, ...)
{
    char body[1024];
    body[0] = '\0';

    if (pFmt != nullptr)
    {
        va_list args;
        va_start(args, pFmt);
        std::vsnprintf(body, sizeof(body), pFmt, args);
        va_end(args);
    }

    const char* pFunc = (pFunction != nullptr && pFunction[0] != '\0') ? pFunction : "?";

    char message[1280];
    std::snprintf(message, sizeof(message), "[%s] %s:%d %s: %s", levelTag(level), fileName(pFile), line, pFunc, body);

    pthread_mutex_lock(&g_mutex);

    LogCallback pCallback = g_pCallback;
    void*       pUser     = g_pUser;

    appendDump(message);
    writeToDestination(level, message);

    if (pCallback != nullptr)
    {
        pCallback(level, message, pUser);
    }

    pthread_mutex_unlock(&g_mutex);
}

} // namespace util
} // namespace opengam
