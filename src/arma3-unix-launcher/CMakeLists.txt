set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
if (NOT APPLE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/arma3-unix-launcher/lib" PARENT_SCOPE)
endif()

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Svg CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES *.cpp *.h *.hpp *.qrc *.ui)

add_executable(arma3-unix-launcher MACOSX_BUNDLE ${SOURCES})
target_compile_definitions(arma3-unix-launcher PRIVATE "-DREPOSITORY_VERSION=${COMMIT_COUNT}")
target_include_directories(arma3-unix-launcher PRIVATE .)
target_link_libraries(arma3-unix-launcher PRIVATE
    arma3-unix-launcher-library
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
install(TARGETS arma3-unix-launcher
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION bin COMPONENT Runtime)

if (NOT APPLE)
    install(FILES blagoicons/arma3.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME arma3-unix-launcher.png)
    install(FILES arma3-unix-launcher.desktop
            DESTINATION share/applications)
endif()

if (APPLE)
    find_package(Qt5Widgets CONFIG REQUIRED)
    get_target_property(WIDGETS_LOCATION Qt5::Widgets IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${WIDGETS_LOCATION}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT_BIN_DIR}")
    
    add_custom_command(TARGET arma3-unix-launcher POST_BUILD
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE_DIR:arma3-unix-launcher>/../.."
            -always-overwrite
        COMMENT "Running macdeployqt...")

    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)

    set(MACOSX_BUNDLE_ICON_FILE arma3-unix-launcher.icns)
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/blagoicons/arma3-unix-launcher.icns)
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(arma3-unix-launcher PRIVATE ${ICON_FILE})

    set_target_properties(arma3-unix-launcher PROPERTIES OUTPUT_NAME "Arma 3 Unix Launcher")

    add_custom_command(TARGET arma3-unix-launcher POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy "${STEAMWORKS_LIB_PATH}" "$<TARGET_FILE_DIR:arma3-unix-launcher>/libsteam_api.dylib"
                       COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/external/steamworks-LICENSE.txt" "$<TARGET_FILE_DIR:arma3-unix-launcher>/steamworks-LICENSE.txt"
                       COMMENT "Copying Steamworks SDK")
else()
    install(FILES "${STEAMWORKS_LIB_PATH}"
                  "${CMAKE_SOURCE_DIR}/external/steamworks-LICENSE.txt"
            DESTINATION share/arma3-unix-launcher/lib)
endif()
