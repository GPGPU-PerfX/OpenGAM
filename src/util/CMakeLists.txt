# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Internal utility static library. Linked into libopengam and
# gamUnittests. Not installed and not part of the public C ABI.

add_library(util STATIC
    gam_log.cpp
    gam_assert.cpp
)

# Name the archive libutil.a (or the platform equivalent). Place it at
# the root of the build tree, next to the shared library and
# gamUnittests.
set_target_properties(util PROPERTIES
    OUTPUT_NAME util
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

target_include_directories(util
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# The shared library is built with exceptions and RTTI disabled.
# Keep the same language dialect here so util can be linked into it.
target_compile_options(util PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
)

# dbg and dbg_asan configure CMAKE_BUILD_TYPE=Debug, so the logging
# and assertion macros stay enabled there. rel uses Release, and the
# macros compile out.
target_compile_definitions(util
    PUBLIC
        $<$<CONFIG:Debug>:GAM_ENABLE_DEBUG>
)

find_package(Threads REQUIRED)
target_link_libraries(util PUBLIC Threads::Threads)
