enable_testing()

include(ExternalProject)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST gtest)

#set(GTEST_LOCATION ${CMAKE_BINARY_DIR}/gtest)

#ExternalProject_Add(googletest
#    GIT_REPOSITORY https://github.com/google/googletest
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_LOCATION}
#)

#include_directories(${GTEST_LOCATION}/include)
#link_directories(${GTEST_LOCATION}/lib)

file(GLOB TEST_SRC *.cpp)

foreach(test ${TEST_SRC})
	get_filename_component(binary_name ${test} NAME_WE)
    add_executable(${binary_name} ${test} ${BACKWARD_ENABLE})
	target_include_directories(${binary_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(${binary_name} gtest gtest_main pthread arma3-unix-launcher-library)
    add_backward(${binary_name})
    add_test(NAME ${binary_name} COMMAND ${binary_name})
endforeach()

add_custom_target(link_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/tests/test-files ${CMAKE_BINARY_DIR}/tests/test-files)

if (RUN_COVERAGE)
    find_program(GCOVR_EXECUTABLE gcovr)
    if (NOT GCOVR_EXECUTABLE)
        message(FATAL_ERROR "gcovr is not installed. Cannot run coverage")
    endif ()
    add_custom_target(coverage_report
                      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/coverage"
                      COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/coverage"
                      COMMAND ${GCOVR_EXECUTABLE} -r \"${CMAKE_SOURCE_DIR}\" --filter=\"${CMAKE_SOURCE_DIR}/include.*|${CMAKE_SOURCE_DIR}/src.*\" --html -o \"${CMAKE_BINARY_DIR}/coverage/coverage.html\" --html-details
                      )
endif ()
