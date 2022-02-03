file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(dayz-linux-launcher-library STATIC ${SOURCES})
target_include_directories(dayz-linux-launcher-library INTERFACE .)
target_link_libraries(dayz-linux-launcher-library PRIVATE arma3-unix-launcher-library common fmt::fmt-header-only nlohmann::json spdlog::spdlog)
