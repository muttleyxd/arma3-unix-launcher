#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fmt/format.h>

#include "default_test_reporter.hpp"

#include "string_utils.hpp"

TEST_CASE("RemoveElementsFromPath")
{
    using namespace StringUtils;
    std::string const path = "/mnt/folder1/folder2/folder3";
    CHECK_EQ(RemoveElementsFromPath(path), "/mnt/folder1/folder2");
    CHECK_EQ(RemoveElementsFromPath(path, false), "/mnt/folder1/folder2/");
    CHECK_EQ(RemoveElementsFromPath(path, true, 2), "/mnt/folder1");
    CHECK_EQ(RemoveElementsFromPath(path, false, 2), "/mnt/folder1/");
    CHECK_EQ(RemoveElementsFromPath(path, false, 4), "/");
    CHECK_EQ(RemoveElementsFromPath(path, true, 4), "");

    CHECK_EQ(RemoveElementsFromPath(path, false, 32), "/");
    CHECK_EQ(RemoveElementsFromPath(path, true, 32), "");

    CHECK_EQ(RemoveElementsFromPath("/"), "");
    CHECK_EQ(RemoveElementsFromPath("thisisnotapath"), "thisisnotapath");

    CHECK_EQ(RemoveElementsFromPath(""), "");
}

TEST_CASE("Replace")
{
    using namespace StringUtils;
    CHECK_EQ(Replace("aaaa", "aa", "a"), "aa");
    CHECK_EQ(Replace("aaba", "aa", "a"), "aba");
    CHECK_EQ(Replace("aaaaa", "aa", "a"), "aaa");
    CHECK_EQ(Replace("this is a text", " a", ""), "this is text");

    CHECK_EQ(Replace("/dir1/dir2/", "/", "\\"), "\\dir1\\dir2\\");

    // Shouldn't be able to find empty string
    CHECK_EQ(Replace("testing", "", "test"), "testing");

    CHECK_EQ(Replace("tests", "te", "ab"), "absts");
    CHECK_EQ(Replace("tests", "testss", "ab"), "tests");
    CHECK_EQ(Replace("tests", "tests", "ab"), "ab");

    CHECK_EQ(Replace("first line\nsecond line\nthird line", "line", "l"), "first l\nsecond l\nthird l");
    CHECK_EQ(Replace("whole\tstring", "\t", "\n"), "whole\nstring");
}

TEST_CASE("Trim")
{
    using namespace StringUtils;
    const std::string to_trim = "\n\n\n\n\n\n    test string\t\n     \t\t  ";
    const std::string trimmed_left = "test string\t\n     \t\t  ";
    const std::string trimmed_right = "\n\n\n\n\n\n    test string";
    const std::string trimmed = "test string";

    CHECK_EQ(trim(to_trim), trimmed);
    CHECK_EQ(trim_left(to_trim), trimmed_left);
    CHECK_EQ(trim_right(to_trim), trimmed_right);

    CHECK_EQ(trim(trimmed), trimmed);

    auto trim_with_copy = trim(to_trim);
    CHECK_EQ(trim_with_copy, trimmed);

    std::string_view trim_with_view_argument(to_trim);
    auto trimmed_view = trim(trim_with_view_argument);
    CHECK_EQ(trimmed_view, trimmed);
}

TEST_CASE("StartsWith")
{
    using namespace StringUtils;
    std::string const text = "This is some text";

    CHECK(StartsWith(text, "This"));
    CHECK(StartsWith(text, "This "));
    CHECK(StartsWith(text, "This is some text"));

    CHECK_FALSE(StartsWith(text, "This is some text "));
    CHECK_FALSE(StartsWith(text, "this"));
    CHECK_FALSE(StartsWith(text, "\nThis"));
    CHECK_FALSE(StartsWith(text, " This"));
    CHECK_FALSE(StartsWith(text, "text"));
}

TEST_CASE("EndsWith")
{
    using namespace StringUtils;
    std::string const text = "This is some text";

    CHECK(EndsWith(text, "text"));
    CHECK(EndsWith(text, " text"));
    CHECK(EndsWith(text, "This is some text"));
    CHECK_FALSE(EndsWith(text, "this is some text"));
    CHECK_FALSE(EndsWith(text, " This is some text"));
    CHECK_FALSE(EndsWith(text, "\ntext"));
}

TEST_CASE("Split")
{
    using namespace StringUtils;
    std::string const text = "This is some text";

    std::vector<std::string_view> text_split_by_space
    {
        std::string_view(text.c_str(), 4),
        std::string_view(text.c_str() + 5, 2),
        std::string_view(text.c_str() + 8, 4),
        std::string_view(text.c_str() + 13, 4)
    };

    CHECK_EQ(text_split_by_space, Split(text, " "));

    std::vector<std::string_view> text_split_by_t
    {
        std::string_view(text.c_str() + 1, 12),
        std::string_view(text.c_str() + 14, 2),
    };

    CHECK_EQ(text_split_by_t, Split(text, "Tt"));

    CHECK_EQ(std::vector<std::string_view>(), Split("  ", " "));

    std::string const remove_stamina_text = "name=\"Remove Stamina\"";
    std::vector<std::string_view> remove_stamina
    {
        std::string_view(remove_stamina_text.c_str(), 4),
        std::string_view(remove_stamina_text.c_str() + 5, 16)
    };
    CHECK_EQ(remove_stamina, Split(remove_stamina_text, "="));
}

TEST_CASE("ToWindowsPath")
{
    using namespace std::filesystem;
    using namespace StringUtils;

    SUBCASE("Relative path")
    {
        path linux_path{"dir1/dir2/file.cpp"};
        path windows_path{R"(dir1\dir2\file.cpp)"};
        CHECK_EQ(windows_path, ToWindowsPath(linux_path));
    }

    SUBCASE("Absolute path")
    {
        path linux_path{"/dir1/dir2/file.cpp"};

        SUBCASE("Default drive")
        {
            path windows_path{R"(C:\dir1\dir2\file.cpp)"};
            CHECK_EQ(windows_path, ToWindowsPath(linux_path));
        }

        SUBCASE("Custom drive")
        {
            path windows_path{R"(Z:\dir1\dir2\file.cpp)"};
            CHECK_EQ(windows_path, ToWindowsPath(linux_path, 'Z'));
        }
    }
}
