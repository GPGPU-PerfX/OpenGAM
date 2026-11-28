/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file opengam.h
 * @brief Public C ABI for OpenGAM (Open-Source GPU Architecture Model).
 *
 * Include this header to use the OpenGAM C API:
 *
 *     #include <opengam.h>
 *
 * The ABI is a stable C interface. This header exposes no C++ types, and
 * C++ exceptions do not cross the ABI boundary. All currently exported
 * functions return a ::GAMresult and use the ::GAMAPI calling convention,
 * as in `GAMresult GAMAPI gamModelGetVersion(int *version)`.
 *
 * The first published entry point is ::gamModelGetVersion. Additional
 * functions will be added to this header without changing the meaning of
 * existing symbols, result codes, or version encoding. New public
 * functions must also be added to the export list in `opengam.def`.
 *
 * When building the shared library (`libopengam.so` or the equivalent
 * on other platforms), define `GAM_BUILD_LIBRARY` so that public
 * declarations use default ELF visibility and can be exported from
 * `opengam.def`. Applications that include this header should not
 * define `GAM_BUILD_LIBRARY`.
 */

#ifndef __opengam_h__
#define __opengam_h__

/**
 * @def GAMAPI
 * @brief Calling convention for public entry points.
 *
 * Place this macro between the return type and the function name, the
 * same way CUDA driver and runtime headers use `CUDAAPI` / `CUDARTAPI`:
 *
 *     GAMresult GAMAPI gamModelGetVersion(int *version);
 *
 * On Windows this is `__stdcall`. On other platforms it is empty.
 * Shared-library export is controlled by `opengam.def`, not by this
 * macro. When `GAM_BUILD_LIBRARY` is defined, public declarations in
 * this header also receive default ELF visibility so the linker can
 * export the symbols named in that file.
 */
#ifdef _WIN32
#define GAMAPI __stdcall
#else
#define GAMAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && defined(GAM_BUILD_LIBRARY)
#pragma GCC visibility push(default)
#endif

/**
 * @defgroup GAM_VERSION Version Management
 *
 * Compile-time and run-time identification of the OpenGAM library.
 * The packed encoding is always (1000 * major + 10 * minor). For
 * example, OpenGAM 0.1 is 10 and OpenGAM 0.2 would be 20.
 *
 * @{
 */

/** Compile-time major version of this header and library. */
#define GAM_MODEL_VERSION_MAJOR 0

/** Compile-time minor version of this header and library. */
#define GAM_MODEL_VERSION_MINOR 1

/**
 * Packed compile-time version: (1000 * ::GAM_MODEL_VERSION_MAJOR +
 * 10 * ::GAM_MODEL_VERSION_MINOR).
 *
 * A program may compare this macro with the value returned by
 * ::gamModelGetVersion to detect a header/library mismatch.
 */
#define GAM_MODEL_VERSION \
    ((GAM_MODEL_VERSION_MAJOR * 1000) + (GAM_MODEL_VERSION_MINOR * 10))

/** @} */

/**
 * @brief Result codes returned by every OpenGAM C ABI function.
 *
 * ::GAM_SUCCESS is zero and is the only success value. All failures are
 * non-zero. ::GAM_ERROR_NOT_READY is not a failure: it means previously
 * issued asynchronous work has not completed yet, and is distinct from
 * ::GAM_SUCCESS, which means completion.
 *
 * Where a enumerator has the same name as a CUDA driver ::CUresult
 * code, the numeric value is identical. ::GAM_ERROR_OUT_OF_RANGE (702)
 * has no CUDA driver counterpart and is defined here for a supported
 * address or range that lies outside the admitted window.
 *
 * Codes that no current entry point can return are still part of this
 * stable ABI. Later functions will use them with the meanings documented
 * below; callers may handle them now.
 */
