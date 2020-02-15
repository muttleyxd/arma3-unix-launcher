enable_testing()

option(TRACE_MOCKS "Enable mock tracing (logs every call to mock object to stdout)" OFF)
if (TRACE_MOCKS)
    string(APPEND CMAKE_CXX_FLAGS " -include ${CMAKE_CURRENT_SOURCE_DIR}/mock/mock_tracer.hpp")
endif()

add_subdirectory(arma3-unix-launcher-library)
