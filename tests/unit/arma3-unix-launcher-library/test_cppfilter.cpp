#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fmt/format.h>

#include "default_test_reporter.hpp"

#include "cppfilter.hpp"

std::string const valid_config_file = R"cpp(steamLanguage="";
language="English";
forcedAdapterId=-1;
detectedAdapterId=2;
detectedAdapterVendorId=1;
detectedAdapterDeviceId=1;
detectedAdapterSubSysId=1;
detectedAdapterRevision=1;
detectedAdapterBenchmark=32;
detectedCPUBenchmark=81;
detectedCPUFrequency=4200;
detectedCPUCores=8;
detectedCPUDescription="Intel(R) Core(TM) i7-7700K CPU @ 4.20GHz";
winX=16;
winY=32;
winWidth=1280;
winHeight=960;
winDefWidth=1024;
winDefHeight=768;
fullScreenWidth=1680;
fullScreenHeight=1050;
enderWidth=1280;
renderHeight=960;
multiSampleCount=8;
multiSampleQuality=0;
particlesQuality=2;
GPU_MaxFramesAhead=1000;
GPU_DetectedFramesAhead=1;
HDRPrecision=16;
vsync=1;
AToC=15;
cloudsQuality=4;
waterSSReflectionsQuality=0;
pipQuality=3;
dynamicLightsQuality=4;
PPAA=7;
ppSSAO=6;
ppCaustics=1;
tripleBuffering=0;
ppBloom=1;
ppRotBlur=1;
ppRadialBlur=1;
ppDOF=1;
ppSharpen=1;
)cpp";

std::string const mod_classes = R"cpp(class ModLauncherList
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
)cpp";

std::string const unrelated_classes = R"cpp(
class UnrelatedClass
{
    class SubClass
    {
        class SubSubClass
        {
            text="YAY!";
        };
        class SubSubClass2
        {
            RandomField="OK";
        };
    };
};
)cpp";

std::string const error_prone_classes = R"cpp(teststring="\"\nsometext";
class BadClass
{
    class InheritingClass
    {
         text="\n\"\"Some text2\"";
         text2="SomeRa ndomText";
    };
};
)cpp";

TEST_CASE("RemoveModLauncherList")
{
    std::array<std::string, 8> config_files =
        {
            "",
            "some trash",
            valid_config_file,
            valid_config_file + mod_classes,
            valid_config_file + unrelated_classes,
            valid_config_file + mod_classes + unrelated_classes,
            valid_config_file + error_prone_classes,
            valid_config_file + error_prone_classes + mod_classes
        };

    std::array<std::string, 8> out_files =
        {
            "",
            "some trash",
            valid_config_file,
            valid_config_file,
            valid_config_file + unrelated_classes,
            valid_config_file + unrelated_classes,
            valid_config_file + error_prone_classes,
            valid_config_file + error_prone_classes
        };

    for (size_t i = 0; i < config_files.size(); ++i)
    {
        CppFilter cpp_filter{config_files[i]};
        CHECK_EQ(out_files[i], cpp_filter.RemoveClass("class ModLauncherList"));
    }
}
