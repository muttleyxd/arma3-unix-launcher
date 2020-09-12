file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(common STATIC ${SOURCES})
target_include_directories(common INTERFACE .)
target_link_libraries(common PRIVATE fmt::fmt-header-only)

if (UNIX AND NOT APPLE)
    target_link_libraries(common INTERFACE ${CMAKE_DL_LIBS})
endif()
