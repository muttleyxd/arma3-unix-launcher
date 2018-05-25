#include "gtest/gtest.h"

#include "string_utils.hpp"

#include <string_view>
#include <vector>

TEST(StringUtilsTests, RemoveElementsFromPath)
{
    using namespace StringUtils;
    const std::string path = "/mnt/folder1/folder2/folder3";
    ASSERT_EQ(RemoveElementsFromPath(path), "/mnt/folder1/folder2");
    ASSERT_EQ(RemoveElementsFromPath(path, false), "/mnt/folder1/folder2/");
    ASSERT_EQ(RemoveElementsFromPath(path, true, 2), "/mnt/folder1");
    ASSERT_EQ(RemoveElementsFromPath(path, false, 2), "/mnt/folder1/");
    ASSERT_EQ(RemoveElementsFromPath(path, false, 4), "/");
    ASSERT_EQ(RemoveElementsFromPath(path, true, 4), "");

    ASSERT_EQ(RemoveElementsFromPath(path, false, 32), "/");
    ASSERT_EQ(RemoveElementsFromPath(path, true, 32), "");

    ASSERT_EQ(RemoveElementsFromPath("/"), "");
    ASSERT_EQ(RemoveElementsFromPath("thisisnotapath"), "thisisnotapath");
}

TEST(StringUtilsTests, Replace)
{
    using namespace StringUtils;
    ASSERT_EQ(Replace("aaaa", "aa", "a"), "aa");
    ASSERT_EQ(Replace("aaba", "aa", "a"), "aba");
    ASSERT_EQ(Replace("aaaaa", "aa", "a"), "aaa");
    ASSERT_EQ(Replace("this is a text", " a", ""), "this is text");

    // Shouldn't be able to find empty string
    ASSERT_EQ(Replace("testing", "", "test"), "testing");

    ASSERT_EQ(Replace("tests", "te", "ab"), "absts");
    ASSERT_EQ(Replace("tests", "testss", "ab"), "tests");
    ASSERT_EQ(Replace("tests", "tests", "ab"), "ab");

    ASSERT_EQ(Replace("first line\nsecond line\nthird line", "line", "l"), "first l\nsecond l\nthird l");
    ASSERT_EQ(Replace("whole\tstring", "\t", "\n"), "whole\nstring");
}

TEST(StringUtilsTests, Trim)
{
    using namespace StringUtils;
    const std::string to_trim = "\n\n\n\n\n\n    test string\t\n     \t\t  ";
    const std::string trimmed_left = "test string\t\n     \t\t  ";
    const std::string trimmed_right = "\n\n\n\n\n\n    test string";
    const std::string trimmed = "test string";

    ASSERT_EQ(Trim(to_trim), trimmed);
    ASSERT_EQ(TrimLeft(to_trim), trimmed_left);
    ASSERT_EQ(TrimRight(to_trim), trimmed_right);

    ASSERT_EQ(Trim(trimmed), trimmed);

    std::string trim_with_copy = Trim<std::string, std::string>(to_trim);
    ASSERT_EQ(trim_with_copy, trimmed);

    std::string_view trim_with_view_argument(to_trim);
    std::string_view trimmed_view = Trim<std::string_view, std::string_view>(trim_with_view_argument);
    ASSERT_EQ(trimmed_view, trimmed);
}

TEST(StringUtilsTests, StartsWith)
{
    using namespace StringUtils;
    const std::string text = "This is some text";

    ASSERT_TRUE(StartsWith(text, "This"));
    ASSERT_TRUE(StartsWith(text, "This "));
    ASSERT_TRUE(StartsWith(text, "This is some text"));

    ASSERT_FALSE(StartsWith(text, "This is some text "));
    ASSERT_FALSE(StartsWith(text, "this"));
    ASSERT_FALSE(StartsWith(text, "\nThis"));
    ASSERT_FALSE(StartsWith(text, " This"));
    ASSERT_FALSE(StartsWith(text, "text"));
}

TEST(StringUtilsTests, EndsWith)
{
    using namespace StringUtils;
    const std::string text = "This is some text";

    ASSERT_TRUE(EndsWith(text, "text"));
    ASSERT_TRUE(EndsWith(text, " text"));
    ASSERT_TRUE(EndsWith(text, "This is some text"));
    ASSERT_FALSE(EndsWith(text, "this is some text"));
    ASSERT_FALSE(EndsWith(text, "\ntext"));
}

TEST(StringUtilsTests, Split)
{
    using namespace StringUtils;
    const std::string text = "This is some text";

    std::vector<std::string_view> text_split_by_space
    {
        std::string_view(text.c_str(), 4),
        std::string_view(text.c_str() + 5, 2),
        std::string_view(text.c_str() + 8, 4),
        std::string_view(text.c_str() + 13, 4)
    };

    ASSERT_EQ(Split(text, " "), text_split_by_space);

    std::vector<std::string_view> text_split_by_t
    {
        std::string_view(text.c_str() + 1, 12),
        std::string_view(text.c_str() + 14, 2),
    };

    ASSERT_EQ(Split(text, "Tt"), text_split_by_t);

    ASSERT_EQ(Split("  ", " "), std::vector<std::string_view>());
}
