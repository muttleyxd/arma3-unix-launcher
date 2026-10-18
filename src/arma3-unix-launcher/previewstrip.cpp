#include "previewstrip.h"

#include <QEvent>
#include <QIcon>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "modtablewidget.h"

namespace
{
    constexpr int card_width = PreviewStrip::thumbnail_pixels + 24;
    constexpr int card_height = PreviewStrip::thumbnail_pixels + 40;

    class SelectionSyncGuard
    {
        public:
            explicit SelectionSyncGuard(bool &flag) : flag_(flag)
            {
                flag_ = true;
            }
            ~SelectionSyncGuard()
            {
                flag_ = false;
            }

        private:
            bool &flag_;
    };
}

PreviewStrip::PreviewStrip(QWidget *parent)
    : QWidget(parent)
    , toggle_(new QToolButton(this))
    , view_(new QListView(this))
{
    auto const layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    toggle_->setText("Previews");
    toggle_->setCheckable(true);
    toggle_->setChecked(true);
    toggle_->setAutoRaise(true);
    toggle_->setArrowType(Qt::DownArrow);
    toggle_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toggle_, &QToolButton::toggled, this, [this](bool expanded)
    {
        set_collapsed(!expanded);
    });
    layout->addWidget(toggle_);

    view_->setModel(&model_);
    view_->setViewMode(QListView::IconMode);
    view_->setFlow(QListView::LeftToRight);
    view_->setWrapping(false);
    view_->setMovement(QListView::Static);
    view_->setUniformItemSizes(true);
    view_->setIconSize(QSize(thumbnail_pixels, thumbnail_pixels));
    view_->setGridSize(QSize(card_width, card_height));
    view_->setTextElideMode(Qt::ElideRight);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view_->setDragEnabled(false);
    view_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setFixedHeight(card_height + view_->horizontalScrollBar()->sizeHint().height() + 2 * view_->frameWidth());
    view_->viewport()->installEventFilter(this);
    connect(view_->selectionModel(), &QItemSelectionModel::currentChanged, this, &PreviewStrip::select_table_row_for_card);
    layout->addWidget(view_);

    rebuild_timer_.setSingleShot(true);
    rebuild_timer_.setInterval(0);
    connect(&rebuild_timer_, &QTimer::timeout, this, &PreviewStrip::rebuild);
}

void PreviewStrip::attach(ModTableWidget *table)
{
    table_ = table;
    // Coalesce bursts of changes (e.g. loading hundreds of mods) into one rebuild
    connect(table_, &ModTableWidget::mods_changed, &rebuild_timer_, qOverload<>(&QTimer::start));
    connect(table_, &QTableWidget::itemSelectionChanged, this, &PreviewStrip::select_card_for_table_row);
    rebuild_timer_.start();
}

