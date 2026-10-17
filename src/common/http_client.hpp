#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>

namespace Http
{
    // Minimal HTTP client interface, so code doing requests can be unit tested with a mock.
    // Implementations never throw: every failure (network, TLS, HTTP status >= 400, size limit) is
    // reported as std::nullopt.
    class IHttpClient
    {
        public:
            virtual ~IHttpClient() = default;

            // Response body of a GET request
            virtual std::optional<std::string> Get(std::string const &url) = 0;

            // Response body of a POST request with an application/x-www-form-urlencoded body
            virtual std::optional<std::string> PostForm(std::string const &url, std::string const &form_body) = 0;
    };

    // libcurl based client. Uses the libcurl C API directly instead of curlpp: curlpp reports errors
    // with exceptions, and throwing is unsafe while libsteam_api (which overrides __cxa_throw) is loaded.
    class CurlHttpClient : public IHttpClient
    {
        public:
            // allowed_protocols is a libcurl protocol list (CURLOPT_PROTOCOLS_STR), also applied to redirects.
            // URLs come from Web API responses, so local schemes such as file are off by default.
            CurlHttpClient(std::optional<std::filesystem::path> ca_bundle,
                           std::chrono::seconds timeout = std::chrono::seconds(20),
                           std::size_t max_response_bytes = 16 * 1024 * 1024,
                           std::string allowed_protocols = "http,https");

            std::optional<std::string> Get(std::string const &url) override;
            std::optional<std::string> PostForm(std::string const &url, std::string const &form_body) override;

        private:
            std::optional<std::string> Perform(std::string const &url, std::string const *form_body);

            std::optional<std::filesystem::path> ca_bundle_;
            std::chrono::seconds timeout_;
            std::size_t max_response_bytes_;
            std::string allowed_protocols_;
    };
}
