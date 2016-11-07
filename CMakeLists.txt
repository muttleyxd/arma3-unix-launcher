cmake_minimum_required (VERSION 3.1)
project (arma3-linux-launcher)

set (CMAKE_CXX_STANDARD 11)

# GTKMM
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)
include_directories (${GTKMM_INCLUDE_DIRS})
link_directories (${GTKMM_LIBRARY_DIRS})

#build executable
add_executable (arma3-linux-launcher main.cpp Filesystem.cpp Logger.cpp MainWindow.cpp Mod.cpp Settings.cpp Utils.cpp VDF.cpp VDFKey.cpp)
target_link_libraries (arma3-linux-launcher ${GTKMM_LIBRARIES} pthread)

# Installation
install (TARGETS arma3-linux-launcher DESTINATION /usr/bin)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.desktop DESTINATION /usr/share/applications)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/MainForm.glade DESTINATION /usr/share/arma3-linux-launcher)
