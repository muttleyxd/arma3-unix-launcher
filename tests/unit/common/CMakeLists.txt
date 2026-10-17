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
                 LINK_LIBS  fmt::fmt-header-only spdlog::spdlog ${CMAKE_DL_LIBS}
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

create_unit_test(TEST_GROUP common
                 TEST_NAME  mod_search
                 SOURCES    test_mod_search.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  workshop_links
                 SOURCES    test_workshop_links.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/workshop_links.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/workshop_links.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  ca_bundle
                 SOURCES    test_ca_bundle.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/ca_bundle.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/ca_bundle.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  http_client
                 SOURCES    test_http_client.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/http_client.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/http_client.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  curl fmt::fmt-header-only spdlog::spdlog
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  workshop_details
                 SOURCES    test_workshop_details.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/workshop_details.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/workshop_details.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only nlohmann::json
                 )

create_unit_test(TEST_GROUP common
                 TEST_NAME  thumbnail_cache
                 SOURCES    test_thumbnail_cache.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/thumbnail_cache.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/thumbnail_cache.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/common
                 )
