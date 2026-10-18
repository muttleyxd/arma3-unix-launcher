#pragma once

#include <filesystem>
#include <functional>
#include <optional>

namespace CaBundle
{
    using ExistsPredicate = std::function<bool(std::filesystem::path const &)>;

    // CA certificate bundle for HTTPS requests: ssl_cert_file (the SSL_CERT_FILE environment variable)
    // if it exists, otherwise the first existing well known location (Linux distributions, macOS).
    // Returns std::nullopt if there is none.
    std::optional<std::filesystem::path> Find(ExistsPredicate const &exists, char const *const ssl_cert_file);

    // Find() using the real filesystem and environment
    std::optional<std::filesystem::path> Find();
}
