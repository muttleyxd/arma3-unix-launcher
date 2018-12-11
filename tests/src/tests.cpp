#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "tests.hpp"

#include <algorithm>

#include <dirent.h>
#include <unistd.h>

namespace Tests::Utils
{
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

    std::filesystem::path GetWorkDir()
    {
        std::filesystem::path ret;
        char buf[1024];
        ssize_t result = readlink("/proc/self/exe", buf, sizeof(buf));
        if (result != 0)
        {
            buf[result] = 0;
            ret = RemoveLastElement(buf, true, 1);
        }
        return ret;
    }

    std::vector<std::string> Ls(std::string path)
    {
        std::vector<std::string> ret;
        DIR *dir = opendir(path.c_str());
        if (!dir)
            return ret;

        dirent *dp;
        while ((dp = readdir(dir)))
            ret.push_back(dp->d_name);
        closedir(dir);
        std::sort(ret.begin(), ret.end());
        return ret;
    }

    std::map<std::string, std::string> remove_stamina_map
    {
        {"name", "Remove Stamina"},
        {"picture", "logo.paa"},
        {"hidePicture", "false"},
        {"hideName", "false"},
        {"logo", "logo.paa"},
        {"description", "Simple mod which removes stamina from ArmA 3"},
        {"author", "Muttley"}
    };

    std::map<std::string, std::string> big_mod_map
    {
        {"dir", "@bigmod"},
        {"name", "Big Mod"},
        {"picture", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\picture.paa"},
        {"actionName", "$STR_MOD_LAUNCHER_ACTION"},
        {"action", "https://bigmod.somewhere"},
        {"description", "Bugtracker: https://bigmod.somewhere/issues<br/>Documentation: https://bigmod.somewhere/wiki"},
        {"hideName", "1"},
        {"hidePicture", "0"},
        {"logo", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"logoOver", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"tooltip", "Big Mod"},
        {"tooltipOwned", "Big Mod Owned"},
        {"overview", "Big Mod is very Big."},
        {"author", "Big Modders"},
        {"overviewPicture", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"overviewText", "Big Mod for Arma 3"},
        {"overviewFootnote", "<br /><br /><t color='#aa00aa'>Some random bla bla with HTML tags.<br />This will be <t /><t color='#ffaa00'>annoying<t /> to parse"},
        {"version", "1.0a"}
    };

    std::map<std::string, std::string> random_mod_map
    {
        {"name", "Random Mod"},
        {"description", "Another mod for unit-tests"}
    };
}
