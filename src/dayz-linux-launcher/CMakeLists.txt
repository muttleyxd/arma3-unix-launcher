set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/dayz-linux-launcher/lib")

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Svg CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES *.cpp *.h *.hpp *.qrc *.ui)

add_executable(dayz-linux-launcher ${SOURCES})
target_compile_definitions(dayz-linux-launcher PRIVATE "-DREPOSITORY_VERSION=${COMMIT_COUNT}")
target_include_directories(dayz-linux-launcher PRIVATE .)
target_link_libraries(dayz-linux-launcher PRIVATE
    arma3-unix-launcher-library
    dayz-linux-launcher-library
    common
    steam-integration
    argparse::argparse
    fmt::fmt-header-only
    nlohmann::json
    pugixml::pugixml
    rihab::scope_guard
    Qt5::Widgets
    Qt5::Svg
    spdlog::spdlog
    )
install(TARGETS dayz-linux-launcher
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION bin COMPONENT Runtime)
install(FILES blagoicons/dayz.png
        DESTINATION share/icons/hicolor/256x256/apps
        RENAME dayz-linux-launcher.png)
install(FILES dayz-linux-launcher.desktop
        DESTINATION share/applications)
install(FILES "${STEAMWORKS_LIB_PATH}"
              "${CMAKE_SOURCE_DIR}/external/steamworks-LICENSE.txt"
        DESTINATION share/dayz-linux-launcher/lib)
