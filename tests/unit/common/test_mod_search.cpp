#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "default_test_reporter.hpp"

#include "mod_search.hpp"

TEST_CASE("Matches_EmptyOrBlankQuery_MatchesEverything")
{
    using namespace ModSearch;
    CHECK(Matches("ACE3", "463939057", ""));
    CHECK(Matches("ACE3", "463939057", "   "));
    CHECK(Matches("ACE3", "463939057", " \t\n"));
    CHECK(Matches("", "", ""));
}

TEST_CASE("Matches_IsCaseInsensitiveSubstringOfName")
{
    using namespace ModSearch;
    CHECK(Matches("ACE3 Medical", "", "ace"));
    CHECK(Matches("ACE3 Medical", "", "MEDIC"));
    CHECK(Matches("ACE3 Medical", "", "3 med"));
    CHECK(Matches("CBA_A3", "", "cba_a3"));
    CHECK_FALSE(Matches("ACE3 Medical", "", "rhs"));
    CHECK_FALSE(Matches("", "", "ace"));
}

TEST_CASE("Matches_AllWordsMustMatch")
{
    using namespace ModSearch;
    CHECK(Matches("ACE3 Medical", "", "ace med"));
    CHECK(Matches("ACE3 Medical", "", "med ace"));
    CHECK(Matches("ACE3 Medical", "", "  ace   med  "));
    CHECK_FALSE(Matches("ACE3 Medical", "", "ace rhs"));
    CHECK_FALSE(Matches("ACE3 Medical", "", "ace\trhs"));
}

TEST_CASE("Matches_WorkshopId")
{
    using namespace ModSearch;
    CHECK(Matches("ACE3", "463939057", "463939057"));
    CHECK(Matches("ACE3", "463939057", "4639"));
    CHECK(Matches("ACE3", "463939057", "ace 4639"));
    CHECK_FALSE(Matches("ACE3", "463939057", "450814997"));
    CHECK_FALSE(Matches("ACE3", "", "4639"));
}

TEST_CASE("Matches_NonAsciiIsComparedVerbatim")
{
    using namespace ModSearch;
    CHECK(Matches("Ruská armáda", "", "armáda"));
    CHECK(Matches("Ruská armáda", "", "RUSKÁ") == false); // only ASCII letters are case folded
    CHECK(Matches("Ruská armáda", "", "ruská"));
    CHECK(Matches("\xff\xfe mod", "", "MOD"));
}
