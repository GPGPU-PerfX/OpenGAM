/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file gam_align.h
 * @brief Integer and pointer alignment helpers for OpenGAM.
 *
 * These helpers operate on power-of-two alignments. Passing a zero or
 * non-power-of-two alignment is invalid and yields an undefined result.
 *
 * The integer overloads are constexpr. The pointer overloads are
 * inline and use the same bitwise rule on the address.
 */

#ifndef OPENGAM_UTIL_GAM_ALIGN_H
#define OPENGAM_UTIL_GAM_ALIGN_H

#include <cstddef>
#include <cstdint>

namespace opengam
{
namespace util
{

/**
 * Returns true if @p value is a non-zero power of two.
 */
constexpr bool isPowerOfTwo(std::size_t value) noexcept
{
    return value != 0 && (value & (value - 1)) == 0;
}

/**
 * Returns true if @p value is a multiple of @p alignment.
 * @p alignment must be a power of two.
 */
constexpr bool isAligned(std::size_t value, std::size_t alignment) noexcept
{
    return (value & (alignment - 1)) == 0;
}

/**
 * Returns true if @p ptr is aligned to @p alignment bytes.
 * @p alignment must be a power of two.
 */
inline bool isAligned(const void *ptr, std::size_t alignment) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(ptr);
    return (address & (static_cast<std::uintptr_t>(alignment) - 1)) == 0;
}

/**
 * Rounds @p value down to a multiple of @p alignment.
 * @p alignment must be a power of two.
 */
constexpr std::size_t alignDown(std::size_t value, std::size_t alignment) noexcept
{
    return value & ~(alignment - 1);
}

/**
 * Rounds @p ptr down to a multiple of @p alignment bytes.
 * @p alignment must be a power of two.
 */
inline void *alignDown(void *ptr, std::size_t alignment) noexcept
{
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    address &= ~(static_cast<std::uintptr_t>(alignment) - 1);
    return reinterpret_cast<void *>(address);
}

/**
 * Rounds @p ptr down to a multiple of @p alignment bytes.
 * @p alignment must be a power of two.
 */
inline const void *alignDown(const void *ptr, std::size_t alignment) noexcept
{
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    address &= ~(static_cast<std::uintptr_t>(alignment) - 1);
    return reinterpret_cast<const void *>(address);
}

/**
 * Rounds @p value up to a multiple of @p alignment.
 * @p alignment must be a power of two.
 *
 * If @p value is close to the top of the `std::size_t` range, the
 * intermediate addition may wrap; the result is then also undefined.
 */
constexpr std::size_t alignUp(std::size_t value, std::size_t alignment) noexcept
{
    return (value + (alignment - 1)) & ~(alignment - 1);
}

/**
 * Rounds @p ptr up to a multiple of @p alignment bytes.
 * @p alignment must be a power of two.
 */
inline void *alignUp(void *ptr, std::size_t alignment) noexcept
{
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    address = (address + (static_cast<std::uintptr_t>(alignment) - 1))
              & ~(static_cast<std::uintptr_t>(alignment) - 1);
    return reinterpret_cast<void *>(address);
}

/**
 * Rounds @p ptr up to a multiple of @p alignment bytes.
 * @p alignment must be a power of two.
 */
inline const void *alignUp(const void *ptr, std::size_t alignment) noexcept
{
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    address = (address + (static_cast<std::uintptr_t>(alignment) - 1))
              & ~(static_cast<std::uintptr_t>(alignment) - 1);
    return reinterpret_cast<const void *>(address);
}

} // namespace util
} // namespace opengam

#endif /* OPENGAM_UTIL_GAM_ALIGN_H */
