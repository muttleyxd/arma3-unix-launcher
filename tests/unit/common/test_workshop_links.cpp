#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "default_test_reporter.hpp"

#include "workshop_links.hpp"

TEST_CASE("SteamClientUrl")
{
    using namespace WorkshopLinks;
    CHECK_EQ(SteamClientUrl("463939057"), "steam://url/CommunityFilePage/463939057");
    CHECK_EQ(SteamClientUrl("1"), "steam://url/CommunityFilePage/1");
}

TEST_CASE("BrowserUrl")
{
    using namespace WorkshopLinks;
    CHECK_EQ(BrowserUrl("463939057"), "https://steamcommunity.com/sharedfiles/filedetails/?id=463939057");
    CHECK_EQ(BrowserUrl("18446744073709551615"),
             "https://steamcommunity.com/sharedfiles/filedetails/?id=18446744073709551615");
}

TEST_CASE("InvalidWorkshopIds_ReturnNullopt")
{
    using namespace WorkshopLinks;
    for (std::string_view const id :
            {
                "", " ", "abc", "4639x9057", "-463939057", "463939057 ", "/home/user/mods/@ace",
                "0", "000", "18446744073709551616", "123456789012345678901"
            })
    {
        CAPTURE(id);
        CHECK_FALSE(SteamClientUrl(id).has_value());
        CHECK_FALSE(BrowserUrl(id).has_value());
    }
}
