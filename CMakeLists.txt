cmake_minimum_required(VERSION 3.8.2)
project(arma3-unix-launcher)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST gtest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter")

add_subdirectory(src)

if (GTEST_FOUND)
	add_subdirectory(tests)
endif ()
