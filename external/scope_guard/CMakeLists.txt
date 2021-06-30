add_library(scope_guard INTERFACE)
target_include_directories(scope_guard INTERFACE .)

add_library(rihab::scope_guard ALIAS scope_guard)
