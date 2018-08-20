#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#include "exceptions/not_a_symlink.hpp"
#include "exceptions/path_no_access.hpp"

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
    ASSERT_EQ(0, Filesystem::DirectoryCreate(dir + "/subdir"));
    ASSERT_TRUE(Filesystem::DirectoryExists(dir + "/subdir"));
    ASSERT_EQ(0, Filesystem::DirectoryDelete(dir + "/subdir"));
    errno = 0;
    ASSERT_FALSE(Filesystem::DirectoryExists(dir + "/subdir"));
    EXPECT_EQ(ENOENT, errno);

    ASSERT_EQ(0, Filesystem::DirectoryCreate(dir + "/dir with spaces"));
    ASSERT_TRUE(Filesystem::DirectoryExists(dir + "/dir with spaces"));
    ASSERT_EQ(0, Filesystem::DirectoryDelete(dir + "/dir with spaces"));
    errno = 0;
    ASSERT_FALSE(Filesystem::DirectoryExists(dir + "/dir with spaces"));
    EXPECT_EQ(ENOENT, errno);

    errno = 0;
    ASSERT_EQ(-1, Filesystem::DirectoryCreate(dir + "/not-existing-dir/subdirectory"));
    EXPECT_EQ(ENOENT, errno);

    errno = 0;
    ASSERT_EQ(-1, Filesystem::DirectoryCreate("/rootdir"));
    EXPECT_EQ(errno, EACCES);
}

TEST_F(FilesystemTests, FileCreateExistsDelete)
{
    ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/dir with spaces"), 0);
    ASSERT_TRUE(Filesystem::DirectoryExists(dir + "/dir with spaces"));

    std::array<std::string, 3> file_names{"/file", "/dir with spaces/file-in-subdir", "/dir with spaces/file with spaces"};

    for (auto &file_name : file_names)
    {
        ASSERT_EQ(0, Filesystem::FileCreate(dir + file_name));
        ASSERT_TRUE(Filesystem::FileExists(dir + file_name));
        ASSERT_EQ(0, Filesystem::FileDelete(dir + file_name));
        errno = 0;
        ASSERT_FALSE(Filesystem::FileExists(dir + file_name));
        EXPECT_EQ(ENOENT, errno);
    }

    errno = 0;
    ASSERT_FALSE(Filesystem::FileExists("/invalid-dir/invalid-file"));
    EXPECT_EQ(ENOENT, errno);
}


TEST_F(FilesystemTests, ReadWriteText)
{
    std::string file_name = "/file.txt";
    std::string string1 = "abcdefghi\njklmnopq\n";
    std::string string2 = "123456789\n01234567\n";

    // Check reading from existing file
    ASSERT_EQ(string1.length(), Filesystem::FileCreate(dir + file_name, string1));
    ASSERT_EQ(string1, Filesystem::FileReadAllText(dir + file_name));

    ASSERT_EQ(string1.length(), Filesystem::FileCreate(dir + file_name, string2));
    ASSERT_EQ(string2, Filesystem::FileReadAllText(dir + file_name));

    ASSERT_EQ(string1.length(), Filesystem::FileCreate(dir + file_name, string1));
    ASSERT_EQ(string1, Filesystem::FileReadAllText(dir + file_name));

    ASSERT_EQ(0, Filesystem::FileDelete(dir + file_name));

    // Create new file with WriteAllText
    ASSERT_EQ(string1.length(), Filesystem::FileCreate(dir + file_name, string1));
    ASSERT_EQ(string1, Filesystem::FileReadAllText(dir + file_name));
    ASSERT_EQ(0, Filesystem::FileDelete(dir + file_name));

    // Exception when trying to read missing file
    ASSERT_THROW(Filesystem::FileReadAllText(dir + "/nowhere"), PathNoAccessException);
}

