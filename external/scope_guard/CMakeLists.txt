add_library(scope_guard INTERFACE)
target_include_directories(scope_guard INTERFACE .)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(scope_guard INTERFACE "-Wno-template-id-cdtor")
endif()

add_library(rihab::scope_guard ALIAS scope_guard)
