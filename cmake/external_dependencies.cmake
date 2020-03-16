include(CheckCXXSourceCompiles)
include(FetchContent)

function(setup_library SOURCE_TO_TEST)
    set(boolArgs HEADER_ONLY)
    set(oneValueArgs NAME GIT_REPOSITORY TEST_LINK_LIBS)
    set(multiValueArgs WHEN)
    cmake_parse_arguments(LIB_SETUP "${boolArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(TEST_NAME "SYSTEM_${LIB_SETUP_NAME}_WORKS")
    set(CMAKE_REQUIRED_LIBRARIES "${LIB_SETUP_TEST_LINK_LIBS}")
    check_cxx_source_compiles("${SOURCE_TO_TEST}" ${TEST_NAME})

    if (${TEST_NAME})
        if (LIB_SETUP_HEADER_ONLY)
            add_library(${LIB_SETUP_NAME} INTERFACE)
        else()
            find_package(${LIB_SETUP_NAME} REQUIRED)
        endif()
        return()
    endif()

    FetchContent_Declare(${LIB_SETUP_NAME}
                         GIT_REPOSITORY ${LIB_SETUP_GIT_REPOSITORY})
    FetchContent_GetProperties(${LIB_SETUP_NAME})
    set(POPULATED "${LIB_SETUP_NAME}_POPULATED")
    if (NOT "${POPULATED}")
        message("-- Downloading ${LIB_SETUP_NAME} from ${LIB_SETUP_GIT_REPOSITORY}")
        FetchContent_Populate(${LIB_SETUP_NAME})
        set(SRCDIR "${LIB_SETUP_NAME}_SOURCE_DIR")
        set(BINDIR "${LIB_SETUP_NAME}_BINARY_DIR")

        add_subdirectory(${${SRCDIR}} ${${BINDIR}} EXCLUDE_FROM_ALL)
        message("-- Using external ${LIB_SETUP_NAME}")
    endif()
endfunction()

function(setup_argparse)
    set(CHECK_SOURCE "#include <argparse.hpp>
            int main()
            {
              return 0;
            }")
    setup_library("${CHECK_SOURCE}"
                  NAME argparse
                  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
                  HEADER_ONLY
                  )
    if (NOT TARGET argparse::argparse)
        add_library(argparse::argparse ALIAS argparse)
    endif()
endfunction()

function(setup_doctest)
    set(CHECK_SOURCE "#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
                      #include <doctest/doctest.h>")
    setup_library("${CHECK_SOURCE}"
                  NAME doctest
                  GIT_REPOSITORY https://github.com/onqtam/doctest.git
                  HEADER_ONLY
                  )
    add_library(doctest::doctest ALIAS doctest)
endfunction()

function(setup_fmt)
    set(CHECK_SOURCE "#include <fmt/format.h>
        int main()
        {
          fmt::print(\"hello\");
          return 0;
        }")
    setup_library("${CHECK_SOURCE}"
                  NAME fmt
                  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                  TEST_LINK_LIBS fmt
                  )
endfunction()

function(setup_nlohmann_json)
    set(CHECK_SOURCE "#include <nlohmann/json.hpp>
        int main()
        {
          nlohmann::json json;
          json[0] = 1;
          return 0;
        }")
    check_cxx_source_compiles("${CHECK_SOURCE}" SYSTEM_nlohmann_json_WORKS)
    if (SYSTEM_nlohmann_json_WORKS)
        add_library(nlohmann_json INTERFACE)
    else()
        FetchContent_Declare(nlohmann_json
                             URL https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip)
        FetchContent_GetProperties(nlohmann_json)
        if (NOT nlohmann_json_POPULATED)
            FetchContent_Populate(nlohmann_json)
            add_library(nlohmann_json INTERFACE)
            target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
        endif()
    endif()
    add_library(nlohmann::json ALIAS nlohmann_json)
endfunction()

function(setup_trompeloeil)
    set(CHECK_SOURCE "#include <doctest/trompeloeil.hpp>
                      #include <trompeloeil.hpp>")
    setup_library("${CHECK_SOURCE}"
                  NAME trompeloeil
                  GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
                  HEADER_ONLY
                  )
    add_library(trompeloeil::trompeloeil ALIAS trompeloeil)
endfunction()
