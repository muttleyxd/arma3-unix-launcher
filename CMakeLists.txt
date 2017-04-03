cmake_minimum_required (VERSION 2.8.12)
project (arma3-unix-launcher)

set (CMAKE_CXX_STANDARD 11)

# GTKMM
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)
include_directories (${GTKMM_INCLUDE_DIRS})
link_directories (${GTKMM_LIBRARY_DIRS})

#build executable
add_executable (arma3-unix-launcher main.cpp Filesystem.cpp Logger.cpp MainWindow.cpp Mod.cpp Settings.cpp Utils.cpp VDF.cpp VDFKey.cpp)
target_link_libraries (arma3-unix-launcher ${GTKMM_LIBRARIES} pthread)
set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")

if(APPLE)
    add_custom_command (TARGET arma3-unix-launcher POST_BUILD COMMAND mkdir arma3-unix-launcher.app && cd arma3-unix-launcher.app && mkdir Contents && cd Contents && mkdir MacOS)
    add_custom_command (TARGET arma3-unix-launcher POST_BUILD COMMAND cp arma3-unix-launcher ${CMAKE_CURRENT_BINARY_DIR}/arma3-unix-launcher.app/Contents/MacOS)
    add_custom_command (TARGET arma3-unix-launcher POST_BUILD COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/arma3-unix-launcher.app/Contents)
    add_custom_command (TARGET arma3-unix-launcher POST_BUILD COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/MainForm.glade ${CMAKE_CURRENT_BINARY_DIR}/arma3-unix-launcher.app/Contents/MacOS)
else(APPLE)
    add_custom_command (TARGET arma3-unix-launcher POST_BUILD COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/MainForm.glade ${CMAKE_CURRENT_BINARY_DIR}/MainForm.glade)
endif(APPLE)

# Installation
if(APPLE)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/arma3-unix-launcher.app DESTINATION /Applications USE_SOURCE_PERMISSIONS)
else(APPLE)
    install (TARGETS arma3-unix-launcher DESTINATION /usr/bin)
    install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.desktop DESTINATION /usr/share/applications)
    install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/MainForm.glade DESTINATION /usr/share/arma3-unix-launcher)
endif(APPLE)
