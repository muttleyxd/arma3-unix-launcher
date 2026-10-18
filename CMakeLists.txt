cmake_minimum_required(VERSION 3.21)
project(arma3-unix-launcher)

include(cmake/external_dependencies.cmake)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-deprecated-declarations")
add_libraries_to_linker(COMPILER_ID GNU
                        LIBS stdc++fs
                        WHEN ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 9
                        WARNING_MESSAGE "Applying workaround for GCC 8 and lower - linking stdc++fs")

option(BUILD_DAYZ_LAUNCHER "Build DayZ Launcher instead of Arma Launcher" OFF)
option(DEVELOPER_MODE "Enable developer checks" OFF)
option(RUN_TESTS "Run tests with DocTest" OFF)
# Defaults can come from the environment, the Nix devShell exports A3UL_USE_SYSTEM_DEPS=ON
set(A3UL_USE_SYSTEM_DEPS_DEFAULT OFF)
if (DEFINED ENV{A3UL_USE_SYSTEM_DEPS})
    set(A3UL_USE_SYSTEM_DEPS_DEFAULT $ENV{A3UL_USE_SYSTEM_DEPS})
endif()
option(A3UL_USE_SYSTEM_DEPS "Never download dependencies, require them to be provided by the system (e.g. Nix)"
       ${A3UL_USE_SYSTEM_DEPS_DEFAULT})

if (A3UL_USE_SYSTEM_DEPS)
    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
endif()

option(A3UL_COVERAGE "Instrument for clang source-based coverage (see tools/ci/coverage.sh)" OFF)
if (A3UL_COVERAGE)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
endif()

if (NOT DEFINED COMMIT_COUNT)
    set(COMMIT_COUNT 0) # used for update notifications
endif()

if (DEVELOPER_MODE)
    add_definitions(-DSTATIC_TODO_ENABLED)
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
setup_argparse()
setup_curlpp()
setup_fmt()
setup_pugixml()
setup_nlohmann_json()
setup_scope_guard()
setup_steamworkssdk()
setup_spdlog()

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)
qt_standard_project_setup()

# Enable automatic resource compilation (not included in qt_standard_project_setup)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)

if (RUN_TESTS)
    setup_doctest()
    setup_trompeloeil()
    enable_testing()
    add_subdirectory(tests)
endif ()

include(external/use-backward-cpp.cmake)
