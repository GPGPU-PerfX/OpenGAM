cmake_minimum_required(VERSION 3.16)

project(OpenGAM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

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

# Public export list in opengam.def: GNU version script on ELF, module
# definition file on Windows.
if (WIN32)
    target_sources(opengam PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/opengam.def)
elseif (UNIX AND NOT APPLE)
    target_link_options(opengam PRIVATE
        "LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/opengam.def")
endif()

option(GAM_ENABLE_SANITIZERS
    "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(GAM_ENABLE_SANITIZERS)
    if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
            OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        message(FATAL_ERROR "GAM_ENABLE_SANITIZERS requires GCC or Clang")
    endif()
    target_compile_options(opengam PUBLIC
        -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(opengam PUBLIC -fsanitize=address,undefined)
endif()

install(TARGETS opengam
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES include/opengam.h DESTINATION include)
