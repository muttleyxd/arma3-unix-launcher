#include "html_preset_parser.hpp"

#include <pugixml.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "string_utils.hpp"
#include "exceptions/html_parser.hpp"

namespace
{
    std::string get_name(pugi::xpath_node const &mod)
    {
        for (auto const &child : mod.node().children())
        {
            auto const &data_type = child.find_attribute([](pugi::xml_attribute const & attribute)
            {
                return attribute.name() == std::string("data-type") && attribute.value() == std::string("DisplayName");
            });
            if (!data_type.empty())
                return child.text().get();
        }
        throw "cannot get name";
    }

    std::string get_origin(pugi::xpath_node const &mod)
    {
        for (auto const &child : mod.node().children())
        {
            auto const &origin = child.find_node([](pugi::xml_node const & node)
            {
                return node.name() == std::string("span") && node.find_attribute([](pugi::xml_attribute const & attribute)
                {
                    return attribute.name() == std::string("class") && (attribute.value() == std::string("from-local")
                            || attribute.value() == std::string("from-steam"));
                });
            });
            if (!origin.empty())
                return origin.text().get();
        }
        throw "cannot get origin";
    }

    std::string get_path_local(pugi::xpath_node const &mod)
    {
        for (auto const &child : mod.node().children())
        {
            auto const &path = child.find_node([](pugi::xml_node const & node)
            {
                return node.name() == std::string("span") && node.find_attribute([](pugi::xml_attribute const & attribute)
                {
                    return attribute.name() == std::string("data-meta");
                });
            });
            if (!path.empty())
            {
                auto const data_meta =
                    path.attribute("data-meta").value(); // local:Modname|@localOrGlobalPath|
                return std::string(StringUtils::Split(data_meta, "|").at(1));
            }
        }
        throw "cannot get path";
    }

    std::string get_workshop_id_steam(pugi::xpath_node const &mod)
    {
        for (auto const &child : mod.node().children())
        {
            auto const &workshop_id = child.find_node([](pugi::xml_node const & node)
            {
                return node.name() == std::string("a") && node.find_attribute([](pugi::xml_attribute const & attribute)
                {
                    return attribute.name() == std::string("data-type") && attribute.value() == std::string("Link");
                });
            });
            if (!workshop_id.empty())
            {
                std::string_view const link =
                    workshop_id.attribute("href").value(); // http://steamcommunity.com/sharedfiles/filedetails/?id=12345678
                return std::string(link.substr(link.find('=') + 1));
            }
        }
        throw "cannot get path";
    }

    nlohmann::json mod_to_json(pugi::xpath_node const &mod)
    {
        nlohmann::json mod_json;
        mod_json["name"] = get_name(mod);
        auto const origin = get_origin(mod);
        mod_json["origin"] = origin;
        if (origin == "Local")
            mod_json["path"] = get_path_local(mod);
        else
            mod_json["path"] = get_workshop_id_steam(mod);
        return mod_json;
    }

    nlohmann::json get_mods(pugi::xpath_node_set const &mod_table)
    {
        nlohmann::json mods = nlohmann::json::array();
        for (pugi::xpath_node const mod : mod_table)
        {
            try
            {
                mods.push_back(mod_to_json(mod));
            }
            catch (std::exception const &e)
            {
                spdlog::warn("HTML mod parsing failed: {}", e.what());
            }
        }

        return mods;
    }
}

nlohmann::json Html::Preset::Parser::html_to_json(std::string const &html_text)
{
    pugi::xml_document html;
    if (auto const result = html.load_string(html_text.c_str()); !result)
        throw HtmlParserException(result.description());

    auto const mod_table = html.select_nodes(R"path(/html/body/div["mod-list"]/table/tr)path");
    return get_mods(mod_table);
}
