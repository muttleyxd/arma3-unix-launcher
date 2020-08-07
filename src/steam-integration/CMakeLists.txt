set(SOURCES steam_integration.cpp
            steam_integration.hpp

            exceptions/steam_api_failed.cpp
            exceptions/steam_api_failed.hpp
            exceptions/steam_api_not_initialized.cpp
            exceptions/steam_api_not_initialized.hpp
            )

set(SOURCES_STUB steam_integration_stub.cpp
                 exceptions/steam_api_not_initialized.cpp
                 exceptions/steam_api_not_initialized.hpp
                 )

add_library(steam-integration STATIC)
target_include_directories(steam-integration INTERFACE .)

if (STEAMWORKS_SDK_DETECTED)
    target_sources(steam-integration PRIVATE ${SOURCES})
    target_link_libraries(steam-integration PRIVATE common fmt::fmt steamworks::sdk)
else()
    target_sources(steam-integration PRIVATE ${SOURCES_STUB})
endif()
