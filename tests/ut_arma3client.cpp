#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#include "exceptions/file_not_found.hpp"
#include "exceptions/path_no_access.hpp"

#include <algorithm>
#include <string>

#include "arma3client.hpp"

class ARMA3ClientTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
        std::string work_dir = GetWorkDir() + "/arma3-client-tests";

        virtual void SetUp()
        {
            Filesystem::DirectoryCreate(work_dir);
            int a = 0;
        }
        virtual void TearDown()
        {
            Filesystem::DirectoryDelete(work_dir, true);
        }
};

TEST_F(ARMA3ClientTests, Constructor)
{
    ASSERT_NO_THROW(
    {
        ARMA3Client a3c(dir + "/arma3", dir + "/arma3/!workshop", true);
    });
    ASSERT_THROW(
    {
        ARMA3Client a3c(dir + "/farma3", dir + "/farma3/!workshop", true);
    }, FileNotFoundException);
}

TEST_F(ARMA3ClientTests, GetMods)
{
    std::string arma3_dir = "/arma3";
    std::string workshop_dir = "/!workshop";
    std::string mods_dir = dir + arma3_dir + workshop_dir;
    std::string remove_stamina_dir = mods_dir + "/@Remove stamina";
    std::string big_mod_dir = mods_dir + "/@bigmod";

    ARMA3Client a3c(dir + arma3_dir, mods_dir, true);
    a3c.RefreshMods();
    std::vector<Mod> mods{{{remove_stamina_dir, remove_stamina_map}, {big_mod_dir, big_mod_map}}};
    ASSERT_EQ(mods, a3c.mods_workshop_);
}

TEST_F(ARMA3ClientTests, CreateWorkshopSymlink)
{
    std::string arma3_dir = work_dir + "/arma3";
    std::string workshop_dir = arma3_dir + "/!workshop";
    std::string steam_workshop_dir = dir + "/steam/steamapps/workshop/content/107410";
    std::string executable_name = EXECUTABLE_NAME;
    std::vector<std::string> ls_result{"@Remove Stamina", "@bigmod"};

    ASSERT_EQ(0, Filesystem::DirectoryCreate(arma3_dir));
    ASSERT_EQ(0, Filesystem::FileCreate(arma3_dir + "/" + executable_name));

    ARMA3Client a3c(arma3_dir, steam_workshop_dir, true);
    EXPECT_TRUE(a3c.CreateSymlinkToWorkshop());
    ASSERT_TRUE(Filesystem::DirectoryExists(workshop_dir));
    ASSERT_EQ(ls_result, Filesystem::Ls(workshop_dir));
}
