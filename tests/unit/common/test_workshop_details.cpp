#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <numeric>

#include "default_test_reporter.hpp"

#include "workshop_details.hpp"

TEST_CASE("BuildRequestBodies_SingleId")
{
    auto const bodies = WorkshopDetails::BuildRequestBodies({463939057});
    REQUIRE_EQ(bodies.size(), 1);
    CHECK_EQ(bodies[0], "itemcount=1&publishedfileids%5B0%5D=463939057");
}

TEST_CASE("BuildRequestBodies_MultipleIds")
{
    auto const bodies = WorkshopDetails::BuildRequestBodies({463939057, 450814997, 18446744073709551615ull});
    REQUIRE_EQ(bodies.size(), 1);
    CHECK_EQ(bodies[0], "itemcount=3"
                        "&publishedfileids%5B0%5D=463939057"
                        "&publishedfileids%5B1%5D=450814997"
                        "&publishedfileids%5B2%5D=18446744073709551615");
}

TEST_CASE("BuildRequestBodies_NoIds_NoRequests")
{
    CHECK(WorkshopDetails::BuildRequestBodies({}).empty());
}

TEST_CASE("BuildRequestBodies_SplitsIntoBatches")
{
    std::vector<std::uint64_t> ids(5);
    std::iota(ids.begin(), ids.end(), 1);

    auto const bodies = WorkshopDetails::BuildRequestBodies(ids, 2);
    REQUIRE_EQ(bodies.size(), 3);
    CHECK_EQ(bodies[0], "itemcount=2&publishedfileids%5B0%5D=1&publishedfileids%5B1%5D=2");
    CHECK_EQ(bodies[1], "itemcount=2&publishedfileids%5B0%5D=3&publishedfileids%5B1%5D=4");
    CHECK_EQ(bodies[2], "itemcount=1&publishedfileids%5B0%5D=5");
}

TEST_CASE("BuildRequestBodies_DefaultBatchIs100")
{
    std::vector<std::uint64_t> ids(250);
    std::iota(ids.begin(), ids.end(), 1);

    auto const bodies = WorkshopDetails::BuildRequestBodies(ids);
    REQUIRE_EQ(bodies.size(), 3);
    CHECK(bodies[0].rfind("itemcount=100&", 0) == 0);
    CHECK(bodies[1].rfind("itemcount=100&publishedfileids%5B0%5D=101&", 0) == 0);
    CHECK(bodies[2].rfind("itemcount=50&publishedfileids%5B0%5D=201&", 0) == 0);
}

TEST_CASE("BuildRequestBodies_ZeroBatchSize_TreatedAsOne")
{
    auto const bodies = WorkshopDetails::BuildRequestBodies({1, 2}, 0);
    REQUIRE_EQ(bodies.size(), 2);
    CHECK_EQ(bodies[1], "itemcount=1&publishedfileids%5B0%5D=2");
}
