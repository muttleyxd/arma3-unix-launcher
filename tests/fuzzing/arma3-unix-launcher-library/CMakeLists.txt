create_fuzzing_test(TEST_GROUP library
                    TEST_NAME  vdf_fuzz
                    SOURCES    test_vdf_fuzz.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.hpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.hpp
                    INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                    )
