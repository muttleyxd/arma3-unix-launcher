enable_testing()

file(GLOB TEST_SRC *.cpp)

foreach(test ${TEST_SRC})
	get_filename_component(binary_name ${test} NAME_WE)
	add_executable(${binary_name} ${test})
        target_include_directories(${binary_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
        target_link_libraries(${binary_name} ${GTEST_LIBRARIES} arma3-unix-launcher-library)
	add_test(NAME ${binary_name} COMMAND true)
endforeach()
