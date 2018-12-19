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
            size_t slashPos = s.find('/');
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
        ssize_t result = readlink("/proc/self/exe", static_cast<char*>(buf), sizeof(buf));
        if (result != 0)
        {
            buf[result] = 0;
            ret = RemoveLastElement(buf, true, 1);
        }
        return ret;
    }

    std::vector<std::string> Ls(std::string const &path)
    {
        std::vector<std::string> ret;
        DIR *dir = opendir(path.c_str());
        if (!dir)
            return ret;

        dirent *dp;
        while ((dp = readdir(dir)))
            ret.emplace_back(dp->d_name);
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
        {"picture", R"(bigmod\addons\not\nice\path\with\backslashes\picture.paa)"},
        {"actionName", "$STR_MOD_LAUNCHER_ACTION"},
        {"action", "https://bigmod.somewhere"},
        {"description", "Bugtracker: https://bigmod.somewhere/issues<br/>Documentation: https://bigmod.somewhere/wiki"},
        {"hideName", "1"},
        {"hidePicture", "0"},
        {"logo", R"(bigmod\addons\not\nice\path\with\backslashes\logo.paa)"},
        {"logoOver", R"(bigmod\addons\not\nice\path\with\backslashes\logo.paa)"},
        {"tooltip", "Big Mod"},
        {"tooltipOwned", "Big Mod Owned"},
        {"overview", "Big Mod is very Big."},
        {"author", "Big Modders"},
        {"overviewPicture", R"(bigmod\addons\not\nice\path\with\backslashes\logo.paa)"},
        {"overviewText", "Big Mod for Arma 3"},
        {"overviewFootnote", "<br /><br /><t color='#aa00aa'>Some random bla bla with HTML tags.<br />This will be <t /><t color='#ffaa00'>annoying<t /> to parse"},
        {"version", "1.0a"}
    };

    std::map<std::string, std::string> random_mod_map
    {
        {"name", "Random Mod"},
        {"description", "Another mod for unit-tests"}
    };

    std::string valid_config_file =
            R"(steamLanguage="";)" "\n"
            R"(language="English";)" "\n"
            "forcedAdapterId=-1;" "\n"
            "detectedAdapterId=2;" "\n"
            "detectedAdapterVendorId=1;" "\n"
            "detectedAdapterDeviceId=1;" "\n"
            "detectedAdapterSubSysId=1;" "\n"
            "detectedAdapterRevision=1;" "\n"
            "detectedAdapterBenchmark=32;" "\n"
            "detectedCPUBenchmark=81;" "\n"
            "detectedCPUFrequency=4200;" "\n"
            "detectedCPUCores=8;" "\n"
            R"(detectedCPUDescription="Intel(R) Core(TM) i7-7700K CPU @ 4.20GHz";)" "\n"
            "winX=16;" "\n"
            "winY=32;" "\n"
            "winWidth=1280;" "\n"
            "winHeight=960;" "\n"
            "winDefWidth=1024;" "\n"
            "winDefHeight=768;" "\n"
            "fullScreenWidth=1680;" "\n"
            "fullScreenHeight=1050;" "\n"
            "enderWidth=1280;" "\n"
            "renderHeight=960;" "\n"
            "multiSampleCount=8;" "\n"
            "multiSampleQuality=0;" "\n"
            "particlesQuality=2;" "\n"
            "GPU_MaxFramesAhead=1000;" "\n"
            "GPU_DetectedFramesAhead=1;" "\n"
            "HDRPrecision=16;" "\n"
            "vsync=1;" "\n"
            "AToC=15;" "\n"
            "cloudsQuality=4;" "\n"
            "waterSSReflectionsQuality=0;" "\n"
            "pipQuality=3;" "\n"
            "dynamicLightsQuality=4;" "\n"
            "PPAA=7;" "\n"
            "ppSSAO=6;" "\n"
            "ppCaustics=1;" "\n"
            "tripleBuffering=0;" "\n"
            "ppBloom=1;" "\n"
            "ppRotBlur=1;" "\n"
            "ppRadialBlur=1;" "\n"
            "ppDOF=1;" "\n"
            "ppSharpen=1;" "\n";
    std::string valid_config_file_with_mod_classes = valid_config_file +
            class ModLauncherList
            {
                class Mod1
                {
                    dir="@ALiVE";
                    name="Arma 3: ALiVE";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@ALiVE";
                };
                class Mod2
                {
                    dir="@CUP Terrains - CWA";
                    name="CUP Terrains - CWA 1.4.2";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - CWA";
                };
                class Mod3
                {
                    dir="@CUP Terrains - Core";
                    name="CUP Terrains - Core 1.4.2";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - Core";
                };
                class Mod4
                {
                    dir="@CUP Terrains - Maps";
                    name="CUP Terrains - Maps 1.4.2";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - Maps";
                };
                class Mod5
                {
                    dir="@CUP Units";
                    name="CUP Units 1.10.1";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Units";
                };
                class Mod6
                {
                    dir="@CUP Vehicles";
                    name="CUP Vehicles 1.10.1";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Vehicles";
                };
                class Mod7
                {
                    dir="@CUP Weapons";
                    name="CUP Weapons 1.10.1";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Weapons";
                };
                class Mod8
                {
                    dir="@CBA_A3";
                    name="Community Base Addons v3.9.0";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CBA_A3";
                };
                class Mod9
                {
                    dir="@Enhanced Movement";
                    name="Enhanced Movement";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@Enhanced Movement";
                };
                class Mod10
                {
                    dir="@RHSAFRF";
                    name="RHS: Armed Forces of the Russian Federation";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSAFRF";
                };
                class Mod11
                {
                    dir="@RHSGREF";
                    name="RHS: GREF";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSGREF";
                };
                class Mod12
                {
                    dir="@RHSSAF";
                    name="RHS: Serbian Armed Forces";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSSAF";
                };
                class Mod13
                {
                    dir="@RHSUSAF";
                    name="RHS: United States Forces";
                    origin="GAME DIR";
                    fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSUSAF";
                };
            };
                     ;
    std::string valid_config_file_with_unrelated_classes;
    std::string valid_config_file_with_unrelated_and_mod_classes;
}
