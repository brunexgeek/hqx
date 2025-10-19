cmake_minimum_required(VERSION 3.18)
project(hqx)

set(HQX_MAJOR_VERSION 0)
set(HQX_MINOR_VERSION 1)
set(HQX_PATCH_VERSION 0)

if (CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "No build type selected, default to 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(HQX_BUILD_DEBUG 1)
else()
    set(HQX_BUILD_RELEASE 1)
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

if (UNIX)
    set(ENABLE_PROFILING OFF CACHE BOOL "")
    set(ENABLE_SANITIZER OFF CACHE BOOL "")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c99 -fPIC -Werror=incompatible-pointer-types -Werror=implicit-function-declaration -Werror=return-type -Werror=int-conversion -Werror=maybe-uninitialized -Wunused-function -Wunused-variable -Wmissing-declarations -Werror=sign-conversion -Wmissing-prototypes -Wl,--no-undefined -Wconversion")

    if (ENABLE_PROFILING)
        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
    else()
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -O3")
    endif()

    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} -g -O0")

    if (ENABLE_SANITIZER)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fsanitize=address")
    endif()
endif()

add_library(hqx STATIC
	"source/util.c"
	"source/hq2x.c"
    "source/hq3x.c")
target_include_directories(hqx
    PUBLIC "include")
set_target_properties(hqx PROPERTIES
    OUTPUT_NAME "hqx"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PREFIX "lib" )

add_executable(example "source/main.c")
target_link_libraries(example hqx)
set_target_properties(example PROPERTIES
    OUTPUT_NAME "example"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
