#include "std_utils.hpp"

#include <fmt/format.h>
#include <fstream>
#include <filesystem>

namespace StdUtils
{
    bool CreateFile(std::filesystem::path const &path)
    {
        return creat(path.c_str(), 0644);
    }

    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase)
    {
        std::vector<std::string> ret;
        for (const auto &entity : std::filesystem::directory_iterator(path))
        {
            if (set_lowercase)
                ret.emplace_back(StringUtils::Lowercase(entity.path().filename()));
            else
                ret.emplace_back(entity.path().filename());
        }
        return ret;
    }

    std::string FileReadAllText(std::filesystem::path const &path)
    {
        auto file_size = std::filesystem::file_size(path);
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(file_size + 1);

        std::ifstream file(path);
        std::stringstream str;
        str << file.rdbuf();
        return str.str();
    }

    void FileWriteAllText(std::filesystem::path const &path, const std::string &text)
    {
        if (!exists(path.parent_path()))
            throw std::filesystem::filesystem_error("Parent dir does not exist", {});
        std::ofstream(path, std::ios_base::trunc) << text;
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>
#include "tests.hpp"

TEST_SUITE_BEGIN("std_utils");

TEST_CASE("Contains")
{
    using namespace StdUtils;

    SUBCASE("integers")
    {
        std::vector<int> numbers{1, 2, 4, 5, 6};
        CHECK(Contains(numbers, 1));
        CHECK(Contains(numbers, 2));
        CHECK_FALSE(Contains(numbers, 3));
    }

    SUBCASE("strings")
    {
        std::vector<std::string> strings{"test", "123", "addons"};
        CHECK(Contains(strings, "test"));
        CHECK(Contains(strings, "addons"));
        CHECK_FALSE(Contains(strings, "ADDONS"));
    }
}

TEST_CASE("ContainsKey")
{
    using namespace StdUtils;

    SUBCASE("int, int")
    {
        std::map<int, int> numbers{{0, 1}, {1, 1}, {3, 3}};
        CHECK(ContainsKey(numbers, 0));
        CHECK(ContainsKey(numbers, 1));
        CHECK_FALSE(ContainsKey(numbers, 2));
    }

    SUBCASE("string, string")
    {
        std::map<std::string, std::string> strings{{"test", "testValue"}, {"testKey", "testValue2"}};
        CHECK(ContainsKey(strings, "test"));
        CHECK(ContainsKey(strings, "testKey"));
        CHECK_FALSE(ContainsKey(strings, "nothing here"));
    }
}

TEST_CASE("FileReadAllText")
{
    using namespace StdUtils;

    std::filesystem::path test_files_path = Tests::Utils::GetWorkDir() / "test-files";

    WHEN("File exists")
    {
        THEN("File should be read correctly")
        {
            const std::string text = "name=\"Remove Stamina\";"
                                     "picture=\"logo.paa\";"
                                     "hidePicture=\"false\";"
                                     "hideName=\"false\";"
                                     "logo=\"logo.paa\";"
                                     "description=\"Simple mod which removes stamina from ArmA 3\";"
                                     "author=\"Muttley\";";
            CHECK_EQ(text, FileReadAllText(test_files_path / "mod-remove-stamina-no-whitespaces.cpp"));
        }
    }

    WHEN("File does not exist")
    {
        THEN("Exception is thrown")
        {
            using namespace std::filesystem;
            CHECK_THROWS_AS(FileReadAllText(test_files_path / "not-existing-file"), filesystem_error);
        }
    }
}

TEST_CASE("FileWriteAllText")
{
    using namespace StdUtils;
    using namespace std::filesystem;
    path work_dir = Tests::Utils::GetWorkDir();
    path text_file_path = work_dir / "test.txt";

    GIVEN("Text to write")
    {
        std::string const text = "name=\"Remove Stamina\";"
                                 "picture=\"logo.paa\";"
                                 "hidePicture=\"false\";"
                                 "hideName=\"false\";"
                                 "logo=\"logo.paa\";"
                                 "description=\"Simple mod which removes stamina from ArmA 3\";"
                                 "author=\"Muttley\";";
        WHEN("File does not exist")
        {
            THEN("New file is created and it contains text")
            {
                FileWriteAllText(text_file_path, text);
                CHECK_EQ(text, FileReadAllText(text_file_path));
            }
        }

        WHEN("File already exists and has some content inside")
        {
            FileWriteAllText(text_file_path, "trash");
            CHECK_EQ("trash", FileReadAllText(text_file_path));

            THEN("File should be truncated and filled")
            {
                FileWriteAllText(text_file_path, text);
                CHECK_EQ(text, FileReadAllText(text_file_path));
            }
        }

        WHEN("Directory does not exist")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(FileWriteAllText(work_dir / "not-existing-directory/newfile.txt", text), filesystem_error);
            }
        }
    }
}



TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
