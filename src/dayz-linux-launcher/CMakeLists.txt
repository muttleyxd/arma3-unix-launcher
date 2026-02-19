set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/dayz-linux-launcher/lib")

file(GLOB_RECURSE SOURCES *.cpp *.h *.hpp *.qrc *.ui)

qt_add_executable(dayz-linux-launcher ${SOURCES})
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
    Qt6::Widgets
    Qt6::Svg
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
