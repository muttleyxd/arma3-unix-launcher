file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(common STATIC ${SOURCES})
target_compile_definitions(common PRIVATE "-DREPOSITORY_VERSION=${COMMIT_COUNT}")
target_include_directories(common INTERFACE .)
target_compile_options(common PRIVATE -fpermissive)
target_link_libraries(common PRIVATE curlpp::curlpp curl fmt::fmt-header-only nlohmann::json spdlog::spdlog)

if (UNIX AND NOT APPLE)
    target_link_libraries(common INTERFACE ${CMAKE_DL_LIBS})
endif()