TEST_F(FilesystemTests, Ls)
{
    std::vector<std::string> dir_names
    {
        "dir1", "dir2", "dir3", "dir4",
        "dir5", "dir6", "dir7", "dir8"
    };

    std::vector<std::string> subdir_names
    {
        "Addons", "Keys"
    };

    std::vector<std::string> file_names
    {
        "something.pbo", "some.key"
    };

    std::string s = "/"; //separator;

    for (auto &dir_name : dir_names)
    {
        ASSERT_EQ(0, Filesystem::DirectoryCreate(dir + s + dir_name));
        for (auto &subdir_name : subdir_names)
        {
            ASSERT_EQ(0, Filesystem::DirectoryCreate(dir + s + dir_name + s + subdir_name));
            for (auto &file_name : file_names)
                ASSERT_EQ(0, Filesystem::FileCreate(dir + s + dir_name + s + subdir_name + s + file_name));
        }
    }

    std::vector<std::string> subdirectories = Filesystem::Ls(dir);
    std::sort(subdirectories.begin(), subdirectories.end());

    ASSERT_EQ(subdirectories.size(), 8);
    for (size_t i = 0; i < dir_names.size(); i++)
        ASSERT_EQ(subdirectories[i], dir_names[i]);

    ASSERT_THROW(Filesystem::Ls(dir + s + dir_names[0] + s + subdir_names[0] + s + file_names[0]), PathNoAccessException);
    ASSERT_THROW(Filesystem::Ls(dir + s + "nowhere"), PathNoAccessException);

    ASSERT_EQ(file_names, Filesystem::Ls(dir + s + dir_names[0] + s + subdir_names[0]));

    std::vector<std::string> subdir_names_lowercase
    {
        "addons", "keys"
    };
    ASSERT_EQ(subdir_names_lowercase, Filesystem::Ls(dir + s + dir_names[0], true));
}

TEST_F(FilesystemTests, Symlinks)
{
    for (int i = 0; i < 10; i++)
    {
        std::string dir_name = "dir" + std::to_string(i);
        ASSERT_EQ(Filesystem::DirectoryCreate(dir + "/" + dir_name), 0);
        ASSERT_FALSE(Filesystem::SymlinkExists(dir + "/" + dir_name));
        ASSERT_EQ(Filesystem::FileCreate(dir + "/" + dir_name + "/file" + std::to_string(i)), 0);
        ASSERT_EQ(Filesystem::SymlinkCreate(dir + "/" + dir_name + "_symlink", dir_name), 0);
        ASSERT_TRUE(Filesystem::SymlinkExists(dir + "/" + dir_name + "_symlink"));
        ASSERT_EQ(Filesystem::SymlinkCreate(dir + "/" + dir_name + "_symlink_absolute", dir + "/" + dir_name), 0);
        ASSERT_TRUE(Filesystem::SymlinkExists(dir + "/" + dir_name + "_symlink_absolute"));
        std::vector<std::string> lsResult, lsResultSymlink, lsResultSymlinkAbsolute;
        lsResult = Ls(dir + "/" + dir_name);
        lsResultSymlink = Ls(dir + "/" + dir_name + "_symlink");
        lsResultSymlinkAbsolute = Ls(dir + "/" + dir_name + "_symlink_absolute");
        ASSERT_EQ(lsResult, lsResultSymlink);
        ASSERT_EQ(lsResult, lsResultSymlinkAbsolute);

        ASSERT_EQ(Filesystem::SymlinkGetTarget(dir + "/" + dir_name + "_symlink"), dir_name);
        ASSERT_EQ(Filesystem::SymlinkGetTarget(dir + "/" + dir_name + "_symlink_absolute"), dir + "/" + dir_name);
    }

    ASSERT_THROW(Filesystem::SymlinkGetTarget(dir), NotASymlinkException);
    ASSERT_THROW(Filesystem::SymlinkGetTarget(dir + "/nowhere"), PathNoAccessException);
}
