#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>
#include "default_test_reporter.hpp"

#include "html_preset_export.hpp"
#include "mod.hpp"
#include "mock/mod.hpp"
#include "mock/std_utils.hpp"

using ARMA3::HtmlPresetExport::export_mods;
using trompeloeil::_;

class HtmlPresetExportTests
{
    public:
        ModMock mod_mock;
        StdUtilsMock std_utils_mock;

        std::filesystem::path const workshop_path = "/workshop_path";
};

TEST_CASE_FIXTURE(HtmlPresetExportTests, "ExportHtmlPreset_SingleSteamMod")
{
    std::string const expected_preset = R"preset(<?xml version="1.0" encoding="utf-8"?>
<html>
  <!--Exported with Arma 3 Unix Launcher: https://github.com/muttleyxd/arma3-unix-launcher-->
  <head>
    <meta name="arma:Type" content="preset" />
    <meta name="arma:PresetName" content="test preset" />
    <meta name="generator" content="Arma 3 Launcher - https://arma3.com" />
    <title>A3UL preset - test preset</title>
    <style>
body {
        background: rgb(25, 81, 147);
        color: white;
        margin: 0px;
}
td {
    padding: 3px 30px 3px 0;
}
h1 {
    padding: 20px 20px 0 20px;
    font-weight: 200;
    font-size: 3em;
    margin: 0;
}
em {
    font-variant: italic;
    color:silver;
}
.before-list {
    padding: 5px 20px 10px 20px;
}
.mod-list {
    background: rgb(45, 48, 59);
    padding: 20px;
    height: 100%;
}
.dlc-list {
    background: #222222;
    padding: 20px;
}
.footer {
    padding-top: 20px;
    color:gray;
}
.whups {
    color:gray;
}
a {
    color: #D18F21;
    text-decoration: underline;
}
a:hover {
    color:#F1AF41;
    text-decoration: none;
}
.from-steam {
    color: #449EBD;
}
.from-local {
    color: gray;
}
</style>
  </head>
  <body>
    <h1>Arma 3 - Preset <strong>test preset</strong></h1>
    <p class="before-list">
      <em>Drag this file or link to it to Arma 3 Launcher or open it Mods / Preset / Import.</em>
    </p>
    <div class="mod-list">
      <table>
        <tr data-type="ModContainer">
          <td data-type="DisplayName">1234 mod &amp; special character</td>
          <td>
            <span class="from-steam">Steam</span>
          </td>
          <td>
            <a href="http://steamcommunity.com/sharedfiles/filedetails/?id=1234" data-type="Link">http://steamcommunity.com/sharedfiles/filedetails/?id=1234</a>
          </td>
        </tr>
      </table>
    </div>
    <div class="dlc-list">
      <table/>
    </div>
    <div class="footer">
      <span>Exported with <a href="https://github.com/muttleyxd/arma3-unix-launcher">Arma 3 Unix Launcher</a>.</span>
    </div>
  </body>
</html>
)preset";

    std::filesystem::path const mod_path = workshop_path / "1234";

    REQUIRE_CALL(mod_mock, Constructor(mod_path, _));
    std::vector<Mod> mod_list
    {
        Mod(mod_path)
    };

    REQUIRE_CALL(mod_mock, IsWorkshopMod(workshop_path, _)).RETURN(true);
    REQUIRE_CALL(mod_mock, GetName(_)).RETURN("1234 mod & special character");
    auto const preset = export_mods("test preset", mod_list, workshop_path);
    CHECK_EQ(expected_preset, preset);
}
