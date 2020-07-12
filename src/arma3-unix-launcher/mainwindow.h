#pragma once

#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTimer>

#include <nlohmann/json.hpp>

#include <arma3client.hpp>
#include <steam_integration.hpp>

#include "settings.hpp"
#include "ui_mod.hpp"

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(std::unique_ptr<ARMA3::Client> arma3_client, std::filesystem::path const &config_file_path,
                            QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_button_start_clicked();
        void on_button_add_custom_mod_clicked();
        void on_button_remove_custom_mod_clicked();

        void on_checkbox_extra_threads_stateChanged(int arg1);
        void on_checkbox_cpu_count_stateChanged(int arg1);
        void on_checkbox_parameter_file_stateChanged(int arg1);
        void on_checkbox_world_stateChanged(int arg1);
        void on_checkbox_custom_parameters_stateChanged(int arg1);
        void on_checkbox_server_address_stateChanged(int arg1);
        void on_checkbox_server_port_stateChanged(int arg1);
        void on_checkbox_server_password_stateChanged(int arg1);

        void check_if_arma_is_running();
        void check_steam_api();

        void on_checkbox_profile_stateChanged(int arg1);

        void on_button_parameter_file_open_clicked();

        void on_button_mod_preset_open_clicked();

        void on_button_mod_preset_save_clicked();

        void on_button_quit_clicked();

        void on_button_export_workshop_to_txt_clicked();

    private:
        Ui::MainWindow *ui;
        QTimer arma_status_checker;
        QTimer steam_api_checker;

        std::unique_ptr<ARMA3::Client> client;
        std::unique_ptr<Steam::Integration> steam_integration;

        std::filesystem::path config_file;
        Settings manager;

        std::vector<std::uint64_t> mods_to_enable;

        void add_item(QTableWidget &table_widget, UiMod const &mod);
        void initialize_table_widget(QTableWidget &table_widget, QStringList const &column_names);

        void load_mods_from_json(nlohmann::json &preset);
        void load_mods_from_html(std::string const &path);

        std::vector<UiMod> get_mods(QTableWidget const &table_widget);
        UiMod get_mod_from_nth_row(QTableWidget const &table_widget, int row);
        void put_mods_from_ui_to_manager_settings();

        void update_mod_selection_counters(int state = 0);

        void setup_steam_integration();
        void on_workshop_mod_installed(Steam::Structs::ItemDownloadedInfo const &info);
};
