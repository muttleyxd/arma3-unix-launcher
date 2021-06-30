cmake_minimum_required(VERSION 3.11)
project(arma3-unix-launcher)

include(cmake/external_dependencies.cmake)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
add_libraries_to_linker(COMPILER_ID GNU
                        LIBS stdc++fs
                        WHEN ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 9
                        WARNING_MESSAGE "Applying workaround for GCC 8 and lower - linking stdc++fs")

option(DEVELOPER_MODE "Enable developer checks" OFF)
option(RUN_TESTS "Run tests with DocTest" OFF)

if (DEVELOPER_MODE)
    add_definitions(-DSTATIC_TODO_ENABLED)
endif ()

include_directories(external/static-todo)
setup_argparse()
setup_fmt()
setup_pugixml()
setup_nlohmann_json()
setup_scope_guard()
include(external/steamworks-sdk.cmake)

add_subdirectory(src)

if (RUN_TESTS)
    setup_doctest()
    setup_trompeloeil()
    enable_testing()
    add_subdirectory(tests)
endif ()

include(external/use-backward-cpp.cmake)
