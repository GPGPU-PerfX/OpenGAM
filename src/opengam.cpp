/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Implementation of the public OpenGAM C ABI. This translation unit currently exports only gamModelGetVersion. C++
 * exceptions are disabled for the library build and must not cross the ABI.
 */

#include <opengam.h>

// =====================================================================================================================
extern "C" GAMresult GAMAPI gamModelGetVersion(int* version)
{
    /* A null out-parameter is the only failure this call can report. */
    if (version == nullptr)
    {
        return GAM_ERROR_INVALID_VALUE;
    }

    /* Report the implementation version packed by GAM_MODEL_VERSION. */
    *version = GAM_MODEL_VERSION;
    return GAM_SUCCESS;
}
