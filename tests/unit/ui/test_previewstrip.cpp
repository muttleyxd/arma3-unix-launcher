#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <QApplication>
#include <QCheckBox>
#include <QScrollBar>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QWheelEvent>

#include <thread>

#include "default_test_reporter.hpp"

#include "modtablewidget.h"
#include "previewstrip.h"

namespace
{
    void fill(ModTableWidget &table)
    {
        table.add_mod({true, "ACE3", "463939057", true});
        table.add_mod({false, "CBA_A3", "450814997", true});
        table.add_mod({true, "Local thing", "/mods/@xyz", false});
    }

    std::vector<std::string> keys(PreviewStrip const &strip)
    {
        std::vector<std::string> result;
        for (int i = 0; i < strip.card_count(); ++i)
            result.push_back(strip.card_key(i));
        return result;
    }

    QPixmap solid(QColor const color)
    {
        QPixmap pixmap(64, 64);
        pixmap.fill(color);
        return pixmap;
    }

    bool same_image(QPixmap const &a, QPixmap const &b)
    {
        return a.toImage() == b.toImage();
    }
}

TEST_CASE("Cards_MirrorVisibleTableRows")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    CHECK_EQ(keys(strip), (std::vector<std::string> {"463939057", "450814997", "/mods/@xyz"}));
    auto const model = strip.view()->model();
    CHECK_EQ(model->index(0, 0).data(Qt::DisplayRole).toString(), "ACE3");
    CHECK_EQ(model->index(0, 0).data(Qt::CheckStateRole).toInt(), Qt::Checked);
    CHECK_EQ(model->index(1, 0).data(Qt::CheckStateRole).toInt(), Qt::Unchecked);
    CHECK(model->index(0, 0).data(PreviewStrip::is_workshop_role).toBool());
    CHECK_FALSE(model->index(2, 0).data(PreviewStrip::is_workshop_role).toBool());
    // Cards only show the enabled state, toggling happens in the table
    CHECK_FALSE(model->flags(model->index(0, 0)).testFlag(Qt::ItemIsUserCheckable));

    table.set_filter("cba");
    strip.sync_now();
    CHECK_EQ(keys(strip), std::vector<std::string> {"450814997"});

    table.set_filter("");
    table.removeRow(0);
    strip.sync_now();
    CHECK_EQ(keys(strip), (std::vector<std::string> {"450814997", "/mods/@xyz"}));
}

TEST_CASE("Cards_FollowEnabledStateAndSorting")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    table.cellWidget(1, 0)->findChild<QCheckBox *>()->setCheckState(Qt::Checked);
    strip.sync_now();
    CHECK_EQ(strip.view()->model()->index(1, 0).data(Qt::CheckStateRole).toInt(), Qt::Checked);

    table.sortItems(1, Qt::DescendingOrder);
    strip.sync_now();
    CHECK_EQ(keys(strip), (std::vector<std::string> {"/mods/@xyz", "450814997", "463939057"}));
}

TEST_CASE("Changes_AreCoalescedUntilTheEventLoopRuns")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    CHECK_EQ(strip.card_count(), 0);

    QApplication::processEvents();
    CHECK_EQ(strip.card_count(), 3);
}

TEST_CASE("Thumbnails_PlaceholderUntilSet")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    CHECK(same_image(strip.card_pixmap(0), PreviewStrip::placeholder()));
    CHECK(same_image(strip.card_pixmap(2), PreviewStrip::placeholder()));

    strip.set_thumbnail("463939057", solid(Qt::red));
    CHECK_FALSE(same_image(strip.card_pixmap(0), PreviewStrip::placeholder()));
    CHECK(same_image(strip.card_pixmap(1), PreviewStrip::placeholder()));

    // Kept across rebuilds
    table.set_filter("ace");
    strip.sync_now();
    REQUIRE_EQ(strip.card_count(), 1);
    CHECK_FALSE(same_image(strip.card_pixmap(0), PreviewStrip::placeholder()));
}

TEST_CASE("Thumbnails_RequestedForWorkshopModsWithoutOne")
{
    ModTableWidget table;
    PreviewStrip strip;
    QSignalSpy needed(&strip, &PreviewStrip::thumbnails_needed);
    strip.attach(&table);
    strip.set_thumbnail("450814997", solid(Qt::blue));
    fill(table);
    strip.sync_now();

    REQUIRE_EQ(needed.count(), 1);
    auto const ids = needed.takeFirst().at(0).value<std::vector<std::uint64_t>>();
    CHECK_EQ(ids, std::vector<std::uint64_t> {463939057});

    // Not requested again for the same mods
    table.set_filter("a");
    strip.sync_now();
    CHECK_EQ(needed.count(), 0);
}

TEST_CASE("Selection_TableToStrip")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    table.selectRow(1);
    CHECK_EQ(strip.current_card(), 1);

    table.set_filter("local");
    strip.sync_now();
    table.selectRow(2);
    CHECK_EQ(strip.current_card(), 0);
}

TEST_CASE("Selection_StripToTable")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    strip.view()->setCurrentIndex(strip.view()->model()->index(2, 0));
    CHECK_EQ(table.currentRow(), 2);
    REQUIRE_EQ(table.selectionModel()->selectedRows().size(), 1);
    CHECK_EQ(table.selectionModel()->selectedRows().at(0).row(), 2);
    CHECK_EQ(strip.current_card(), 2);
}

