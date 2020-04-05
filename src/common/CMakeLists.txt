file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(common STATIC ${SOURCES})
target_include_directories(common INTERFACE .)
target_link_libraries(common PRIVATE fmt::fmt)
