#pragma once

#include <functional>
#include <vector>

#include <QTableWidget>

#include "ui_mod.hpp"

class ModTableWidget : public QTableWidget
{
    public:
        explicit ModTableWidget(QWidget *parent = nullptr);
        ModTableWidget(int rows, int columns, QWidget *parent = nullptr);
        ~ModTableWidget() override = default;

        void dropEvent(QDropEvent *event) override;

        void add_mod(UiMod const &mod, int index = -1);
        bool contains_mod(std::string_view const path_or_workshop_id) const;
        void disable_all_mods();
        void enable_all_mods();
        UiMod get_mod_at(int index) const;
        std::vector<UiMod> get_mods() const;

        void set_mod_counter_callback(std::function<void(int, int)> &&callback);
        void update_mod_selection_counters(int = 0);

        // Hides mods not matching query (see ModSearch::Matches); drag and drop is off while filtering
        void set_filter(QString const &query);
        bool is_filtering() const;

    private:
        int drop_on(QDropEvent *event);
        bool is_below(QPoint const &pos, QModelIndex const &index);

        void apply_filter(int row);
        void apply_filter_to_all_rows();

        std::function<void(int, int)> mod_counter_callback;
        QString filter;
};
