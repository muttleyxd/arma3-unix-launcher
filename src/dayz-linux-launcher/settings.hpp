#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

namespace Ui
{
    class MainWindow;
}

class QCheckBox;
class QComboBox;
class QLineEdit;

nlohmann::json convert_old_mod_format_to_new_format(nlohmann::json const &mods);
bool is_old_mod_format(nlohmann::json const &mods);

class Settings
{
    public:
        explicit Settings(std::filesystem::path const config_file_path);

        nlohmann::json settings;

        std::string get_launch_parameters();

        void load_settings_to_ui(Ui::MainWindow *ui);
        void save_settings_from_ui(Ui::MainWindow *ui);
        void save_settings_to_disk();
    private:
        std::filesystem::path config_file;

        void read_setting(char const *const setting_name, QComboBox *combobox);
        void read_setting(char const *const setting_name, QCheckBox *checkbox, QLineEdit *textbox = nullptr);
        void write_setting(char const *const setting_name, QComboBox *combobox);
        void write_setting(char const *const setting_name, QCheckBox *checkbox, QLineEdit *textbox = nullptr);
};
