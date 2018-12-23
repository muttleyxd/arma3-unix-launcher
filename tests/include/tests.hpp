#ifndef TESTS_HPP_
#define TESTS_HPP_

#ifndef DOCTEST_CONFIG_DISABLE

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace Tests::Utils
{
    std::string RemoveLastElement(std::string s, bool removeSlash, int count);
    std::filesystem::path GetWorkDir();
    std::vector<std::string> Ls(std::string const &path);
    extern std::map<std::string, std::string> remove_stamina_map;
    extern std::map<std::string, std::string> big_mod_map;
    extern std::map<std::string, std::string> random_mod_map;
    extern std::string valid_config_file;
    extern std::string mod_classes;
    extern std::string unrelated_classes;
    extern std::string error_prone_classes;
}

namespace Tests
{
class Fixture
{
    public:
        Fixture() = default;
        ~Fixture() = default;

        std::filesystem::path work_dir = Tests::Utils::GetWorkDir();
        std::filesystem::path test_files_dir = "test-files";
        std::filesystem::path arma3_dir = "arma3";
        std::filesystem::path custom_dir = "!custom";
        std::filesystem::path workshop_dir = "!workshop";
        std::filesystem::path remove_stamina_dir = "@Remove stamina";
        std::filesystem::path big_mod_dir = "@bigmod";
        std::filesystem::path random_mod_dir = "@Random_Mod";
        std::filesystem::path rand_mod_v2_dir = "@rand_mod_v2";

        std::filesystem::path test_files_path = work_dir / test_files_dir;
};

}

#endif

#endif
