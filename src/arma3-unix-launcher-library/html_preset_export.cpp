#include "html_preset_export.hpp"

#include "string_utils.hpp"

namespace
{
    constexpr char const *PRESET_TEMPLATE = R"modPreset(<?xml version="1.0" encoding="utf-8"?>
<html>
  <!--Exported with Arma 3 Unix Launcher: https://github.com/muttleyxd/arma3-unix-launcher-->
  <head>
    <meta name="arma:Type" content="preset" />
    <meta name="arma:PresetName" content="{preset_name}" />
    <meta name="generator" content="Arma 3 Launcher - https://arma3.com" />
    <title>A3UL preset - {preset_name}</title>
    <style>
body {{
        background: rgb(25, 81, 147);
        color: white;
        margin: 0px;
}}
td {{
    padding: 3px 30px 3px 0;
}}
h1 {{
    padding: 20px 20px 0 20px;
    font-weight: 200;
    font-size: 3em;
    margin: 0;
}}
em {{
    font-variant: italic;
    color:silver;
}}
.before-list {{
    padding: 5px 20px 10px 20px;
}}
.mod-list {{
    background: rgb(45, 48, 59);
    padding: 20px;
    height: 100%;
}}
.dlc-list {{
    background: #222222;
    padding: 20px;
}}
.footer {{
    padding-top: 20px;
    color:gray;
}}
.whups {{
    color:gray;
}}
a {{
    color: #D18F21;
    text-decoration: underline;
}}
a:hover {{
    color:#F1AF41;
    text-decoration: none;
}}
.from-steam {{
    color: #449EBD;
}}
.from-local {{
    color: gray;
}}
</style>
  </head>
  <body>
    <h1>Arma 3 - Preset <strong>{preset_name}</strong></h1>
    <p class="before-list">
      <em>Drag this file or link to it to Arma 3 Launcher or open it Mods / Preset / Import.</em>
    </p>
    <div class="mod-list">
      <table>
{mod_list}
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
)modPreset";

constexpr char const* STEAM_MOD_TEMPLATE = R"steamModTemplate(        <tr data-type="ModContainer">
          <td data-type="DisplayName">{mod_name}</td>
          <td>
            <span class="from-steam">Steam</span>
          </td>
          <td>
            <a href="http://steamcommunity.com/sharedfiles/filedetails/?id={workshop_id}" data-type="Link">http://steamcommunity.com/sharedfiles/filedetails/?id={workshop_id}</a>
          </td>
        </tr>
)steamModTemplate";

constexpr char const* LOCAL_MOD_TEMPLATE = R"localModTemplate(        <tr data-type="ModContainer">
          <td data-type="DisplayName">{mod_name}</td>
          <td>
            <span class="from-local">Local</span>
          </td>
          <td>
            <span class="whups" data-type="Link" data-meta="local:{mod_name}|{mod_dir}|{mod_link}">
              <a href="{mod_link}">{mod_link}</a>
            </span>
          </td>
        </tr>
)localModTemplate";
}

namespace ARMA3::HtmlPresetExport {
std::string export_mods(std::string_view preset_name, std::vector<Mod> const& mods, std::filesystem::path const& workshop_path)
{
    using namespace fmt::literals;

    std::string mod_list = "";
    for (auto const& mod: mods) {
        if (mod.IsWorkshopMod(workshop_path))
            mod_list += fmt::format(STEAM_MOD_TEMPLATE, "mod_name"_a=mod.GetName(), "workshop_id"_a=mod.path_.filename().string());
        else
            mod_list += fmt::format(LOCAL_MOD_TEMPLATE, "mod_name"_a=mod.GetName(), "mod_dir"_a=mod.path_.filename().string(), "mod_link"_a=mod.GetValueOrReturnDefault("url", "action", "https://github.com/muttleyxd/arma3-unix-launcher"));
    }
    return fmt::format(PRESET_TEMPLATE, "mod_list"_a=StringUtils::trim_right(mod_list), "preset_name"_a=preset_name);
}
}
