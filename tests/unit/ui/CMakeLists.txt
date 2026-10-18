enable_testing()

# Widget tests run headless on Qt's offscreen platform
find_package(Qt6 REQUIRED COMPONENTS Test)
create_unit_test(TEST_GROUP ui
                 TEST_NAME  modtablewidget
                 SOURCES    test_modtablewidget.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/modtablewidget.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/modtablewidget.h
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/tablewidgetcheckboxitem.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/tablewidgetcheckboxitem.h
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  Qt6::Widgets fmt::fmt-header-only
                 )

create_unit_test(TEST_GROUP ui
                 TEST_NAME  previewstrip
                 SOURCES    test_previewstrip.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/previewstrip.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/previewstrip.h
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/modtablewidget.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/modtablewidget.h
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/tablewidgetcheckboxitem.cpp
                            ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher/tablewidgetcheckboxitem.h
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.cpp
                            ${CMAKE_SOURCE_DIR}/src/common/mod_search.hpp

                            ${CMAKE_SOURCE_DIR}/tests/unit/default_test_reporter.hpp
                 INCLUDES   ${CMAKE_SOURCE_DIR}/src/arma3-unix-launcher ${CMAKE_SOURCE_DIR}/src/common
                 LINK_LIBS  Qt6::Widgets Qt6::Test fmt::fmt-header-only
                 )
