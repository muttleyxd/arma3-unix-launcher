#pragma once

#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTimer>

#include <nlohmann/json.hpp>

#include <arma3client.hpp>

#include "settings.hpp"
#include "ui_mod.hpp"

namespace Ui
{
    class MainWindow;
}

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

        void on_checkbox_profile_stateChanged(int arg1);

        void on_button_parameter_file_open_clicked();

        void on_button_mod_preset_open_clicked();

        void on_button_mod_preset_save_clicked();

private:
        Ui::MainWindow *ui;
        QTimer arma_status_checker;

        std::unique_ptr<ARMA3::Client> client;

        std::filesystem::path config_file;
        Settings manager;

        void add_item(QTableWidget &table_widget, UiMod const &mod);
        void initialize_table_widget(QTableWidget &table_widget, QStringList const &column_names);

        std::vector<UiMod> get_mods(QTableWidget const &table_widget);
        UiMod get_mod_from_nth_row(QTableWidget const &table_widget, int row);
        void put_mods_from_ui_to_manager_settings();

        void checkbox_changed(int state);
};
