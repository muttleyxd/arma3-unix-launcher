#include "update_checker.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#ifndef REPOSITORY_VERSION
#define REPOSITORY_VERSION 0
#endif

namespace UpdateChecker
{
    std::thread is_update_available(std::function<void(bool, std::string)>&& callback)
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
                httplib::Client cli("https://api.github.com");
                auto res = cli.Get("/repos/muttleyxd/arma3-unix-launcher/releases");
                if (res->status != 200)
                    throw std::runtime_error(fmt::format("HTTP request returned: {}\n{}", res->status, res->body));

                auto const response = nlohmann::json::parse(res->body);
                if (!response.is_array())
                    throw std::runtime_error("Response is not JSON array");

                auto const tag_name = response.at(0).at("tag_name");
                auto const current_tag_name = fmt::format("commit-{}", REPOSITORY_VERSION);
                spdlog::trace("{}:{} newest tag name: '{}', current tag name: '{}'", __PRETTY_FUNCTION__, __LINE__, tag_name, current_tag_name);

                auto const content = response.at(0).at("body");

                callback(tag_name != current_tag_name, content);
                return;
            }
            catch (std::exception const& e)
            {
                spdlog::warn("{}:{} Failed to check for updates: {}\n", __PRETTY_FUNCTION__, __LINE__, e.what());
                callback(false, "");
                return;
            }
        });
    }
}
