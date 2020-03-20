enable_testing()

option(RUN_TESTS_FUZZING "Run fuzzing tests with libFuzzer" OFF)
option(RUN_TESTS_UNIT "Run unit tests with DocTest" ON)

if (RUN_TESTS_FUZZING)
    add_subdirectory(fuzzing)
endif()

if (RUN_TESTS_UNIT)
    add_subdirectory(unit)
endif()
