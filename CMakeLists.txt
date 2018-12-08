cmake_minimum_required(VERSION 3.8.2)
project(arma3-unix-launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter")

option(RUN_TESTS "Run tests with GoogleTest" OFF)
option(RUN_COVERAGE "Run tests with Coverage" OFF)
if (RUN_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

add_subdirectory(external/fmt)
include_directories(external/fmt/include)
include_directories(external/doctest)
include_directories(tests/include)
add_subdirectory(src)

if (RUN_TESTS)
    add_subdirectory(tests)
endif ()

include(external/use-backward-cpp.cmake)
