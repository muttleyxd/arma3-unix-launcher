#include "modtablewidget.h"
#include "tablewidgetcheckboxitem.h"

#include <algorithm>
#include <set>

#include <fmt/format.h>
#include <QCheckBox>
#include <QDropEvent>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QTableWidgetItem>

namespace
{
    void setup(QTableWidget &w)
    {
        w.setDragEnabled(true);
        w.setAcceptDrops(true);
        w.viewport()->setAcceptDrops(true);
        w.setDragDropOverwriteMode(false);
        w.setDropIndicatorShown(true);

        w.setSortingEnabled(true);

        w.setSelectionMode(QAbstractItemView::ExtendedSelection);
        w.setSelectionBehavior(QAbstractItemView::SelectRows);
        w.setDragDropMode(QAbstractItemView::InternalMove);

        QStringList const column_labels = {"ON", "NAME", "WORKSHOP ID / PATH", "SOURCE"};
        w.setColumnCount(column_labels.size());
        w.setHorizontalHeaderLabels(column_labels);
        w.horizontalHeader()->setVisible(true);
        w.horizontalHeader()->setHighlightSections(false);
        w.horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        w.horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        w.horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        w.horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
        w.horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        w.setColumnWidth(0, 52);
        w.setColumnWidth(2, 160);

        w.verticalHeader()->setVisible(false);
        w.verticalHeader()->setDefaultSectionSize(42);
        w.setShowGrid(false);
        w.setWordWrap(false);
        w.setTextElideMode(Qt::ElideMiddle);
    }
}

ModTableWidget::ModTableWidget(QWidget *parent) : QTableWidget(parent)
{
    setup(*this);
}

ModTableWidget::ModTableWidget(int rows, int columns, QWidget *parent) : QTableWidget(rows, columns, parent)
{
    setup(*this);
}

void ModTableWidget::dropEvent(QDropEvent *event) // https://stackoverflow.com/a/43789304
{
    if (!event->isAccepted() && event->source() == this)
    {
        std::set<int> rows;
        for (auto const index : selectedIndexes().toVector())
            rows.insert(index.row());

        std::vector<UiMod> rows_to_move;
        for (auto const row_index : rows)
            rows_to_move.push_back(get_mod_at(row_index));

        auto drop_row = drop_on(event);
        for (auto it = rows.rbegin(); it != rows.rend(); ++it)
        {
            removeRow(*it);
            if (*it < drop_row)
                --drop_row;
        }

        for (auto it = rows_to_move.rbegin(); it != rows_to_move.rend(); ++it)
        {
            add_mod(*it, drop_row);
        }
        event->accept();

        for (std::size_t i = 0; i < rows_to_move.size(); ++i)
        {
            this->selectionModel()->select(this->model()->index(drop_row + i, 0), QItemSelectionModel::Select);
            for (int j = 1; j < columnCount(); ++j)
                item(drop_row + i, j)->setSelected(true);
        }
    }
    QTableWidget::dropEvent(event);
}

int ModTableWidget::drop_on(QDropEvent *event)
{
    auto const index = indexAt(event->pos());
    if (!index.isValid())
        return rowCount();

    if (is_below(event->pos(), index))
        return index.row() + 1;
    return index.row();
}

bool ModTableWidget::is_below(QPoint const &pos, QModelIndex const &index)
{
    auto const rect = visualRect(index);
    auto const margin = 2;
    if (pos.y() - rect.top() < margin)
        return false;
    if (rect.bottom() - pos.y() < margin)
        return true;
    return rect.contains(pos, true) && !(this->model()->flags(index) & Qt::ItemIsDropEnabled)
           && pos.y() >= rect.center().y();
}

void ModTableWidget::update_mod_selection_counters(int)
{
    if (!mod_counter_callback)
        return;

    int count_workshop = 0;
    int count_custom = 0;

    for (auto const &mod : get_mods())
    {
        if (mod.enabled)
        {
            if (mod.is_workshop_mod)
                ++count_workshop;
            else
                ++count_custom;
        }
    }

    mod_counter_callback(count_workshop, count_custom);
}

