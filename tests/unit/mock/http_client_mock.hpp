#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include "http_client.hpp"

class HttpClientMock : public Http::IHttpClient
{
    public:
        MAKE_MOCK1(Get, std::optional<std::string>(std::string const &), override);
        MAKE_MOCK2(PostForm, std::optional<std::string>(std::string const &, std::string const &), override);
};
