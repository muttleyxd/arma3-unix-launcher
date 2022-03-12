set(SOURCES steam_integration_impl.cpp
            steam_integration_impl.hpp
            steam_integration_stub.cpp
            steam_integration_stub.hpp


            exceptions/steam_api_failed.cpp
            exceptions/steam_api_failed.hpp
            exceptions/steam_api_not_initialized.cpp
            exceptions/steam_api_not_initialized.hpp
            )

add_library(steam-integration STATIC ${SOURCES})
target_include_directories(steam-integration INTERFACE .)
target_link_libraries(steam-integration PRIVATE common fmt::fmt-header-only steamworks::sdk spdlog::spdlog)
