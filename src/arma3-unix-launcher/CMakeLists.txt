set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

file(GLOB SOURCES *.cpp *.hpp *.ui)

add_executable(arma3-unix-launcher ${SOURCES})
target_link_libraries(arma3-unix-launcher
    arma3-unix-launcher-library
    Qt5::Widgets)
