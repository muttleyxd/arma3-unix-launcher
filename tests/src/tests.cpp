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

    std::string mod_classes =
            "class ModLauncherList" "\n"
            "{" "\n"
            "    class Mod1" "\n"
            "    {" "\n"
            R"(            dir="@ALiVE";)" "\n"
            R"(            name="Arma 3: ALiVE";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@ALiVE";)" "\n"
            "    };" "\n"
            "    class Mod2" "\n"
            "    {" "\n"
            R"(            dir="@CUP Terrains - CWA";)" "\n"
            R"(            name="CUP Terrains - CWA 1.4.2";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - CWA";)" "\n"
            "    };" "\n"
            "    class Mod3" "\n"
            "    {" "\n"
            R"(            dir="@CUP Terrains - Core";)" "\n"
            R"(            name="CUP Terrains - Core 1.4.2";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - Core";)" "\n"
            "    };" "\n"
            "    class Mod4" "\n"
            "    {" "\n"
            R"(            dir="@CUP Terrains - Maps";)" "\n"
            R"(            name="CUP Terrains - Maps 1.4.2";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Terrains - Maps";)" "\n"
            "    };" "\n"
            "    class Mod5" "\n"
            "    {" "\n"
            R"(            dir="@CUP Units";)" "\n"
            R"(            name="CUP Units 1.10.1";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Units";)" "\n"
            "    };" "\n"
            "    class Mod6" "\n"
            "    {" "\n"
            R"(            dir="@CUP Vehicles";)" "\n"
            R"(            name="CUP Vehicles 1.10.1";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Vehicles";)" "\n"
            "    };" "\n"
            "    class Mod7" "\n"
            "    {" "\n"
            R"(            dir="@CUP Weapons";)" "\n"
            R"(            name="CUP Weapons 1.10.1";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CUP Weapons";)" "\n"
            "    };" "\n"
            "    class Mod8" "\n"
            "    {" "\n"
            R"(            dir="@CBA_A3";)" "\n"
            R"(            name="Community Base Addons v3.9.0";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@CBA_A3";)" "\n"
            "    };" "\n"
            "    class Mod9" "\n"
            "    {" "\n"
            R"(            dir="@Enhanced Movement";)" "\n"
            R"(            name="Enhanced Movement";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@Enhanced Movement";)" "\n"
            "    };" "\n"
            "    class Mod10" "\n"
            "    {" "\n"
            R"(            dir="@RHSAFRF";)" "\n"
            R"(            name="RHS: Armed Forces of the Russian Federation";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSAFRF";)" "\n"
            "    };" "\n"
            "    class Mod11" "\n"
            "    {" "\n"
            R"(            dir="@RHSGREF";)" "\n"
            R"(            name="RHS: GREF";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSGREF";)" "\n"
            "    };" "\n"
            "    class Mod12" "\n"
            "    {" "\n"
            R"(            dir="@RHSSAF";)" "\n"
            R"(            name="RHS: Serbian Armed Forces";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSSAF";)" "\n"
            "    };" "\n"
            "    class Mod13" "\n"
            "    {" "\n"
            R"(            dir="@RHSUSAF";)" "\n"
            R"(            name="RHS: United States Forces";)" "\n"
            R"(            origin="GAME DIR";)" "\n"
            R"(            fullPath="C:\home\muttley\.steam\steam\steamapps\common\Arma 3\!workshop\@RHSUSAF";)" "\n"
            "    };" "\n"
            "};" "\n";

    std::string unrelated_classes =
            "class UnrelatedClass" "\n"
            "{" "\n"
            "    class SubClass" "\n"
            "    {" "\n"
            "        class SubSubClass" "\n"
            "        {" "\n"
            R"(            text="YAY!";)" "\n"
            "        };" "\n"
            "        class SubSubClass2" "\n"
            "        {" "\n"
            R"(            RandomField="OK";)" "\n"
            "        };" "\n"
            "    };" "\n"
            "};" "\n"
            ;

    std::string error_prone_classes =
            R"(teststring="\"\nsometext";)" "\n"
            "class BadClass" "\n"
            "{" "\n"
            "    class InheritingClass" "\n"
            "    {" "\n"
            R"(        text="\n\"\"Some text2\"";)" "\n"
            R"(        text2="SomeRa ndomText";)" "\n"
            "    };" "\n"
            "};" "\n"
            ;
}
