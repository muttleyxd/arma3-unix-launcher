#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <vector>

#include "default_test_reporter.hpp"

#include "workshop_page.h"

namespace
{
    struct RecordingOpener
    {
        std::vector<QString> opened;
        std::vector<bool> results;

        std::function<bool(QUrl const &)> function()
        {
            return [this](QUrl const & url)
            {
                opened.push_back(url.toString());
                auto const result = opened.size() <= results.size() ? results[opened.size() - 1] : true;
                return result;
            };
        }
    };
}

TEST_CASE("OpensInSteamClient")
{
    RecordingOpener opener{{}, {true}};
    CHECK(open_workshop_page("463939057", opener.function()));
    CHECK_EQ(opener.opened, std::vector<QString> {"steam://url/CommunityFilePage/463939057"});
}

TEST_CASE("FallsBackToBrowserWithoutSteam")
{
    RecordingOpener opener{{}, {false, true}};
    CHECK(open_workshop_page("463939057", opener.function()));
    CHECK_EQ(opener.opened, (std::vector<QString>
    {
        "steam://url/CommunityFilePage/463939057",
        "https://steamcommunity.com/sharedfiles/filedetails/?id=463939057"
    }));
}

TEST_CASE("ReportsFailureWhenNothingOpens")
{
    RecordingOpener opener{{}, {false, false}};
    CHECK_FALSE(open_workshop_page("463939057", opener.function()));
    CHECK_EQ(opener.opened.size(), 2);
}

TEST_CASE("InvalidIdOpensNothing")
{
    RecordingOpener opener;
    CHECK_FALSE(open_workshop_page("/mods/@xyz", opener.function()));
    CHECK(opener.opened.empty());
}
