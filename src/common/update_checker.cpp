#include "update_checker.hpp"

#include <sstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "filesystem_utils.hpp"

#ifndef REPOSITORY_VERSION
    #define REPOSITORY_VERSION 0
#endif

namespace
{
    std::filesystem::path get_ca_bundle_path()
    {
        if (FilesystemUtils::Exists("/etc/ssl/certs/ca-certificates.crt"))
            return "/etc/ssl/certs/ca-certificates.crt";
        if (FilesystemUtils::Exists("/etc/ssl/certs/ca-bundle.crt"))
            return "/etc/ssl/certs/ca-bundle.crt";
        throw std::runtime_error("not able to find ca bundle file");
    }
}

namespace UpdateChecker
{
    std::thread is_update_available(std::function<void(bool, std::string)> &&callback)
    {
        return std::thread([callback = std::move(callback)]()
        {
            if (REPOSITORY_VERSION == 0)
            {
                callback(false, "");
                return;
            }

            try
            {
                curlpp::Cleanup curl_cleanup;
                curlpp::Easy curl_request;
                curl_request.setOpt<curlpp::options::CaInfo>(get_ca_bundle_path().string());
                curl_request.setOpt<curlpp::options::Url>("https://api.github.com/repos/muttleyxd/arma3-unix-launcher/releases");
                curl_request.setOpt<curlpp::options::UserAgent>("curl/12.3.4.5");

                std::ostringstream output_stream;
                curlpp::options::WriteStream write_stream(&output_stream);
                curl_request.setOpt(write_stream);
                curl_request.perform();

                auto const response_body = output_stream.str();
                auto const response = nlohmann::json::parse(response_body);
                if (!response.is_array())
                    throw std::runtime_error("Response is not JSON array");

                auto const tag_name = response.at(0).at("tag_name");
                auto const current_tag_name = fmt::format("commit-{}", REPOSITORY_VERSION);
                spdlog::trace("{}:{} newest tag name: '{}', current tag name: '{}'", __PRETTY_FUNCTION__, __LINE__, tag_name,
                              current_tag_name);

                auto const content = response.at(0).at("body");

                callback(tag_name != current_tag_name, content);
                return;
            }
            catch (std::exception const &e)
            {
                spdlog::warn("{}:{} Failed to check for updates: {}\n", __PRETTY_FUNCTION__, __LINE__, e.what());
                callback(false, "");
                return;
            }
        });
    }
}
