#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <random>

#include "default_test_reporter.hpp"

#include "http_client.hpp"

// The client is exercised through file:// URLs, which libcurl handles like any other transfer,
// so no network access is needed

namespace
{
    struct TemporaryDirectory
    {
        TemporaryDirectory()
        {
            path = std::filesystem::temp_directory_path() / ("a3ul-http-test-" + std::to_string(std::random_device()()));
            std::filesystem::create_directories(path);
        }
        ~TemporaryDirectory()
        {
            std::error_code ignored;
            std::filesystem::remove_all(path, ignored);
        }
        std::filesystem::path path;
    };

    Http::CurlHttpClient file_client(std::chrono::seconds timeout = std::chrono::seconds(5),
                                     std::size_t max_response_bytes = 1024 * 1024)
    {
        return Http::CurlHttpClient(std::nullopt, timeout, max_response_bytes, "file");
    }

    std::string file_url(std::filesystem::path const &path)
    {
        return "file://" + path.string();
    }

    void write_file(std::filesystem::path const &path, std::string const &content)
    {
        std::ofstream(path, std::ios::binary) << content;
    }
}

TEST_CASE("Get_ReturnsBody")
{
    TemporaryDirectory directory;
    std::string const binary_content("\x89PNG\r\n\x1a\n\0\0data", 14);
    write_file(directory.path / "image.png", binary_content);

    auto client = file_client();
    CHECK_EQ(client.Get(file_url(directory.path / "image.png")), binary_content);
}

TEST_CASE("Get_EmptyBody")
{
    TemporaryDirectory directory;
    write_file(directory.path / "empty", "");

    auto client = file_client();
    CHECK_EQ(client.Get(file_url(directory.path / "empty")), std::string());
}

TEST_CASE("Get_Failures_ReturnNullopt")
{
    TemporaryDirectory directory;
    auto client = file_client();

    CHECK_FALSE(client.Get(file_url(directory.path / "missing")).has_value());
    CHECK_FALSE(client.Get("").has_value());
    CHECK_FALSE(client.Get("not a url").has_value());
    CHECK_FALSE(client.Get("unsupported-scheme://example.com/").has_value());
}

TEST_CASE("Get_ResponseOverSizeLimit_ReturnsNullopt")
{
    TemporaryDirectory directory;
    write_file(directory.path / "small", std::string(10, 'a'));
    write_file(directory.path / "large", std::string(11, 'a'));

    auto client = file_client(std::chrono::seconds(5), 10);
    CHECK_EQ(client.Get(file_url(directory.path / "small")), std::string(10, 'a'));
    CHECK_FALSE(client.Get(file_url(directory.path / "large")).has_value());
}

TEST_CASE("DefaultProtocols_RefuseLocalFiles")
{
    TemporaryDirectory directory;
    write_file(directory.path / "secret", "do not read");

    Http::CurlHttpClient client(std::nullopt);
    CHECK_FALSE(client.Get(file_url(directory.path / "secret")).has_value());
}

TEST_CASE("PostForm_Failures_ReturnNullopt")
{
    Http::CurlHttpClient client(std::nullopt);
    CHECK_FALSE(client.PostForm("", "a=1").has_value());
    CHECK_FALSE(client.PostForm("unsupported-scheme://example.com/", "a=1").has_value());
}

TEST_CASE("UnreachableHttpsServer_ReturnsNullopt")
{
    // Nothing listens on the discard port; connection errors must not throw or crash
    Http::CurlHttpClient client(std::filesystem::path("/nonexistent/ca-bundle.pem"), std::chrono::seconds(1));
    CHECK_FALSE(client.Get("https://127.0.0.1:9/").has_value());
}
