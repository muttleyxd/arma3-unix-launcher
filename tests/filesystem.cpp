#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#include <algorithm>
#include <string>

class FilesystemTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/fs-tests";

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

    for (auto &file_name : file_names)
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
        "dir1", "dir2", "dir3", "dir4",
        "dir5", "dir6", "dir7", "dir8"
    };

    std::array<std::string, 2> subdir_names
    {
        "Addons", "Keys"
    };

    std::array<std::string, 2> file_names
    {
        "something.pbo", "some.key"
    };

    std::string s = "/"; //separator;

    for (auto &dir_name : dir_names)
    {
        ASSERT_EQ(Filesystem::DirectoryCreate(dir + s + dir_name), 0);
        for (auto &subdir_name : subdir_names)
        {
            ASSERT_EQ(Filesystem::DirectoryCreate(dir + s + dir_name + s + subdir_name), 0);
            for (auto &file_name : file_names)
                ASSERT_EQ(Filesystem::FileCreate(dir + s + dir_name + s + subdir_name + s + file_name), 0);
        }
    }

    std::vector<std::string> subdirectories = Filesystem::GetSubdirectories(dir);
    std::sort(subdirectories.begin(), subdirectories.end());

    ASSERT_EQ(subdirectories.size(), 8);
    for (size_t i = 0; i < dir_names.size(); i++)
        ASSERT_EQ(subdirectories[i], dir_names[i]);

    try
    {
        errno = 0;
        std::vector<std::string> call_on_file = Filesystem::GetSubdirectories(dir + s + dir_names[0] + s + subdir_names[0] + s + file_names[0]);
        ASSERT_FALSE(true) << "This should not be executed - exception should be thrown earlier";
    }
    catch (std::exception &ex)
    {
        std::string_view exception_message(ex.what());
        ASSERT_EQ(exception_message, std::string("Error: 20 Not a directory"));
    }

    try
    {
        errno = 0;
        std::vector<std::string> not_exists = Filesystem::GetSubdirectories(dir + s + "nowhere");
        ASSERT_FALSE(true) << "This should not be executed - exception should be thrown earlier";
    }
    catch (std::exception &ex)
    {
        std::string_view exception_message(ex.what());
        ASSERT_EQ(exception_message, "Error: 2 No such file or directory");
    }
}

TEST_F(FilesystemTests, Symlinks)
{
    for (int i = 0; i < 10; i++)
    {
        std::string dir_name = "dir" + std::to_string(i);
        ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/" + dir_name), 0);
        ASSERT_EQ(Filesystem::FileCreate(dir + "/" + dir_name + "/file" + std::to_string(i)), 0);
        ASSERT_EQ(Filesystem::SymlinkCreate(dir + "/" + dir_name + "_symlink", dir_name), 0);
        ASSERT_EQ(Filesystem::SymlinkCreate(dir + "/" + dir_name + "_symlink_absolute", dir + "/" + dir_name), 0);
        std::vector<std::string> lsResult, lsResultSymlink, lsResultSymlinkAbsolute;
        lsResult = Ls(dir + "/" + dir_name);
        lsResultSymlink = Ls(dir + "/" + dir_name + "_symlink");
        lsResultSymlinkAbsolute = Ls(dir + "/" + dir_name + "_symlink_absolute");
        ASSERT_EQ(lsResult, lsResultSymlink);
        ASSERT_EQ(lsResult, lsResultSymlinkAbsolute);

        ASSERT_EQ(Filesystem::SymlinkGetTarget(dir + "/" + dir_name + "_symlink"), dir_name);
        ASSERT_EQ(Filesystem::SymlinkGetTarget(dir + "/" + dir_name + "_symlink_absolute"), dir + "/" + dir_name);
    }

    try
    {
        errno = 0;
        Filesystem::SymlinkGetTarget(dir);
        ASSERT_FALSE(true) << "This should not be executed - exception should be thrown earlier";
    }
    catch (std::exception &ex)
    {
        std::string_view exception_message(ex.what());
        ASSERT_EQ(exception_message, "Error: Not a symlink");
    }

    try
    {
        errno = 0;
        Filesystem::SymlinkGetTarget(dir + "/nowhere");
        ASSERT_FALSE(true) << "This should not be executed - exception should be thrown earlier";
    }
    catch (std::exception &ex)
    {
        std::string_view exception_message(ex.what());
        ASSERT_EQ(exception_message, "Error: 2 No such file or directory");
    }
}
