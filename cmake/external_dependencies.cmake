
include(CheckCXXSourceCompiles)
include(FetchContent)

function(setup_library SOURCE_TO_TEST)
    set(boolArgs HEADER_ONLY)
    set(oneValueArgs NAME CXX_FLAGS GIT_REPOSITORY GIT_TAG TEST_DEFINITIONS TEST_LINK_LIBS)
    set(multiValueArgs WHEN)
    cmake_parse_arguments(LIB_SETUP "${boolArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(TEST_NAME "SYSTEM_${LIB_SETUP_NAME}_WORKS")
    set(CMAKE_REQUIRED_DEFINITIONS "${LIB_SETUP_TEST_DEFINITIONS}")
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

    if (NOT LIB_SETUP_GIT_TAG)
        set(LIB_SETUP_GIT_TAG master)
    endif()

    FetchContent_Declare(${LIB_SETUP_NAME}
                         GIT_REPOSITORY ${LIB_SETUP_GIT_REPOSITORY}
                         GIT_TAG ${LIB_SETUP_GIT_TAG})
    FetchContent_GetProperties(${LIB_SETUP_NAME})
    set(POPULATED "${LIB_SETUP_NAME}_POPULATED")
    if (NOT "${POPULATED}")
        message("-- Downloading ${LIB_SETUP_NAME} from ${LIB_SETUP_GIT_REPOSITORY}")
        FetchContent_Populate(${LIB_SETUP_NAME})
        set(SRCDIR "${LIB_SETUP_NAME}_SOURCE_DIR")
        set(BINDIR "${LIB_SETUP_NAME}_BINARY_DIR")

        string(APPEND CMAKE_CXX_FLAGS " ${LIB_SETUP_CXX_FLAGS}")
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
                  GIT_TAG 45664c4
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
        #include <string_view>

        template<typename... Args>
        using format_string_t = fmt::format_string<Args...>; // required by spdlog

        int main()
        {
          std::string_view message = \"hello\";
          fmt::print(\"{}\", message);
          return 0;
        }")
    setup_library("${CHECK_SOURCE}"
                  NAME fmt
                  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                  TEST_LINK_LIBS fmt
                  )

    if (NOT TARGET fmt::fmt)
        add_library(fmt::fmt ALIAS fmt)
    endif()
    if (NOT TARGET fmt::fmt-header-only)
        add_library(fmt::fmt-header-only ALIAS fmt::fmt)
    endif()
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

function(setup_pugixml)
    set(CHECK_SOURCE "#include <pugixml.hpp>
        int main()
        {
            pugi::xml_document doc;
            return 0;
        }")
    setup_library("${CHECK_SOURCE}"
                  NAME pugixml
                  GIT_REPOSITORY https://github.com/muttleyxd/pugixml.git
                  GIT_TAG simple-build-for-a3ul
                  TEST_LINK_LIBS pugixml
                  )
    get_target_property(TARGET_TYPE pugixml TYPE)
    if ("${TARGET_TYPE}" STREQUAL "SHARED_LIBRARY")
        target_compile_options(pugixml INTERFACE "-fpic")
    endif()
    get_target_property(TARGET_IMPORTED pugixml IMPORTED)
    if (TARGET_IMPORTED)
        set_target_properties(pugixml PROPERTIES IMPORTED_GLOBAL TRUE)
    endif()
    add_library(pugixml::pugixml ALIAS pugixml)
endfunction()

function(setup_scope_guard)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/scope_guard)
endfunction()

function(setup_spdlog)
    if (NOT DEFINED SPDLOG_BUILD_SHARED)
        set(SPDLOG_BUILD_SHARED ON)
    endif()
    set(SPDLOG_FMT_EXTERNAL OFF)
    set(SPDLOG_FMT_EXTERNAL_HO ON)
    set(CHECK_SOURCE "#include <spdlog/spdlog.h>
        #if !__has_include(<spdlog/cfg/env.h>) // required by verboseness level setting
        #error libspdlog 1.9 or newer is required
        #endif
        int main()
        {
            spdlog::info(\"hello\");
            return 0;
        }")

    set(FMT_TARGET_NAME "fmt")
    if (TARGET fmt-header-only)
        set(FMT_TARGET_NAME "fmt-header-only")
    endif()

    setup_library("${CHECK_SOURCE}"
                  NAME spdlog
                  GIT_REPOSITORY https://github.com/gabime/spdlog.git
                  GIT_TAG v1.x
                  TEST_DEFINITIONS -DSPDLOG_FMT_EXTERNAL
                  TEST_LINK_LIBS ${FMT_TARGET_NAME}
                  CXX_FLAGS "-Wno-attributes -Wno-reorder"
                  )
endfunction()

function(setup_trompeloeil)
    set(CHECK_SOURCE "#if __has_include(<doctest/trompeloeil.hpp>)
                      #else
                      #error no doctest/trompeloeil.hpp
                      #endif
                      #if __has_include(<trompeloeil.hpp>)
                      #else
                      #error no trompeloeil.hpp
                      #endif
                      int main() { }")
    setup_library("${CHECK_SOURCE}"
                  NAME trompeloeil
                  GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
                  GIT_TAG 64fd171
                  HEADER_ONLY
                  )
    add_library(trompeloeil::trompeloeil ALIAS trompeloeil)
endfunction()
