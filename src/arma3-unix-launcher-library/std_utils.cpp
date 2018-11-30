#include "std_utils.hpp"

namespace StdUtils
{
    bool Contains(const std::vector<std::string> &vec, const char *t)
    {
        return std::any_of(vec.begin(), vec.end(), std::bind2nd(std::equal_to<std::string>(), std::string(t)));
    }

    bool CreateFile(const std::filesystem::path &path)
    {
        return creat(path.c_str(), 0644);
    }

    std::vector<std::string> Ls(const std::filesystem::path &path, bool set_lowercase)
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

    std::string FileReadAllText(const std::filesystem::path &path)
    {
        int fd = open(path.c_str(), 0);
        if (fd == -1)
            throw PathNoAccessException(std::string(path) + "\nError: " + std::to_string(errno) + " " + strerror(errno));
        size_t file_size = static_cast<size_t>(lseek(fd, 0, SEEK_END));
        lseek(fd, 0, SEEK_SET);

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(file_size);
        read(fd, buffer.get(), file_size);
        close(fd);
        return buffer.get();
    }
}

#include <doctest.h>

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

TEST_SUITE_END();
