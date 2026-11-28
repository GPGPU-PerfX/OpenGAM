cmake_minimum_required(VERSION 3.16)

project(OpenGAM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Apply sanitizer flags to every target in this build, including
# GoogleTest, which is added below with add_subdirectory.
option(OPENGAM_ENABLE_SANITIZERS
    "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(OPENGAM_ENABLE_SANITIZERS)
    if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
            OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        message(FATAL_ERROR "OPENGAM_ENABLE_SANITIZERS requires GCC or Clang")
    endif()
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(opengam SHARED src/opengam.cpp)
add_library(OpenGAM::opengam ALIAS opengam)

set_target_properties(opengam PROPERTIES
    OUTPUT_NAME opengam
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(opengam
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(opengam PRIVATE GAM_BUILD_LIBRARY)
target_compile_options(opengam PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>)

# Public export lists: opengam.def is a GNU version script on Linux;
# opengam_macos.exports is the exported-symbols list on macOS.
# Linux and macOS are the only supported platforms for now.
if (APPLE)
    target_link_options(opengam PRIVATE
        "LINKER:-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/opengam_macos.exports")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_options(opengam PRIVATE
        "LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/opengam.def")
else()
    message(FATAL_ERROR "OpenGAM currently supports Linux and macOS only")
endif()

# GoogleTest is a git submodule at third_party/googletest. Compile it
# into the CMake build directory (build/googletest by default). Do not
# install it with the library.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
    message(FATAL_ERROR
        "GoogleTest is missing from third_party/googletest. "
        "Initialize the submodule and re-run CMake:\n"
        "  git submodule update --init")
endif()
set(INSTALL_GTEST OFF CACHE BOOL "Do not install GoogleTest" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Do not build GoogleMock" FORCE)
add_subdirectory(third_party/googletest "${CMAKE_BINARY_DIR}/googletest" EXCLUDE_FROM_ALL)

enable_testing()
add_executable(opengam_abi_tests tests/gam_model_get_version_test.cpp)
target_link_libraries(opengam_abi_tests PRIVATE opengam GTest::gtest_main)
add_test(NAME opengam_abi_tests COMMAND opengam_abi_tests)

install(TARGETS opengam
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES include/opengam.h include/opengam_typedefs.h DESTINATION include)