void PreviewStrip::set_thumbnail(std::string const &workshop_id, QPixmap const &pixmap)
{
    auto const scaled = pixmap.scaled(thumbnail_pixels, thumbnail_pixels, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    thumbnails_[workshop_id] = scaled;
    for (int row = 0; row < model_.rowCount(); ++row)
    {
        auto const item = model_.item(row);
        if (item->data(key_role).toString().toStdString() != workshop_id)
            continue;
        item->setIcon(QIcon(scaled));
        item->setData(scaled, pixmap_role);
    }
}

void PreviewStrip::set_collapsed(bool collapsed)
{
    if (collapsed == is_collapsed())
        return;
    view_->setVisible(!collapsed);
    {
        QSignalBlocker const blocker(toggle_);
        toggle_->setChecked(!collapsed);
    }
    toggle_->setArrowType(collapsed ? Qt::RightArrow : Qt::DownArrow);
    emit collapsed_changed(collapsed);
}

bool PreviewStrip::is_collapsed() const
{
    return view_->isHidden();
}

void PreviewStrip::sync_now()
{
    rebuild_timer_.stop();
    rebuild();
}

QListView *PreviewStrip::view() const
{
    return view_;
}

int PreviewStrip::card_count() const
{
    return model_.rowCount();
}

std::string PreviewStrip::card_key(int index) const
{
    return model_.item(index)->data(key_role).toString().toStdString();
}

int PreviewStrip::current_card() const
{
    return view_->currentIndex().isValid() ? view_->currentIndex().row() : -1;
}

QPixmap PreviewStrip::card_pixmap(int index) const
{
    return model_.item(index)->data(pixmap_role).value<QPixmap>();
}

QPixmap PreviewStrip::placeholder()
{
    static QPixmap const pixmap = []()
    {
        QPixmap result(thumbnail_pixels, thumbnail_pixels);
        result.fill(QColor(60, 60, 60));
        QPainter painter(&result);
        painter.setPen(QColor(170, 170, 170));
        painter.drawText(result.rect(), Qt::AlignCenter, "No preview");
        return result;
    }
    ();
    return pixmap;
}

bool PreviewStrip::eventFilter(QObject *watched, QEvent *event)
{
    // A mouse wheel only scrolls vertically, the strip only horizontally
    if (watched == view_->viewport() && event->type() == QEvent::Wheel)
    {
        auto const wheel = static_cast<QWheelEvent *>(event);
        auto const delta = wheel->angleDelta();
        if (std::abs(delta.y()) > std::abs(delta.x()))
        {
            auto const scroll_bar = view_->horizontalScrollBar();
            scroll_bar->setValue(scroll_bar->value() - delta.y());
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PreviewStrip::rebuild()
{
    if (!table_)
        return;

    std::vector<std::uint64_t> missing_thumbnails;
    {
        SelectionSyncGuard const guard(syncing_selection_);
        model_.clear();
        for (int row = 0; row < table_->rowCount(); ++row)
        {
            if (table_->isRowHidden(row))
                continue;
            auto const mod = table_->get_mod_at(row);
            auto const thumbnail = thumbnails_.find(mod.path_or_workshop_id);
            auto const pixmap = thumbnail != thumbnails_.end() ? thumbnail->second : placeholder();

            auto const item = new QStandardItem(QIcon(pixmap), QString::fromStdString(mod.name));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setCheckState(mod.enabled ? Qt::Checked : Qt::Unchecked);
            item->setToolTip(QString::fromStdString(mod.name + "\n" + mod.path_or_workshop_id));
            item->setData(QString::fromStdString(mod.path_or_workshop_id), key_role);
            item->setData(mod.is_workshop_mod, is_workshop_role);
            item->setData(pixmap, pixmap_role);
            model_.appendRow(item);

            bool is_number = false;
            auto const id = QString::fromStdString(mod.path_or_workshop_id).toULongLong(&is_number);
            if (mod.is_workshop_mod && is_number && thumbnail == thumbnails_.end() && requested_thumbnails_.insert(id).second)
                missing_thumbnails.push_back(id);
        }
    }
    select_card_for_table_row();

    if (!missing_thumbnails.empty())
        emit thumbnails_needed(missing_thumbnails);
}

void PreviewStrip::select_card_for_table_row()
{
    if (syncing_selection_ || !table_)
        return;
    SelectionSyncGuard const guard(syncing_selection_);

    auto const row = table_->currentRow();
    if (row < 0 || table_->isRowHidden(row) || table_->selectedItems().isEmpty())
    {
        view_->selectionModel()->clear();
        return;
    }
    auto const key = QString::fromStdString(table_->get_mod_at(row).path_or_workshop_id);
    for (int card = 0; card < model_.rowCount(); ++card)
    {
        if (model_.item(card)->data(key_role).toString() != key)
            continue;
        auto const index = model_.index(card, 0);
        view_->setCurrentIndex(index);
        view_->scrollTo(index, QAbstractItemView::PositionAtCenter);
        return;
    }
}

void PreviewStrip::select_table_row_for_card()
{
    if (syncing_selection_ || !table_ || !view_->currentIndex().isValid())
        return;
    SelectionSyncGuard const guard(syncing_selection_);

    auto const key = view_->currentIndex().data(key_role).toString().toStdString();
    for (int row = 0; row < table_->rowCount(); ++row)
    {
        if (table_->get_mod_at(row).path_or_workshop_id != key)
            continue;
        table_->selectRow(row);
        table_->scrollToItem(table_->item(row, 1));
        return;
    }
}
