file(GLOB SOURCES *.cpp *.hpp ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/include/exceptions/*.hpp)
add_library(arma3-unix-launcher-library STATIC ${SOURCES})
target_include_directories(arma3-unix-launcher-library PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(arma3-unix-launcher-library stdc++fs fmt)
