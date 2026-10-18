#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <chrono>

#include <QApplication>
#include <QScrollBar>

#include <fmt/format.h>

#include "default_test_reporter.hpp"

#include "modtablewidget.h"
#include "previewstrip.h"

// Responsiveness with a large mod list (spec: 500+ mods). Limits are generous so the test is not
// flaky on slow machines; the measured times are printed.

using namespace std::chrono;

namespace
{
    constexpr int mod_count = 500;

    template<typename Function>
    microseconds measure(std::string const &what, Function &&function)
    {
        auto const start = steady_clock::now();
        function();
        auto const elapsed = duration_cast<microseconds>(steady_clock::now() - start);
        fmt::print("{}: {:.2f} ms\n", what, elapsed.count() / 1000.0);
        return elapsed;
    }
}

TEST_CASE("FiveHundredMods_StayResponsive")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    strip.resize(1200, strip.sizeHint().height());
    strip.show();
    table.resize(1200, 600);
    table.show();

    auto const load = measure("add 500 mods", [&]()
    {
        for (int i = 0; i < mod_count; ++i)
            table.add_mod({i % 3 == 0, fmt::format("Mod {} {}", i, i % 2 ? "Medical" : "Vehicles"),
                           std::to_string(1000000 + i), true});
        QApplication::processEvents(); // includes the coalesced strip rebuild
    });
    CHECK_EQ(strip.card_count(), mod_count);
    CHECK_LT(load, seconds(5));

    // Every keystroke filters the table and rebuilds the strip
    for (auto const &[query, expected_cards] :
            std::vector<std::pair<char const *, int>> {{"m", 500}, {"me", 250}, {"med", 250}, {"medical", 250},
                {"medical 4", 70}, {"vehicles 49", 5}, {"", 500}
            })
    {
        auto const filtering = measure(fmt::format("filter '{}'", query), [&]()
        {
            table.set_filter(query);
            strip.sync_now();
            table.viewport()->repaint();
        });
        CAPTURE(query);
        CHECK_EQ(strip.card_count(), expected_cards);
        CHECK_LT(filtering, milliseconds(250));
    }

    auto const scrolling = measure("scroll strip end to end", [&]()
    {
        auto const scroll_bar = strip.view()->horizontalScrollBar();
        for (int value = 0; value <= scroll_bar->maximum(); value += 200)
        {
            scroll_bar->setValue(value);
            strip.view()->viewport()->repaint();
        }
    });
    CHECK_LT(scrolling, seconds(5));
}

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);

    doctest::Context context(argc, argv);
    return context.run();
}
