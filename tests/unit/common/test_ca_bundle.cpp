#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <set>

#include "default_test_reporter.hpp"

#include "ca_bundle.hpp"

namespace
{
    CaBundle::ExistsPredicate existing(std::set<std::filesystem::path> files)
    {
        return [files = std::move(files)](std::filesystem::path const & path)
        {
            return files.count(path) > 0;
        };
    }
}

TEST_CASE("Find_MacOS")
{
    CHECK_EQ(CaBundle::Find(existing({"/etc/ssl/cert.pem"}), nullptr), std::filesystem::path("/etc/ssl/cert.pem"));
}

TEST_CASE("Find_LinuxLocations")
{
    using CaBundle::Find;
    CHECK_EQ(Find(existing({"/etc/ssl/certs/ca-certificates.crt"}), nullptr),
             std::filesystem::path("/etc/ssl/certs/ca-certificates.crt"));
    CHECK_EQ(Find(existing({"/etc/ssl/certs/ca-bundle.crt"}), nullptr),
             std::filesystem::path("/etc/ssl/certs/ca-bundle.crt"));
}

TEST_CASE("Find_PrefersEarlierLocations")
{
    auto const all = existing({"/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/ca-bundle.crt", "/etc/ssl/cert.pem"});
    CHECK_EQ(CaBundle::Find(all, nullptr), std::filesystem::path("/etc/ssl/certs/ca-certificates.crt"));
}

TEST_CASE("Find_SslCertFileEnvironmentVariable")
{
    using CaBundle::Find;
    auto const files = existing({"/custom/bundle.pem", "/etc/ssl/cert.pem"});
    CHECK_EQ(Find(files, "/custom/bundle.pem"), std::filesystem::path("/custom/bundle.pem"));

    // Ignored when it does not exist or is empty
    CHECK_EQ(Find(files, "/missing/bundle.pem"), std::filesystem::path("/etc/ssl/cert.pem"));
    CHECK_EQ(Find(files, ""), std::filesystem::path("/etc/ssl/cert.pem"));
}

TEST_CASE("Find_NothingExists_ReturnsNullopt")
{
    CHECK_FALSE(CaBundle::Find(existing({}), nullptr).has_value());
    CHECK_FALSE(CaBundle::Find(existing({}), "/missing/bundle.pem").has_value());
}

TEST_CASE("Find_RealFilesystem_OnMacOS")
{
    // Every supported macOS ships /etc/ssl/cert.pem
    auto const found = CaBundle::Find();
    REQUIRE(found.has_value());
    CHECK(std::filesystem::exists(*found));
}
