#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <QApplication>
#include <QCheckBox>

#include "default_test_reporter.hpp"

#include "modtablewidget.h"

namespace
{
    std::vector<std::string> visible_names(ModTableWidget const &table)
    {
        std::vector<std::string> names;
        for (int row = 0; row < table.rowCount(); ++row)
            if (!table.isRowHidden(row))
                names.push_back(table.get_mod_at(row).name);
        return names;
    }

    void fill(ModTableWidget &table)
    {
        table.add_mod({true, "ACE3 Medical", "463939057", true});
        table.add_mod({false, "CBA_A3", "450814997", true});
        table.add_mod({true, "RHSAFRF", "843425103", true});
        table.add_mod({true, "Local thing", "/mods/@xyz", false});
    }
}

TEST_CASE("Filter_HidesNonMatchingRows")
{
    ModTableWidget table;
    fill(table);

    table.set_filter("ace med");
    CHECK(table.is_filtering());
    CHECK_EQ(visible_names(table), std::vector<std::string> {"ACE3 Medical"});

    table.set_filter("a3");
    CHECK_EQ(visible_names(table), std::vector<std::string> {"CBA_A3"});

    table.set_filter("C");
    CHECK_EQ(visible_names(table), (std::vector<std::string> {"ACE3 Medical", "CBA_A3", "Local thing"}));

    table.set_filter("");
    CHECK_FALSE(table.is_filtering());
    CHECK_EQ(visible_names(table).size(), 4);
}

TEST_CASE("Filter_MatchesWorkshopIdButNotLocalPaths")
{
    ModTableWidget table;
    fill(table);

    table.set_filter("8434");
    CHECK_EQ(visible_names(table), std::vector<std::string> {"RHSAFRF"});

    table.set_filter("xyz");
    CHECK(visible_names(table).empty());

    table.set_filter("local");
    CHECK_EQ(visible_names(table), std::vector<std::string> {"Local thing"});
}

TEST_CASE("Filter_AppliesToModsAddedLater")
{
    ModTableWidget table;
    fill(table);
    table.set_filter("ace");

    table.add_mod({false, "ACE3 Arsenal", "1", true});
    table.add_mod({false, "Something else", "2", true}, 0);
    CHECK_EQ(visible_names(table), (std::vector<std::string> {"ACE3 Medical", "ACE3 Arsenal"}));
}

TEST_CASE("Filter_SurvivesSorting")
{
    ModTableWidget table;
    fill(table);
    table.set_filter("rhs");

    table.sortItems(1, Qt::DescendingOrder);
    CHECK_EQ(visible_names(table), std::vector<std::string> {"RHSAFRF"});
    table.sortItems(1, Qt::AscendingOrder);
    CHECK_EQ(visible_names(table), std::vector<std::string> {"RHSAFRF"});
}

TEST_CASE("Filter_DisablesDragAndDrop")
{
    ModTableWidget table;
    fill(table);
    REQUIRE_EQ(table.dragDropMode(), QAbstractItemView::InternalMove);

    table.set_filter("ace");
    CHECK_EQ(table.dragDropMode(), QAbstractItemView::NoDragDrop);
    CHECK_FALSE(table.dragEnabled());

    table.set_filter("  ");
    CHECK_EQ(table.dragDropMode(), QAbstractItemView::InternalMove);
    CHECK(table.dragEnabled());
}

TEST_CASE("Filter_DoesNotChangeEnabledStateOrCounters")
{
    ModTableWidget table;
    int workshop = -1;
    int custom = -1;
    table.set_mod_counter_callback([&](int w, int c)
    {
        workshop = w;
        custom = c;
    });
    fill(table);

    table.set_filter("cba");
    table.update_mod_selection_counters();
    CHECK_EQ(workshop, 2);
    CHECK_EQ(custom, 1);

    auto const mods = table.get_mods();
    REQUIRE_EQ(mods.size(), 4);
    CHECK(mods[0].enabled);
    CHECK_FALSE(mods[1].enabled);
}

TEST_CASE("CurrentWorkshopId")
{
    ModTableWidget table;
    fill(table);
    CHECK_FALSE(table.current_workshop_id().has_value());

    table.selectRow(2);
    CHECK_EQ(table.current_workshop_id(), "843425103");

    table.selectRow(3); // local mod
    CHECK_FALSE(table.current_workshop_id().has_value());

    table.selectRow(0);
    table.set_filter("cba"); // selected row hidden
    CHECK_FALSE(table.current_workshop_id().has_value());
}

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);

    doctest::Context context(argc, argv);
    return context.run();
}
