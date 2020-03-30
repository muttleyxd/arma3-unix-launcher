create_fuzzing_test(TEST_GROUP library
                    TEST_NAME  cppfilter_fuzz
                    SOURCES    test_cppfilter_fuzz.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/cppfilter.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/cppfilter.hpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.hpp
                    INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                    )

create_fuzzing_test(TEST_GROUP library
                    TEST_NAME  mod_fuzz
                    SOURCES    test_mod_fuzz.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/mod.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/mod.hpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.hpp
                               ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                               ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                               ${CMAKE_SOURCE_DIR}/tests/unit/mock/filesystem_utils.hpp
                               ${CMAKE_SOURCE_DIR}/tests/unit/mock/std_utils.hpp
                    INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                               ${CMAKE_SOURCE_DIR}/src/common
                               ${CMAKE_SOURCE_DIR}/tests/unit
                    )

create_fuzzing_test(TEST_GROUP library
                    TEST_NAME  vdf_fuzz
                    SOURCES    test_vdf_fuzz.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.hpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.cpp
                               ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.hpp
                    INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                               ${CMAKE_SOURCE_DIR}/src/common
                    )
