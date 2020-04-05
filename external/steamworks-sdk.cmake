set(STEAMWORKS_SDK_DETECTED FALSE)

if (NOT STEAMWORKS_SDK_PATH)
    return()
endif()

if (NOT (IS_DIRECTORY "${STEAMWORKS_SDK_PATH}/sdk/public/steam" AND EXISTS "${STEAMWORKS_SDK_PATH}/sdk/redistributable_bin/linux64/libsteam_api.so"))
    message(FATAL_ERROR "Incorrect path provided, ${STEAMWORKS_SDK_PATH}/sdk/public/steam and ${STEAMWORKS_SDK_PATH}/sdk/redistributable_bin/linux64/libsteam_api.so should exist")
endif()

set(STEAMWORKS_SDK_DETECTED TRUE)

add_library(steamworks_sdk SHARED IMPORTED GLOBAL)

if (APPLE)
    # todo: set correct rpath
    set(STEAMWORKS_LIB_PATH "${STEAMWORKS_SDK_PATH}/sdk/redistributable_bin/osx/libsteam_api.dylib")
else()
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/arma3-unix-launcher/lib")
    set(STEAMWORKS_LIB_PATH "${STEAMWORKS_SDK_PATH}/sdk/redistributable_bin/linux64/libsteam_api.so")
endif()
set_property(TARGET steamworks_sdk PROPERTY IMPORTED_LOCATION "${STEAMWORKS_LIB_PATH}")
target_include_directories(steamworks_sdk INTERFACE  "${STEAMWORKS_SDK_PATH}/sdk/public/steam")
add_library(steamworks::sdk ALIAS steamworks_sdk)
