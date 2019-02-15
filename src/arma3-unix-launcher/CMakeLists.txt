set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Svg CONFIG REQUIRED)

file(GLOB SOURCES *.cpp *.hpp *.qrc *.ui)
if (RUN_TESTS)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/tests/src/tests.cpp)
endif ()

add_executable(arma3-unix-launcher ${SOURCES})
target_link_libraries(arma3-unix-launcher
    arma3-unix-launcher-library
    Qt5::Widgets
    Qt5::Svg)
