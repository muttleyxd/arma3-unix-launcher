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

namespace
{
    // Trimmed real response for ACE3 (463939057), an unknown id and some damaged entries
    constexpr char const *response_fixture = R"({"response":{"result":1,"resultcount":6,"publishedfiledetails":[
        {"publishedfileid":"463939057","result":1,"creator":"76561198194647182","consumer_app_id":107410,
         "preview_url":"https://images.steamusercontent.com/ugc/964230428541162652/F7DC4A5DD2A4896E2D572D7E9E085489426FC64B/",
         "title":"ace","time_updated":1787068350},
        {"publishedfileid":"1","result":9},
        {"publishedfileid":"450814997","result":1,"title":"CBA_A3","time_updated":1780000000},
        {"publishedfileid":"843425103","result":1,"title":"RHSAFRF","preview_url":"","time_updated":1780000001},
        {"publishedfileid":"not-a-number","result":1,"preview_url":"https://example.com/a.png"},
        {"publishedfileid":843577117,"result":"1","preview_url":["https://example.com/b.png"],"title":5}
    ]}})";
}

TEST_CASE("ParseResponse_RealShape")
{
    auto const items = WorkshopDetails::ParseResponse(response_fixture);
    REQUIRE_EQ(items.size(), 1);
    CHECK_EQ(items[0].id, 463939057);
    CHECK_EQ(items[0].title, "ace");
    CHECK_EQ(items[0].preview_url,
             "https://images.steamusercontent.com/ugc/964230428541162652/F7DC4A5DD2A4896E2D572D7E9E085489426FC64B/");
    CHECK_EQ(items[0].time_updated, 1787068350);
}

TEST_CASE("ParseResponse_OptionalFields")
{
    auto const items = WorkshopDetails::ParseResponse(
                           R"({"response":{"publishedfiledetails":[
                               {"publishedfileid":"42","result":1,"preview_url":"https://example.com/p.jpg"},
                               {"publishedfileid":"43","result":1,"preview_url":"https://example.com/q.jpg",
                                "title":null,"time_updated":"yesterday"}]}})");
    REQUIRE_EQ(items.size(), 2);
    CHECK_EQ(items[0], WorkshopDetails::ItemDetails{42, "", "https://example.com/p.jpg", 0});
    CHECK_EQ(items[1], WorkshopDetails::ItemDetails{43, "", "https://example.com/q.jpg", 0});
}

TEST_CASE("ParseResponse_MalformedOrUnexpected_ReturnsEmpty")
{
    using WorkshopDetails::ParseResponse;
    for (char const *body :
            {
                "", "{", "not json", "null", "[]", "42", R"({"response":null})", R"({"response":[]})",
                R"({"response":{}})", R"({"response":{"publishedfiledetails":{}}})",
                R"({"response":{"publishedfiledetails":[null, 1, "x", []]}})",
                "<html><body>502 Bad Gateway</body></html>"
            })
    {
        CAPTURE(body);
        CHECK(ParseResponse(body).empty());
    }
}
