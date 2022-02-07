enable_testing()

create_unit_test(TEST_GROUP library
                 TEST_NAME  arma3client
                 SOURCES    test_arma3client.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/arma3client.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/arma3client.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/file_not_found.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/file_not_found.hpp

                            ${CMAKE_SOURCE_DIR}/src/common/exceptions/path_no_access.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/exceptions/path_no_access.hpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/filesystem_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/std_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/vdf.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                            ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only spdlog::spdlog
                 )

create_unit_test(TEST_GROUP library
                 TEST_NAME  cppfilter
                 SOURCES    test_cppfilter.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/cppfilter.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/cppfilter.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                            ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only
                 )

 create_unit_test(TEST_GROUP library
                  TEST_NAME  html_preset_export
                  SOURCES    test_html_preset_export.cpp
                             ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/html_preset_export.cpp
                             ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/html_preset_export.hpp

                             ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                  INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                             ${CMAKE_SOURCE_DIR}/src/common
                  LINK_LIBS  fmt::fmt-header-only
                  )

create_unit_test(TEST_GROUP library
                 TEST_NAME  mod
                 SOURCES    test_mod.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/mod.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/mod.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.hpp

                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/filesystem_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/std_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/vdf.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                            ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only spdlog::spdlog
                 )

create_unit_test(TEST_GROUP library
                 TEST_NAME  steam_utils
                 SOURCES    test_steam_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/steam_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/steam_utils.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/directory_not_found.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/steam_install_not_found.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/steam_install_not_found.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/steam_workshop_directory_not_found.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/steam_workshop_directory_not_found.hpp

                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/string_utils.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/filesystem_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/std_utils.hpp
                            ${CMAKE_SOURCE_DIR}/tests/unit/mock/vdf.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                            ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only spdlog::spdlog
                 )

create_unit_test(TEST_GROUP library
                 TEST_NAME  vdf
                 SOURCES    test_vdf.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/vdf.hpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library/exceptions/syntax_error.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher-library
                            ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  fmt::fmt-header-only
                 )
