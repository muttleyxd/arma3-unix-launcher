file(GLOB SOURCES *.cpp *.hpp ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/include/exceptions/*.hpp)

function(add_a3ul_library lib_name)
    add_library(${lib_name} SHARED ${SOURCES})
    target_include_directories(${lib_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(${lib_name} stdc++fs fmt)
endfunction()

add_a3ul_library(arma3-unix-launcher-library)
target_compile_definitions(arma3-unix-launcher-library PUBLIC -DDOCTEST_CONFIG_DISABLE)

if (RUN_TESTS)
    add_a3ul_library(arma3-unix-launcher-library-testing)    
endif ()