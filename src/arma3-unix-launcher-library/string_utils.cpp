#include "string_utils.hpp"

#include <algorithm>
#include <iostream>

namespace StringUtils
{
    size_t find_last_nth(std::string const &text, char const c, int count = 1)
    {
        ssize_t found_chars = 0;
        ssize_t found_i = 0;
        for (ssize_t i = text.size() - 1; i >= 0; i--)
        {
            if (text[i] == c)
            {
                found_chars++;
                found_i = i;
            }
            if (found_chars == count)
                return found_i;
        }
        if (found_chars > 0)
            return found_i;
        return std::string::npos;
    }

    std::string_view RemoveElementsFromPath(std::string const &text, bool remove_slash, int count)
    {
        if (text.length() == 0)
            return std::string_view(text.c_str(), text.size());

        size_t pos = find_last_nth(text, '/', count);
        if (pos == std::string::npos)
            return std::string_view(text.c_str(), text.size());
        if (!remove_slash)
            pos++;
        return std::string_view(text.c_str(), pos);
    }

    std::string Replace(std::string text, std::string const &from, std::string const &to)
    {
        if (from.empty())
            return text;

        size_t start_pos = 0;
        while (start_pos != std::string::npos)
        {
            start_pos = text.find(from, start_pos);
            if (start_pos == std::string::npos)
                return text;
            text.replace(start_pos, from.length(), to);
            //move start_pos forward so it doesn't replace the same string over and over
            start_pos += to.length() - from.length() + 2;
        }
        return text;
    }

    bool EndsWith(std::string const &text, std::string const &find)
    {
        if (find.size() > text.size())
            return false;
        return find == std::string_view(text.c_str() + text.size() - find.size(), find.size());
    }

    bool StartsWith(std::string const &text, std::string const &find)
    {
        if (find.size() > text.size())
            return false;
        return find == std::string_view(text.c_str(), find.size());
    }

    std::vector<std::string_view> Split(std::string const &text_to_split, std::string const &delimiters)
    {
        std::vector<std::string_view> ret;

        size_t start_trim_size = text_to_split.find_first_not_of(delimiters);
        size_t end_trim_size = text_to_split.find_last_not_of(delimiters);
        if (start_trim_size == std::string::npos || end_trim_size == std::string::npos)
            return ret;
        std::string_view trimmed(text_to_split.c_str() + start_trim_size, end_trim_size - start_trim_size + 1);

        size_t start_pos = 0;
        size_t end_pos = std::string::npos;
        while (true)
        {
            start_pos = trimmed.find_first_not_of(delimiters, start_pos);
            end_pos = trimmed.find_first_of(delimiters, start_pos);
            if (end_pos == std::string::npos)
            {
                ret.push_back(trimmed.substr(start_pos, trimmed.size() - start_pos));
                break;
            }
            ret.push_back(trimmed.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos;
        }

        return ret;
    }

    std::string Lowercase(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        return text;
    }

    std::filesystem::path ToWindowsPath(std::filesystem::path const &path)
    {
        if (path.empty())
            return path;
        std::string path_str = Replace(path.c_str(), "/", "\\");
        if (path.is_absolute())
            return "C:" + path_str;
        return std::move(path_str);
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>

TEST_SUITE_BEGIN("StringUtils");

TEST_CASE("RemoveElementsFromPath")
{
    using namespace StringUtils;
    const std::string path = "/mnt/folder1/folder2/folder3";
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

    CHECK_EQ(Trim(to_trim), trimmed);
    CHECK_EQ(TrimLeft(to_trim), trimmed_left);
    CHECK_EQ(TrimRight(to_trim), trimmed_right);

    CHECK_EQ(Trim(trimmed), trimmed);

    auto trim_with_copy = Trim<std::string, std::string>(to_trim);
    CHECK_EQ(trim_with_copy, trimmed);

    std::string_view trim_with_view_argument(to_trim);
    auto trimmed_view = Trim<std::string_view, std::string_view>(trim_with_view_argument);
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
        path windows_path{R"(C:\dir1\dir2\file.cpp)"};
        CHECK_EQ(windows_path, ToWindowsPath(linux_path));
    }
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
