#include "html_preset_parser.hpp"

#include <pugixml.hpp>

#include "exceptions/html_parser.hpp"

nlohmann::json Html::Preset::Parser::html_to_json(std::string const &html_text)
{
    pugi::xml_document html;
    if (auto const result = html.load_string(html_text.c_str()); !result)
        throw HtmlParserException(result.description());

    nlohmann::json mods = nlohmann::json::array();

    auto const mod_table = html.select_nodes(R"path(/html/body/div["mod-list"]/table/tr)path");
    for (pugi::xpath_node const mod : mod_table)
    {
        nlohmann::json mod_json;
        for (auto const &child : mod.node().children())
        {
            auto data_type = child.find_attribute([](pugi::xml_attribute const & attribute)
            {
                return attribute.name() == std::string("data-type");
            });
            if (!data_type.empty() && data_type.value() == std::string("DisplayName"))
            {
                mod_json["displayName"] = child.text().get();
                continue;
            }

            auto origin = child.find_node([](pugi::xml_node const & node)
            {
                return node.name() == std::string("span");
            });
            if (!origin.empty())
            {
                mod_json["origin"] = origin.text().get();
                continue;
            }

            auto workshop_id = child.find_node([](pugi::xml_node const & node)
            {
                return node.name() == std::string("a");
            });
            if (!workshop_id.empty())
            {
                std::string_view const link =
                    workshop_id.attribute("href").value(); // http://steamcommunity.com/sharedfiles/filedetails/?id=12345678
                mod_json["workshopId"] = link.substr(link.find('=') + 1);
            }
        }
        mods.push_back(mod_json);
    }
    return mods;
}
