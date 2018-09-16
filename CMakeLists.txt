cmake_minimum_required(VERSION 3.8.2)
project(arma3-unix-launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter")

option(RUN_TESTS "Run tests with GoogleTest" ON)

add_subdirectory(src)

if (RUN_TESTS)
	add_subdirectory(tests)
endif ()
