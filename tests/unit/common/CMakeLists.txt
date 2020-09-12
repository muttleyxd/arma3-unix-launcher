enable_testing()

create_unit_test(TEST_GROUP common
                 TEST_NAME  std_utils
                 SOURCES    test_std_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/std_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/std_utils.hpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only ${CMAKE_DL_LIBS}
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  string_utils
                 SOURCES    test_string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only
                 )