typedef enum GAMresult_enum
{
    /**
     * The API call completed without errors.
     * For a query of in-flight work, this also means the work has finished.
     */
    GAM_SUCCESS = 0,

    /**
     * One or more parameters are not within an acceptable range of values.
     * This includes a required pointer that is null, a size that is not
     * consistent with the described object, and a field whose value is
     * reserved or out of domain.
     */
    GAM_ERROR_INVALID_VALUE = 1,

    /**
     * The API call could not allocate enough memory or other resources
     * to perform the requested operation.
     */
    GAM_ERROR_OUT_OF_MEMORY = 2,

    /**
     * The library has not been initialized, or initialization failed.
     * A call that requires a prior successful setup returns this code
     * until that setup has completed.
     */
    GAM_ERROR_NOT_INITIALIZED = 3,

    /**
     * The library is in the process of shutting down, or has already
     * been shut down. Remaining calls that require a live library
     * instance return this code.
     */
    GAM_ERROR_DEINITIALIZED = 4,

    /**
     * No usable device was detected. This is returned when a lookup or
     * enumeration finds an empty device set.
     */
    GAM_ERROR_NO_DEVICE = 100,

    /**
     * The device ordinal, handle, or other device identity supplied by
     * the caller does not refer to a valid device, or the requested
     * action is invalid for that device.
     */
    GAM_ERROR_INVALID_DEVICE = 101,

    /**
     * A map or register operation failed. The named resource was not
     * made available at the requested address or handle.
     */
    GAM_ERROR_MAP_FAILED = 205,

    /**
     * An unmap or unregister operation failed. The named resource was
     * not released from the requested address or handle.
     */
    GAM_ERROR_UNMAP_FAILED = 206,

    /**
     * The specified array is currently mapped and therefore cannot be
     * destroyed until it is unmapped.
     */
    GAM_ERROR_ARRAY_IS_MAPPED = 207,

    /**
     * The resource is already mapped. A second map of the same object
     * at a conflicting address or with a conflicting state is refused.
     */
    GAM_ERROR_ALREADY_MAPPED = 208,

    /**
     * The resource is not mapped. An operation that requires an active
     * mapping returns this code.
     */
    GAM_ERROR_NOT_MAPPED = 211,

    /**
     * A mapped resource is not available for access as an array.
     */
    GAM_ERROR_NOT_MAPPED_AS_ARRAY = 212,

    /**
     * A mapped resource is not available for access as a pointer.
     */
    GAM_ERROR_NOT_MAPPED_AS_POINTER = 213,

    /**
     * An operating-system call required by the requested operation
     * failed. The underlying OS error is not surfaced as a separate
     * ABI code.
     */
    GAM_ERROR_OPERATING_SYSTEM = 304,

    /**
     * Previously issued asynchronous work has not completed yet.
     * This is not a failure. It is distinct from ::GAM_SUCCESS, which
     * indicates completion. A later wait or query may still return
     * ::GAM_SUCCESS or a terminal error for the same work.
     */
    GAM_ERROR_NOT_READY = 600,

    /**
     * A load or store was rejected by address translation. The access
     * targeted an unmapped, protected, or otherwise inadmissible
     * address.
     */
    GAM_ERROR_ILLEGAL_ADDRESS = 700,

    /**
     * A submit or launch did not occur because it could not obtain the
     * required resources. Typical causes include exhausted queue
     * capacity or a request larger than the admitted resource limits.
     */
    GAM_ERROR_LAUNCH_OUT_OF_RESOURCES = 701,

    /**
     * The address or range is outside the supported window. This is
     * distinct from ::GAM_ERROR_ILLEGAL_ADDRESS, which reports a
     * translation reject of an otherwise well-formed access, and from
     * ::GAM_ERROR_INVALID_VALUE, which reports a malformed argument.
     */
    GAM_ERROR_OUT_OF_RANGE = 702,

    /**
     * The requested operation is not supported by the current device,
     * platform, or library build.
     */
    GAM_ERROR_NOT_SUPPORTED = 801,

    /**
     * The timeout specified for a wait operation has expired before
     * the waited-for condition became true.
     */
    GAM_ERROR_TIMEOUT = 909,

    /**
     * An unknown internal error occurred. This is a last-resort code
     * for a failure that does not map to a more specific result.
     */
    GAM_ERROR_UNKNOWN = 999
} GAMresult;

/**
 * @brief Returns the OpenGAM model version.
 *
 * Writes the version number of the loaded OpenGAM library into
 * \p *version. The version is encoded as
 * (1000 * major + 10 * minor). For example, OpenGAM 0.1 is 10.
 *
 * The value is the version of the library implementation, not merely
 * the version of this header. Compare it with ::GAM_MODEL_VERSION
 * when a program needs to detect a header/library mismatch.
 *
 * This function is always callable. It does not require a device, a
 * prior initialization call, or any other OpenGAM object. A null
 * \p version is rejected; on failure \p *version is not written.
 *
 * @param version Returns the packed OpenGAM model version. Must be
 *                non-null.
 *
 * @return
 * ::GAM_SUCCESS,
 * ::GAM_ERROR_INVALID_VALUE
 */
GAMresult GAMAPI gamModelGetVersion(int *version);

#if defined(__GNUC__) && defined(GAM_BUILD_LIBRARY)
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif /* __opengam_h__ */