TEST_CASE("Collapse")
{
    PreviewStrip strip;
    QSignalSpy changed(&strip, &PreviewStrip::collapsed_changed);
    CHECK_FALSE(strip.is_collapsed());

    strip.set_collapsed(true);
    CHECK(strip.is_collapsed());
    CHECK(strip.view()->isHidden());
    REQUIRE_EQ(changed.count(), 1);
    CHECK(changed.takeFirst().at(0).toBool());

    strip.set_collapsed(true);
    CHECK_EQ(changed.count(), 0);

    strip.set_collapsed(false);
    CHECK_FALSE(strip.view()->isHidden());
    CHECK_EQ(changed.count(), 1);
}

TEST_CASE("Layout_IsAHorizontalStrip")
{
    PreviewStrip strip;
    auto const view = strip.view();
    CHECK_EQ(view->flow(), QListView::LeftToRight);
    CHECK_FALSE(view->isWrapping());
    CHECK(view->uniformItemSizes());
    CHECK_EQ(view->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);
}

TEST_CASE("Wheel_ScrollsHorizontally")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    for (int i = 1; i <= 50; ++i)
        table.add_mod({false, "Mod " + std::to_string(i), std::to_string(i), true});
    strip.resize(400, strip.sizeHint().height());
    strip.show();
    strip.sync_now();
    QApplication::processEvents();

    auto const scroll_bar = strip.view()->horizontalScrollBar();
    REQUIRE_GT(scroll_bar->maximum(), 0);
    REQUIRE_EQ(scroll_bar->value(), 0);

    auto const viewport = strip.view()->viewport();
    QPointF const position(10, 10);
    QWheelEvent wheel(position, viewport->mapToGlobal(position), QPoint(), QPoint(0, -120), Qt::NoButton,
                      Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(viewport, &wheel);
    CHECK_GT(scroll_bar->value(), 0);
}

TEST_CASE("DeliverThumbnail_FromWorkerThread")
{
    QTemporaryDir directory;
    REQUIRE(directory.isValid());
    auto const image_path = directory.filePath("463939057.png").toStdString();
    REQUIRE(solid(Qt::green).save(QString::fromStdString(image_path)));

    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    std::thread worker([&strip, &image_path]()
    {
        strip.deliver_thumbnail(463939057, image_path);
        strip.deliver_thumbnail(450814997, "/nonexistent/image.png"); // unreadable: ignored
    });
    worker.join();
    // Applied on the UI thread
    CHECK(same_image(strip.card_pixmap(0), PreviewStrip::placeholder()));
    QApplication::processEvents();

    auto const delivered = strip.card_pixmap(0).toImage();
    REQUIRE_FALSE(delivered.isNull());
    CHECK_EQ(delivered.pixelColor(delivered.width() / 2, delivered.height() / 2), QColor(Qt::green));
    CHECK(same_image(strip.card_pixmap(1), PreviewStrip::placeholder()));
}

TEST_CASE("Thumbnail_ScaledForDevicePixelRatio")
{
    ModTableWidget table;
    PreviewStrip strip;
    strip.attach(&table);
    fill(table);
    strip.sync_now();

    QPixmap large(1024, 512);
    large.fill(Qt::red);
    strip.set_thumbnail("463939057", large);
    auto const pixmap = strip.card_pixmap(0);
    auto const ratio = strip.devicePixelRatioF();
    REQUIRE_EQ(ratio, doctest::Approx(2.0));
    CHECK_EQ(pixmap.devicePixelRatio(), doctest::Approx(ratio));
    CHECK_EQ(pixmap.width(), static_cast<int>(PreviewStrip::thumbnail_pixels * ratio));
    CHECK_EQ(pixmap.height(), static_cast<int>(PreviewStrip::thumbnail_pixels * ratio / 2));
}

TEST_CASE("OpenButton_OnWorkshopCardsOnly")
{
    ModTableWidget table;
    PreviewStrip strip;
    QSignalSpy requested(&strip, &PreviewStrip::workshop_page_requested);
    strip.attach(&table);
    fill(table);
    strip.resize(800, strip.sizeHint().height());
    strip.show();
    strip.sync_now();
    QApplication::processEvents();

    auto const model = strip.view()->model();
    auto const workshop_button = strip.open_button_rect(model->index(0, 0));
    REQUIRE(workshop_button.isValid());
    QTest::mouseClick(strip.view()->viewport(), Qt::LeftButton, Qt::NoModifier, workshop_button.center());
    REQUIRE_EQ(requested.count(), 1);
    CHECK_EQ(requested.takeFirst().at(0).value<std::string>(), "463939057");

    // Local mods have no Workshop page
    CHECK_FALSE(strip.open_button_rect(model->index(2, 0)).isValid());
    auto const local_card = strip.view()->visualRect(model->index(2, 0));
    QTest::mouseClick(strip.view()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      QPoint(local_card.right() - workshop_button.width() / 2, workshop_button.center().y()));
    CHECK_EQ(requested.count(), 0);

    // Clicking elsewhere on a card only selects it
    QTest::mouseClick(strip.view()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      strip.view()->visualRect(model->index(1, 0)).center());
    CHECK_EQ(requested.count(), 0);
    CHECK_EQ(table.currentRow(), 1);
}

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    qputenv("QT_SCALE_FACTOR", "2"); // Retina-like device pixel ratio
    QApplication application(argc, argv);
    qRegisterMetaType<std::vector<std::uint64_t>>();
    qRegisterMetaType<std::string>();

    doctest::Context context(argc, argv);
    return context.run();
}
