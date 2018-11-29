cmake_minimum_required(VERSION 3.8.2)
project(arma3-unix-launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter")

option(RUN_TESTS "Run tests with GoogleTest" OFF)
option(RUN_COVERAGE "Run tests with Coverage" OFF)
if (RUN_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

add_subdirectory(external/backward-cpp)
add_subdirectory(src)

if (RUN_TESTS)
    add_subdirectory(tests)
endif ()


get_directory_property(MY_LIBRARIES_NAMES BUILDSYSTEM_TARGETS)
message("TARGETS: ${MY_LIBRARIES_NAMES}")

function(dude_please_dump_cmake_variables)

  get_cmake_property(_variableNames VARIABLES)

  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName} = ${${_variableName}}")
  endforeach()

endfunction()


#dude_please_dump_cmake_variables()

function(get_all_targets _result _dir)
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        get_all_targets(${_result} "${_subdir}")
    endforeach()
    get_property(_sub_targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)
    set(${_result} ${${_result}} ${_sub_targets} PARENT_SCOPE)
endfunction()

get_all_targets(DUPA "${CMAKE_SOURCE_DIR}")
message("DUPA: ${DUPA}")