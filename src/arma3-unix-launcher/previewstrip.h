#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <QListView>
#include <QPixmap>
#include <QStandardItemModel>
#include <QTimer>
#include <QToolButton>
#include <QWidget>

class ModTableWidget;

// Horizontal strip of mod cards (thumbnail, name, enabled state) mirroring the visible rows of a
// ModTableWidget, with selection kept in sync both ways.
class PreviewStrip : public QWidget
{
        Q_OBJECT

    public:
        static constexpr int key_role = Qt::UserRole + 1;         // path_or_workshop_id of the mod
        static constexpr int is_workshop_role = Qt::UserRole + 2;
        static constexpr int pixmap_role = Qt::UserRole + 3;
        static constexpr int thumbnail_pixels = 128;

        explicit PreviewStrip(QWidget *parent = nullptr);

        void attach(ModTableWidget *table);

        void set_thumbnail(std::string const &workshop_id, QPixmap const &pixmap);

        // Thread-safe: loads the image on the calling thread and applies it on the UI thread.
        // Unreadable images are ignored.
        void deliver_thumbnail(std::uint64_t const workshop_id, std::filesystem::path const &image);

        void set_collapsed(bool collapsed);
        bool is_collapsed() const;

        // Applies pending table changes now instead of on the next event loop iteration
        void sync_now();

        QListView *view() const;
        int card_count() const;
        std::string card_key(int index) const;
        int current_card() const;
        QPixmap card_pixmap(int index) const;
        // Area of the "open Workshop page" button on a card in viewport coordinates, invalid if it has none
        QRect open_button_rect(QModelIndex const &index) const;
        static QPixmap placeholder();

    signals:
        void collapsed_changed(bool collapsed);
        // Workshop mods shown in the strip that have no thumbnail yet
        void thumbnails_needed(std::vector<std::uint64_t> workshop_ids);
        void workshop_page_requested(std::string workshop_id);

    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;

    private:
        void rebuild();
        void select_card_for_table_row();
        void select_table_row_for_card();

        ModTableWidget *table_ = nullptr;
        QToolButton *toggle_;
        QListView *view_;
        QStandardItemModel model_;
        QTimer rebuild_timer_;
        std::map<std::string, QPixmap> thumbnails_;
        std::set<std::uint64_t> requested_thumbnails_;
        bool syncing_selection_ = false;
};