void ModTableWidget::add_mod(UiMod const &mod, int index)
{
    if (index == -1)
        index = rowCount();
    insertRow(index);

    QWidget *checkbox_widget = new QWidget();
    QHBoxLayout *checkbox_layout = new QHBoxLayout(checkbox_widget);
    QCheckBox *checkbox = new QCheckBox();

    checkbox_layout->addWidget(checkbox);
    checkbox_layout->setAlignment(Qt::AlignCenter);
    checkbox_layout->setContentsMargins(0, 0, 0, 0);

    checkbox_widget->setLayout(checkbox_layout);

    if (mod.enabled)
        checkbox->setCheckState(Qt::Checked);
    else
        checkbox->setCheckState(Qt::Unchecked);

    setCellWidget(index, 0, checkbox_widget);

    //kinda hacky, but least code changes... (this is a hidden item)
    auto checkbox_sort = new TableWidgetCheckboxItem(mod.enabled);
    setItem(index, 0, checkbox_sort);

    setItem(index, 1, new QTableWidgetItem(mod.name.c_str()));
    setItem(index, 2, new QTableWidgetItem(mod.path_or_workshop_id.c_str()));
    auto source_item = new QTableWidgetItem(mod.is_workshop_mod ? "Workshop" : "Local");
    source_item->setData(Qt::UserRole, mod.is_workshop_mod);
    setItem(index, 3, source_item);

    //kinda hacky again, but QTableWidgetItem isn't a QObject, so we cannot connect to it
    QObject::connect(checkbox, &QCheckBox::stateChanged, this, [this, checkbox_sort](int c)
    {
        checkbox_sort->setModEnabled(c);
        apply_filter();
    });

    QObject::connect(checkbox, &QCheckBox::stateChanged, this, &ModTableWidget::update_mod_selection_counters);

    for (int i = 1; i < columnCount(); ++i)
    {
        auto current_item = item(index, i);
        current_item->setFlags(current_item->flags() ^ Qt::ItemIsEditable);
        current_item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    item(index, 2)->setToolTip(QString::fromStdString(mod.path_or_workshop_id));

    setRowHidden(index, !row_matches_filter(index));
}

bool ModTableWidget::contains_mod(std::string_view const path_or_workshop_id) const
{
    for (int i = 0; i < rowCount(); ++i)
    {
        auto const mod = get_mod_at(i);
        if (mod.path_or_workshop_id == path_or_workshop_id)
            return true;
    }
    return false;
}

void ModTableWidget::disable_all_mods()
{
    for (int row = 0; row < rowCount(); ++row)
    {
        auto cell_widget = cellWidget(row, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();
        checkbox->setCheckState(Qt::CheckState::Unchecked);
    }
}

void ModTableWidget::enable_all_mods()
{
    for (int row = 0; row < rowCount(); ++row)
    {
        auto cell_widget = cellWidget(row, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();
        checkbox->setCheckState(Qt::CheckState::Checked);
    }
}

UiMod ModTableWidget::get_mod_at(int index) const
{
    auto cell_widget = cellWidget(index, 0);
    auto checkbox = cell_widget->findChild<QCheckBox *>();

    bool enabled = checkbox->checkState() == Qt::CheckState::Checked;
    std::string name = item(index, 1)->text().toStdString();
    std::string path_or_workshop_id = item(index, 2)->text().toStdString();
    bool is_workshop_mod = item(index, 3)->data(Qt::UserRole).toBool();
    return {enabled, name, path_or_workshop_id, is_workshop_mod};
}

bool ModTableWidget::is_row_complete(int index) const
{
    return cellWidget(index, 0) != nullptr && item(index, 1) != nullptr && item(index, 2) != nullptr
           && item(index, 3) != nullptr;
}

bool ModTableWidget::is_mod_enabled_at(int index) const
{
    auto cell_widget = cellWidget(index, 0);
    if (cell_widget == nullptr)
        return false;
    auto checkbox = cell_widget->findChild<QCheckBox *>();
    return checkbox != nullptr && checkbox->checkState() == Qt::CheckState::Checked;
}

void ModTableWidget::set_mod_enabled_at(int index, bool enabled)
{
    auto cell_widget = cellWidget(index, 0);
    if (cell_widget == nullptr)
        return;
    if (auto checkbox = cell_widget->findChild<QCheckBox *>())
        checkbox->setCheckState(enabled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

std::vector<UiMod> ModTableWidget::get_mods() const
{
    std::vector<UiMod> mods;

    for (int i = 0; i < rowCount(); ++i)
        mods.emplace_back(get_mod_at(i));
    return mods;
}

void ModTableWidget::set_mod_counter_callback(std::function<void (int, int)> &&callback)
{
    mod_counter_callback = std::move(callback);
}

void ModTableWidget::set_filter(QString const &search_text, Filter filter)
{
    filter_search_text = search_text.trimmed();
    filter_mode = filter;
    apply_filter();
}

void ModTableWidget::apply_filter()
{
    for (int row = 0; row < rowCount(); ++row)
        setRowHidden(row, !row_matches_filter(row));
}

bool ModTableWidget::row_matches_filter(int row) const
{
    if (!is_row_complete(row))
        return true;

    auto const enabled = is_mod_enabled_at(row);
    auto const is_workshop_mod = item(row, 3)->data(Qt::UserRole).toBool();

    switch (filter_mode)
    {
        case Filter::enabled:
            if (!enabled)
                return false;
            break;
        case Filter::disabled:
            if (enabled)
                return false;
            break;
        case Filter::workshop:
            if (!is_workshop_mod)
                return false;
            break;
        case Filter::local:
            if (is_workshop_mod)
                return false;
            break;
        case Filter::all:
            break;
    }

    if (filter_search_text.isEmpty())
        return true;

    return item(row, 1)->text().contains(filter_search_text, Qt::CaseInsensitive)
           || item(row, 2)->text().contains(filter_search_text, Qt::CaseInsensitive);
}
