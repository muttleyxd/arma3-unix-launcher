#include "gtest/gtest.h"

#include "filesystem.hpp"

#include <algorithm>
#include <string>

std::string RemoveLastElement(std::string s, bool removeSlash, int count)
{
    if (s.length() == 0)
        return "";
    std::reverse(s.begin(), s.end());

    for (int i = 0; i < count; i++)
    {
        size_t slashPos = s.find("/");
        slashPos++;

        s = s.substr(slashPos);
    }
    if (!removeSlash)
        s = "/" + s;
    std::reverse(s.begin(), s.end());

    return s;
}

std::string getworkdir()
{
    std::string ret = "";
    char buf[1024];
    ssize_t result = readlink("/proc/self/exe", buf, sizeof(buf));
    if (result != 0)
    {
        buf[result] = 0;
        ret = RemoveLastElement(buf, true, 1);
    }
    return ret;
}

class FilesystemTests : public ::testing::Test
{
public:
    std::string dir = getworkdir() + "/fs-tests";

    virtual void SetUp()
    {
        Filesystem::DirectoryCreate(dir);
    }
    virtual void TearDown()
    {
        Filesystem::DirectoryDelete(dir, true);
    }
};

/*
 * This test currently tests functions provided by OS (mkdir, rmdir, stat), but I'm leaving
 * it here in case if some kind of wrapping will be needed for it later
 */
TEST_F(FilesystemTests, DirectoryCreateExistsDelete)
{
    ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/subdir"), 0);
    ASSERT_EQ(Filesystem::DirectoryExists(dir + "/subdir"), 0);
    ASSERT_EQ(Filesystem::DirectoryDelete(dir + "/subdir"), 0);
    errno = 0;
    ASSERT_EQ(Filesystem::DirectoryExists(dir + "/subdir"), -1);
    EXPECT_EQ(errno, ENOENT);

    ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/dir with spaces"), 0);
    ASSERT_EQ(Filesystem::DirectoryExists(dir + "/dir with spaces"), 0);
    ASSERT_EQ(Filesystem::DirectoryDelete(dir + "/dir with spaces"), 0);
    errno = 0;
    ASSERT_EQ(Filesystem::DirectoryExists(dir + "/dir with spaces"), -1);
    EXPECT_EQ(errno, ENOENT);

    errno = 0;
    ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/not-existing-dir/subdirectory"), -1);
    EXPECT_EQ(errno, ENOENT);

    errno = 0;
    ASSERT_EQ(Filesystem::DirectoryCreate("/rootdir"), -1);
    EXPECT_EQ(errno, EACCES);
}

TEST_F(FilesystemTests, FileCreateExistsDelete)
{
    ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/dir with spaces"), 0);
    ASSERT_EQ(Filesystem::DirectoryExists(dir + "/dir with spaces"), 0);

    std::array<std::string, 3> file_names{"/file", "/dir with spaces/file-in-subdir", "/dir with spaces/file with spaces"};

    for (auto& file_name: file_names)
    {
        ASSERT_EQ(Filesystem::FileCreate(dir + file_name), 0);
        ASSERT_EQ(Filesystem::FileExists(dir + file_name), 0);
        ASSERT_EQ(Filesystem::FileDelete(dir + file_name), 0);
        errno = 0;
        ASSERT_EQ(Filesystem::FileExists(dir + file_name), -1);
        EXPECT_EQ(errno, ENOENT);
    }
}


TEST_F(FilesystemTests, ReadWriteText)
{
    std::string file_name = "/file.txt";
    std::string string1 = "abcdefghi\njklmnopq\n";
    std::string string2 = "123456789\n01234567\n";

    // Check reading from existing file
    ASSERT_EQ(Filesystem::FileCreate(dir + file_name, string1), string1.length());
    ASSERT_EQ(Filesystem::FileReadAllText(dir + file_name), string1);

    ASSERT_EQ(Filesystem::FileCreate(dir + file_name, string2), string1.length());
    ASSERT_EQ(Filesystem::FileReadAllText(dir + file_name), string2);

    ASSERT_EQ(Filesystem::FileCreate(dir + file_name, string1), string1.length());
    ASSERT_EQ(Filesystem::FileReadAllText(dir + file_name), string1);

    ASSERT_EQ(Filesystem::FileDelete(dir + file_name), 0);

    // Create new file with WriteAllText
    ASSERT_EQ(Filesystem::FileCreate(dir + file_name, string1), string1.length());
    ASSERT_EQ(Filesystem::FileReadAllText(dir + file_name), string1);
    ASSERT_EQ(Filesystem::FileDelete(dir + file_name), 0);
}

TEST_F(FilesystemTests, GetSubdirectories)
{
    std::array<std::string, 8> dir_names
    {
        "/dir1", "/dir2", "/dir3", "/dir4",
        "/dir5", "/dir6", "/dir7", "/dir8"
    };

    std::array<std::string, 2> subdir_names
    {
        "/Addons", "/Keys"
    };

    std::array<std::string, 2> file_names
    {
        "/something.pbo", "/some.key"
    };

    for (auto& dir_name: dir_names)
    {
        ASSERT_EQ(Filesystem::DirectoryCreate(dir + dir_name), 0);
        for (auto &subdir_name: subdir_names)
        {
            ASSERT_EQ(Filesystem::DirectoryCreate(dir + dir_name + subdir_name), 0);
            for (auto &file_name: file_names)
                ASSERT_EQ(Filesystem::FileCreate(dir + dir_name + subdir_name + file_name), 0);
        }
    }

    std::vector<std::string> subdirectories = Filesystem::GetSubdirectories(dir);
    std::sort(subdirectories.begin(), subdirectories.end());

    ASSERT_EQ(subdirectories.size(), 8);
    for (int i = 0; i < sizeof(dir_names); i++)
        ASSERT_EQ(subdirectories[i], dir_names[i]);
}

TEST_F(FilesystemTests, Symlinks)
{
    /*
     * test flow:
     * create dir with files and subdirectories
     * create symlinks, check if they work
     */
}
