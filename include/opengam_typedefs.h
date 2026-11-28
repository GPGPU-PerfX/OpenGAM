/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file opengam_typedefs.h
 * @brief Function-pointer types for loading the OpenGAM C ABI dynamically.
 *
 * Include this header when the caller obtains entry points at run time (for example with `dlopen` and `dlsym`) rather
 * than linking `libopengam.so` or `libopengam.dylib` directly:
 *
 *     #include <opengam_typedefs.h>
 *
 * `opengam.h` remains the declaration of the linkable C API. This header includes it, then defines a function-pointer
 * type for each public entry point. Each type is named `PFN_<function>_v<version>`, where <version> is the packed
 * OpenGAM model version that introduced that signature (`1000 * major + 10 * minor`). The unversioned `PFN_<function>`
 * macro always aliases the latest type. For OpenGAM 0.1, that packed version is 10, so ::gamModelGetVersion is
 * `PFN_gamModelGetVersion_v10`.
 *
 * When a new public function is added, declare it in `opengam.h`, export it from `opengam.def` and
 * `opengam_macos.exports`, and add both the versioned typedef and the latest-version macro here in the same change.
 */

#ifndef __opengam_typedefs_h__
#define __opengam_typedefs_h__

#include <opengam.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def PFN_gamModelGetVersion
 * @brief Latest function-pointer type for ::gamModelGetVersion.
 */
#define PFN_gamModelGetVersion PFN_gamModelGetVersion_v10

/**
 * @brief Function pointer for ::gamModelGetVersion, introduced in OpenGAM 0.1.
 *
 * The pointed-to function has the same contract as the declaration in `opengam.h`: it writes the packed model version
 * to *\p version and returns ::GAM_SUCCESS, or ::GAM_ERROR_INVALID_VALUE when \p version is null.
 */
typedef GAMresult GAMAPI (*PFN_gamModelGetVersion_v10)(int* version);

#ifdef __cplusplus
}
#endif

#endif /* __opengam_typedefs_h__ */
