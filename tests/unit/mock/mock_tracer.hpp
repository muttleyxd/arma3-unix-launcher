#pragma once

#include <iostream>
#include <trompeloeil.hpp>

static inline trompeloeil::stream_tracer tracer{std::cout};
