#include "ca_bundle.hpp"

#include <array>
#include <cstdlib>
#include <string_view>
#include <system_error>

namespace
{
    constexpr std::array<char const *, 3> well_known_locations
    {
        "/etc/ssl/certs/ca-certificates.crt", // Debian, Ubuntu, Arch
        "/etc/ssl/certs/ca-bundle.crt",       // Fedora
        "/etc/ssl/cert.pem",                  // macOS
    };
}

namespace CaBundle
{
    std::optional<std::filesystem::path> Find(ExistsPredicate const &exists, char const *const ssl_cert_file)
    {
        if (ssl_cert_file && std::string_view(ssl_cert_file) != "" && exists(ssl_cert_file))
            return ssl_cert_file;

        for (auto const location : well_known_locations)
            if (exists(location))
                return location;
        return std::nullopt;
    }

    std::optional<std::filesystem::path> Find()
    {
        auto const exists = [](std::filesystem::path const & path)
        {
            std::error_code error;
            return std::filesystem::exists(path, error);
        };
        return Find(exists, std::getenv("SSL_CERT_FILE"));
    }
}
