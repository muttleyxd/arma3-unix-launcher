add_executable(tests tests.cpp)
target_link_libraries(tests arma3-unix-launcher-library)

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
