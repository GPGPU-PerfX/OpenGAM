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
 * The ABI is a stable C interface. This header exposes no C++ types, and C++ exceptions do not cross the ABI boundary.
 * All currently exported functions return a ::GAMresult and use the ::GAMAPI calling convention, as in `GAMresult
 * GAMAPI gamModelGetVersion(int* version)`.
 *
 * The first published entry point is ::gamModelGetVersion. Additional functions will be added to this header without
 * changing the meaning of existing symbols, result codes, or version encoding. New public functions must also be added
 * to `opengam.def`, to `opengam_macos.exports`, and to the function-pointer types in `opengam_typedefs.h`.
 *
 * Callers that load the shared library dynamically (`libopengam.so` or `libopengam.dylib`) should include
 * `opengam_typedefs.h` instead of this header alone.
 *
 * When building the shared library, define `GAM_BUILD_LIBRARY` so that public declarations use default visibility and
 * can be exported from `opengam.def` or `opengam_macos.exports`. Applications that include this header should not
 * define `GAM_BUILD_LIBRARY`.
 */

#ifndef __opengam_h__
#define __opengam_h__

/**
 * @def GAMAPI
 * @brief Calling convention for public entry points.
 *
 * Place this macro between the return type and the function name, the same way CUDA driver and runtime headers use
 * `CUDAAPI` / `CUDARTAPI`:
 *
 *     GAMresult GAMAPI gamModelGetVersion(int* version);
 *
 * It is empty on the supported platforms (Linux and macOS). Shared-library export is controlled by `opengam.def` on
 * Linux and by `opengam_macos.exports` on macOS, not by this macro. When `GAM_BUILD_LIBRARY` is defined, public
 * declarations in this header also receive default visibility so the linker can export the symbols named in those
 * files.
 */
#define GAMAPI

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && defined(GAM_BUILD_LIBRARY)
#pragma GCC visibility push(default)
#endif

/**
 * @defgroup GAM_VERSION Version Management
 *
 * Compile-time and run-time identification of the OpenGAM library. The packed encoding is always (1000 * major + 10 *
 * minor). For example, OpenGAM 0.1 is 10 and OpenGAM 0.2 would be 20.
 *
 * @{
 */

/** Compile-time major version of this header and library. */
#define GAM_MODEL_VERSION_MAJOR 0

/** Compile-time minor version of this header and library. */
#define GAM_MODEL_VERSION_MINOR 1

/**
 * Packed compile-time version: (1000 * ::GAM_MODEL_VERSION_MAJOR + 10 * ::GAM_MODEL_VERSION_MINOR).
 *
 * A program may compare this macro with the value returned by ::gamModelGetVersion to detect a header/library mismatch.
 */
#define GAM_MODEL_VERSION ((GAM_MODEL_VERSION_MAJOR * 1000) + (GAM_MODEL_VERSION_MINOR * 10))

/** @} */

/**
 * @brief Result codes returned by every OpenGAM C ABI function.
 *
 * ::GAM_SUCCESS is zero and is the only success value. All failures are non-zero. This first cut only defines the codes
 * that current entry points can return. Additional codes will be added when a later function needs them, without
 * changing the meaning or value of existing enumerators.
 */
typedef enum GAMresult_enum
{
    /**
     * The API call completed without errors.
     */
    GAM_SUCCESS = 0,

    /**
     * One or more parameters are not within an acceptable range of values. This includes a required pointer that is
     * null.
     */
    GAM_ERROR_INVALID_VALUE = 1
} GAMresult;

/**
 * @brief Returns the OpenGAM model version.
 *
 * Writes the version number of the loaded OpenGAM library into \p *version. The version is encoded as (1000 * major +
 * 10 * minor). For example, OpenGAM 0.1 is 10.
 *
 * The value is the version of the library implementation, not merely the version of this header. Compare it with
 * ::GAM_MODEL_VERSION when a program needs to detect a header/library mismatch.
 *
 * This function is always callable. It does not require a device, a prior initialization call, or any other OpenGAM
 * object. A null \p version is rejected; on failure \p *version is not written.
 *
 * @param version Returns the packed OpenGAM model version. Must be non-null.
 *
 * @return
 * ::GAM_SUCCESS,
 * ::GAM_ERROR_INVALID_VALUE
 */
GAMresult GAMAPI gamModelGetVersion(int* version);

#if defined(__GNUC__) && defined(GAM_BUILD_LIBRARY)
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif /* __opengam_h__ */
