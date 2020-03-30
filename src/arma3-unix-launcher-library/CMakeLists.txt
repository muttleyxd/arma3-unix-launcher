file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(arma3-unix-launcher-library STATIC ${SOURCES})
target_include_directories(arma3-unix-launcher-library INTERFACE .)
target_link_libraries(arma3-unix-launcher-library PRIVATE common fmt::fmt nlohmann::json)
