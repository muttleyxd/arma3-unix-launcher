#include "http_client.hpp"

#include <memory>
#include <mutex>

#include <curl/curl.h>
#include <spdlog/spdlog.h>

namespace
{
    struct ResponseBuffer
    {
        std::string body;
        std::size_t max_bytes;
        bool too_large = false;
    };

    size_t write_callback(char *data, size_t size, size_t count, void *user_data)
    {
        auto &buffer = *static_cast<ResponseBuffer *>(user_data);
        size_t const bytes = size * count;
        if (buffer.body.size() + bytes > buffer.max_bytes)
        {
            buffer.too_large = true;
            return 0; // makes libcurl abort the transfer with CURLE_WRITE_ERROR
        }
        buffer.body.append(data, bytes);
        return bytes;
    }

    void initialize_curl_once()
    {
        static std::once_flag initialized;
        std::call_once(initialized, []()
        {
            curl_global_init(CURL_GLOBAL_DEFAULT);
        });
    }
}

namespace Http
{
    CurlHttpClient::CurlHttpClient(std::optional<std::filesystem::path> ca_bundle, std::chrono::seconds timeout,
                                   std::size_t max_response_bytes, std::string allowed_protocols)
        : ca_bundle_(std::move(ca_bundle))
        , timeout_(timeout)
        , max_response_bytes_(max_response_bytes)
        , allowed_protocols_(std::move(allowed_protocols))
    {
    }

    std::optional<std::string> CurlHttpClient::Get(std::string const &url)
    {
        return Perform(url, nullptr);
    }

    std::optional<std::string> CurlHttpClient::PostForm(std::string const &url, std::string const &form_body)
    {
        return Perform(url, &form_body);
    }

    std::optional<std::string> CurlHttpClient::Perform(std::string const &url, std::string const *form_body)
    {
        initialize_curl_once();
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
        if (!curl)
            return std::nullopt;

        ResponseBuffer buffer{std::string(), max_response_bytes_};
        auto handle = curl.get();
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_PROTOCOLS_STR, allowed_protocols_.c_str());
        curl_easy_setopt(handle, CURLOPT_REDIR_PROTOCOLS_STR, allowed_protocols_.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L); // requests run on worker threads
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, static_cast<long>(timeout_.count()));
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "arma3-unix-launcher");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
        if (ca_bundle_)
            curl_easy_setopt(handle, CURLOPT_CAINFO, ca_bundle_->c_str());
        if (form_body)
        {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, form_body->c_str());
            curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(form_body->size()));
        }

        CURLcode const result = curl_easy_perform(handle);
        if (result != CURLE_OK)
        {
            if (buffer.too_large)
                spdlog::debug("HTTP request to '{}' aborted, response larger than {} bytes", url, max_response_bytes_);
            else
                spdlog::debug("HTTP request to '{}' failed: {}", url, curl_easy_strerror(result));
            return std::nullopt;
        }
        return std::move(buffer.body);
    }
}
