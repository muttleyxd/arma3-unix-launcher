function(create_fuzzing_test)
    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        message(FATAL_ERROR "Fuzzing tests are only supported on Clang")
    endif()

    set(oneValueArgs TEST_GROUP TEST_NAME)
    set(multiValueArgs INCLUDES LINK_LIBS SOURCES)
    cmake_parse_arguments(CREATE_TEST "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(NAME "${CREATE_TEST_TEST_NAME}")
    set(GROUP "${CREATE_TEST_TEST_GROUP}")
    set(INCLUDES "${CREATE_TEST_INCLUDES}")
    set(LINK_LIBS "${CREATE_TEST_LINK_LIBS}")
    set(TARGET_NAME "test_fuzzing_${GROUP}_${NAME}")

    add_executable(${TARGET_NAME} ${CREATE_TEST_SOURCES})
    target_compile_options(${TARGET_NAME} PRIVATE -g -O1 -fsanitize=fuzzer,address)
    target_link_options(${TARGET_NAME} PRIVATE -fsanitize=fuzzer,address)
    target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES} ${CMAKE_SOURCE_DIR}/tests/fuzzing)
    target_link_libraries(${TARGET_NAME} PRIVATE ${LINK_LIBS} doctest::doctest fmt::fmt trompeloeil::trompeloeil)
    add_test(${TARGET_NAME} ${TARGET_NAME} -max_total_time=20)

    add_executable(${TARGET_NAME}_direct ${CREATE_TEST_SOURCES} ${CMAKE_SOURCE_DIR}/tests/fuzzing/direct.cpp)
    target_include_directories(${TARGET_NAME}_direct PRIVATE ${INCLUDES} ${CMAKE_SOURCE_DIR}/tests/fuzzing)
    target_link_libraries(${TARGET_NAME}_direct PRIVATE ${LINK_LIBS} doctest::doctest fmt::fmt trompeloeil::trompeloeil)
endfunction()

function(create_unit_test)
    set(oneValueArgs TEST_GROUP TEST_NAME)
    set(multiValueArgs INCLUDES LINK_LIBS SOURCES)
    cmake_parse_arguments(CREATE_TEST "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(NAME "${CREATE_TEST_TEST_NAME}")
    set(GROUP "${CREATE_TEST_TEST_GROUP}")
    set(INCLUDES "${CREATE_TEST_INCLUDES}")
    set(LINK_LIBS "${CREATE_TEST_LINK_LIBS}")
    set(TARGET_NAME "test_unit_${GROUP}_${NAME}")

    add_executable(${TARGET_NAME} ${CREATE_TEST_SOURCES})
    target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES} ${CMAKE_SOURCE_DIR}/tests/unit)
    target_link_libraries(${TARGET_NAME} PRIVATE ${LINK_LIBS} doctest::doctest trompeloeil::trompeloeil)
    add_test(${TARGET_NAME} ${TARGET_NAME})
endfunction()

function(setup_objects)
    set(oneValueArgs TARGET_NAME OUTPUT_TARGET_LIST)
    set(multiValueArgs INCLUDES SOURCES)
    cmake_parse_arguments(SETUP_OBJECTS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(OUTPUT_TARGETS "")

    foreach(SOURCE_FILE ${SETUP_OBJECTS_SOURCES}) # src/common/some_file.cpp
        get_filename_component(SOURCE_NAME_DIRECTORY "${SOURCE_FILE}" DIRECTORY) # src/common or empty
        string(REPLACE "/" "_" SOURCE_NAME_DIRECTORY_TARGET_NAME "${SOURCE_NAME_DIRECTORY}") # src_common or empty

        set(TARGET_NAME_PREFIX "obj_${SETUP_OBJECTS_TARGET_NAME}") # obj_module
        if (NOT "${SOURCE_NAME_DIRECTORY_TARGET_NAME}" STREQUAL "")
            set(TARGET_NAME_PREFIX "${TARGET_NAME_PREFIX}_${SOURCE_NAME_DIRECTORY_TARGET_NAME}") # obj_module_src_common
        endif()

        get_filename_component(SOURCE_NAME_WE "${SOURCE_FILE}" NAME_WE) # some_file
        set(TARGET_NAME "${TARGET_NAME_PREFIX}_${SOURCE_NAME_WE}") # obj_module_generated_api_some_file

        get_filename_component(SOURCE_EXTENSION "${SOURCE_FILE}" EXT) # .cpp or .hpp
        if (TARGET ${TARGET_NAME} AND "${SOURCE_EXTENSION}" STREQUAL ".hpp")
            target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILE}) # add header to target
        endif()

        if ("${SOURCE_EXTENSION}" STREQUAL ".cpp")
            add_library(${TARGET_NAME} OBJECT ${SOURCE_FILE})
            target_compile_options(${TARGET_NAME} PRIVATE "-fpic")
            target_include_directories(${TARGET_NAME} PUBLIC ${SETUP_OBJECTS_INCLUDES})
            list(APPEND OUTPUT_TARGETS $<TARGET_OBJECTS:${TARGET_NAME}>)
        endif()
    endforeach()

    if (NOT "${SETUP_OBJECTS_OUTPUT_TARGET_LIST}" STREQUAL "")
        set("${SETUP_OBJECTS_OUTPUT_TARGET_LIST}" "${OUTPUT_TARGETS}" PARENT_SCOPE)
    endif()
endfunction()

function(add_flags_to_compiler)
    set(oneValueArgs COMPILER_ID FLAGS WARNING_MESSAGE)
    set(multiValueArgs WARN_WHEN WHEN)
    cmake_parse_arguments(ADD_FLAGS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT "${ADD_FLAGS_WHEN}" STREQUAL "")
        if (NOT ${ADD_FLAGS_WHEN})
            return()
        endif()
    endif()

    if (NOT "${ADD_FLAGS_WARN_WHEN}" STREQUAL "")
        if (${ADD_FLAGS_WARN_WHEN})
            message(WARNING "Warning: ${ADD_FLAGS_WARNING_MESSAGE}; ${ADD_FLAGS_WARN_WHEN}")
        endif()
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "${ADD_FLAGS_COMPILER_ID}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADD_FLAGS_FLAGS}" PARENT_SCOPE)
    endif()
endfunction()

function(add_libraries_to_linker)
    set(oneValueArgs COMPILER_ID LIBS WARNING_MESSAGE)
    set(multiValueArgs WHEN)
    cmake_parse_arguments(ADD_LIBRARIES "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT "${ADD_LIBRARIES_WHEN}" STREQUAL "")
        if (NOT ${ADD_LIBRARIES_WHEN})
            return()
        endif()
    endif()

    if (NOT "${ADD_LIBRARIES_WARNING_MESSAGE}" STREQUAL "")
        message(WARNING "Warning: ${ADD_LIBRARIES_WARNING_MESSAGE}")
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "${ADD_LIBRARIES_COMPILER_ID}")
        link_libraries(${ADD_LIBRARIES_LIBS})
    endif()
endfunction()

function(add_compile_options_to_target)
    set(oneValueArgs COMPILER_ID TARGET WARNING_MESSAGE)
    set(multiValueArgs COMPILE_OPTIONS)
    cmake_parse_arguments(ACO "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "${ACO_COMPILER_ID}")
        if (NOT "${ACO_WARNING_MESSAGE}" STREQUAL "")
            message(WARNING "Warning: ${ACO_WARNING_MESSAGE}")
        endif()

        target_compile_options(${ACO_TARGET} INTERFACE ${ACO_COMPILE_OPTIONS})
    endif()
endfunction()
