#include "settings.hpp"

#include <QCheckBox>
#include <QLineEdit>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "std_utils.hpp"

#include "ui_mainwindow.h"

namespace
{
    void create_default_config(std::filesystem::path const &config_file)
    {
        if (std::filesystem::exists(config_file))
            return;
        std::filesystem::create_directories(config_file.parent_path());
        StdUtils::CreateFile(config_file);

        nlohmann::json json = nlohmann::json::parse(R"json(
                                                    {
                                                        "mods": [],
                                                        "parameters": {
                                                            "checkSignatures": false,
                                                            "connect": null,
                                                            "cpuCount": -1,
                                                            "customParameters": null,
                                                            "dlcContact": false,
                                                            "dlcGlobalMobilization": false,
                                                            "dlcSogPrairieFire": false,
                                                            "dlcCSLA": false,
                                                            "dlcWesternSahara": false,
                                                            "enableHT": false,
                                                            "environmentVariables": null,
                                                            "exThreads": -1,
                                                            "filePatching": false,
                                                            "host": false,
                                                            "hugepages": false,
                                                            "name": null,
                                                            "noLogs": false,
                                                            "noPause": false,
                                                            "noSplash": false,
                                                            "par": null,
                                                            "password": null,
                                                            "port": null,
                                                            "skipIntro": false,
                                                            "window": false,
                                                            "world": null
                                                        },
                                                        "settings": {
                                                            "theme": "System"
                                                        }
                                                    })json");
        StdUtils::FileWriteAllText(config_file, json.dump(4));
    }
}

nlohmann::json convert_old_mod_format_to_new_format(nlohmann::json const &mods)
{
    constexpr char const *name_placeholder = "mod_imported_from_old_preset";

    nlohmann::json ret = nlohmann::json::array();
    for (auto const &workshop_mod : mods.at("workshop"))
        ret.push_back({{"path", workshop_mod.at("id")}, {"name", name_placeholder}, {"enabled", true}});
    for (auto const &custom_mod : mods.at("custom"))
        ret.push_back({{"path", custom_mod.at("path")}, {"name", name_placeholder}, {"enabled", custom_mod.at("enabled")}});
    return ret;
}

bool is_old_mod_format(nlohmann::json const &mods)
{
    return !mods.is_array();
}

Settings::Settings(std::filesystem::path const config_file_path) : config_file(config_file_path)
{
    create_default_config(config_file);
    try
    {
        settings = nlohmann::json::parse(StdUtils::FileReadAllText(config_file));

        if (is_old_mod_format(settings.at("mods")))
            settings["mods"] = convert_old_mod_format_to_new_format(settings["mods"]);
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("Error loading settings from {}:\n{}", config_file.string(), ex.what());
    }
}

std::string Settings::get_launch_parameters()
{
    std::string ret;
    for (auto const &parameter : settings["parameters"].items())
    {
        if (StringUtils::StartsWith(parameter.key(), "dlc") || StringUtils::StartsWith(parameter.key(), "proton"))
            continue;
        else if (parameter.key() == "customParameters" && parameter.value().is_string())
            ret += fmt::format(" {}", std::string(parameter.value()));
        else if (parameter.value().type() == nlohmann::json::value_t::boolean && parameter.value())
            ret += fmt::format(" -{}", parameter.key());
        else if (parameter.value().type() == nlohmann::json::value_t::string)
        {
            std::string value = parameter.value();
            ret += fmt::format(" -{}={}", parameter.key(), value);
        }
        else if ((parameter.value().type() == nlohmann::json::value_t::number_integer
                  || parameter.value().type() == nlohmann::json::value_t::number_unsigned) && parameter.value() != -1)
        {
            int value = parameter.value();
            ret += fmt::format(" -{}={}", parameter.key(), value);
        }
    }
    return ret;
}

void Settings::save_settings_to_disk()
{
    StdUtils::FileWriteAllText(config_file, settings.dump(4));
}

void Settings::load_settings_to_ui(Ui::MainWindow *ui)
{
    // basic tab
    read_setting("skipIntro", ui->checkbox_skip_intro);
    read_setting("noSplash", ui->checkbox_skip_logos_at_startup);
    read_setting("window", ui->checkbox_force_window_mode);
    read_setting("name", ui->checkbox_profile, ui->textbox_profile);
    read_setting("dlcContact", ui->checkbox_dlc_contact);
    read_setting("dlcGlobalMobilization", ui->checkbox_dlc_global_mobilization);
    read_setting("dlcSogPrairieFire", ui->checkbox_dlc_sog_prairie_fire);
    read_setting("dlcCSLA", ui->checkbox_dlc_csla);
    read_setting("dlcWesternSahara", ui->checkbox_dlc_western_sahara);
    read_setting("protonDisableEsync", ui->checkbox_disable_esync);

    // advanced tab
    read_setting("par", ui->checkbox_parameter_file, ui->textbox_parameter_file);
    read_setting("checkSignatures", ui->checkbox_check_signatures);
    read_setting("enableHT", ui->checkbox_enable_hyper_threading);
    read_setting("filePatching", ui->checkbox_enable_file_patching);
    read_setting("noLogs", ui->checkbox_no_logs);
    read_setting("world", ui->checkbox_world, ui->textbox_world);
    read_setting("noPause", ui->checkbox_no_pause);
    read_setting("hugepages", ui->checkbox_hugepages);
    read_setting("customParameters", ui->checkbox_custom_parameters, ui->textbox_custom_parameters);
    read_setting("environmentVariables", ui->checkbox_environment_variables, ui->textbox_environment_variables);

    auto &parameters = settings["parameters"];
    try
    {
        int cpuCount = parameters.at("cpuCount");
        if (cpuCount != -1)
        {
            ui->checkbox_cpu_count->setChecked(true);
            ui->spinbox_cpu_count->setValue(cpuCount);
        }
        else
        {
            ui->checkbox_cpu_count->setChecked(false);
            ui->spinbox_cpu_count->setValue(4);
        }
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while parsing settings[\"cpuCount\"]:\n{}", ex.what());
    }
    try
    {
        int exThreads = parameters.at("exThreads");
        if (exThreads != -1)
        {
            ui->checkbox_extra_threads->setChecked(true);
            ui->checkbox_extra_threads_file_operations->setChecked(exThreads & 1);
            ui->checkbox_extra_threads_texture_loading->setChecked(exThreads & 2);
            ui->checkbox_extra_threads_geometry_loading->setChecked(exThreads & 4);
        }
        else
        {

            ui->checkbox_extra_threads->setChecked(false);
            ui->checkbox_extra_threads_file_operations->setChecked(false);
            ui->checkbox_extra_threads_texture_loading->setChecked(false);
            ui->checkbox_extra_threads_geometry_loading->setChecked(false);
        }
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while parsing settings[\"exThreads\"]:\n{}", ex.what());
    }

    // client tab
    read_setting("connect", ui->checkbox_server_address, ui->textbox_server_address);
    read_setting("port", ui->checkbox_server_port, ui->textbox_server_port);
    read_setting("password", ui->checkbox_server_password, ui->textbox_server_password);
    read_setting("host", ui->checkbox_host_session);

    // other tab
    read_setting("theme", ui->combobox_theme);
}

void Settings::save_settings_from_ui(Ui::MainWindow *ui)
{
    auto &parameters = settings["parameters"];

    // basic tab
    write_setting("skipIntro", ui->checkbox_skip_intro);
    write_setting("noSplash", ui->checkbox_skip_logos_at_startup);
    write_setting("window", ui->checkbox_force_window_mode);
    write_setting("name", ui->checkbox_profile, ui->textbox_profile);
    write_setting("dlcContact", ui->checkbox_dlc_contact);
    write_setting("dlcGlobalMobilization", ui->checkbox_dlc_global_mobilization);
    write_setting("dlcSogPrairieFire", ui->checkbox_dlc_sog_prairie_fire);
    write_setting("dlcCSLA", ui->checkbox_dlc_csla);
    write_setting("dlcWesternSahara", ui->checkbox_dlc_western_sahara);
    write_setting("protonDisableEsync", ui->checkbox_disable_esync);

    // advanced tab
    write_setting("par", ui->checkbox_parameter_file, ui->textbox_parameter_file);
    write_setting("checkSignatures", ui->checkbox_check_signatures);
    write_setting("enableHT", ui->checkbox_enable_hyper_threading);
    write_setting("filePatching", ui->checkbox_enable_file_patching);
    write_setting("noLogs", ui->checkbox_no_logs);
    write_setting("world", ui->checkbox_world, ui->textbox_world);
    write_setting("noPause", ui->checkbox_no_pause);
    write_setting("hugepages", ui->checkbox_hugepages);
    write_setting("customParameters", ui->checkbox_custom_parameters, ui->textbox_custom_parameters);
    write_setting("environmentVariables", ui->checkbox_environment_variables, ui->textbox_environment_variables);

    parameters["cpuCount"] = -1;
    if (ui->checkbox_cpu_count->isChecked())
        parameters["cpuCount"] = ui->spinbox_cpu_count->value();

    parameters["exThreads"] = -1;
    if (ui->checkbox_extra_threads->isChecked())
    {
        int value = 0;
        value += ui->checkbox_extra_threads_file_operations->isChecked() * 1;
        value += ui->checkbox_extra_threads_texture_loading->isChecked() * 2;
        value += ui->checkbox_extra_threads_geometry_loading->isChecked() * 4;
        parameters["exThreads"] = value;
    }

    // client tab
    write_setting("connect", ui->checkbox_server_address, ui->textbox_server_address);
    write_setting("port", ui->checkbox_server_password, ui->textbox_server_port);
    write_setting("password", ui->checkbox_server_password, ui->textbox_server_password);
    write_setting("host", ui->checkbox_host_session);

    // other tab
    write_setting("theme", ui->combobox_theme);
}

void Settings::read_setting(char const *const setting_name, QComboBox *combobox)
{
    auto const &json_settings = settings["settings"];
    try
    {
        combobox->setCurrentText(QString::fromStdString(json_settings[setting_name]));
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while parsing settings[\"{}\"]:\n{}", setting_name, ex.what());
    }
}

void Settings::read_setting(char const *const setting_name, QCheckBox *checkbox, QLineEdit *textbox)
{
    auto const &parameters = settings["parameters"];
    try
    {
        if (textbox != nullptr)
        {
            if (parameters.at(setting_name).is_null())
            {
                checkbox->setChecked(false);
                textbox->setText("");

            }
            else
            {
                checkbox->setChecked(true);
                textbox->setText(QString::fromStdString(parameters[setting_name]));
            }
        }
        else
            checkbox->setChecked(parameters.at(setting_name));
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while parsing settings[\"{}\"]:\n{}", setting_name, ex.what());
    }
}

void Settings::write_setting(char const *const setting_name, QComboBox *combobox)
{
    try
    {
        auto &json_settings = settings["settings"];
        json_settings[setting_name] = combobox->currentText().toStdString();
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while saving settings[\"{}\"]:\n{}", setting_name, ex.what());
    }
}

void Settings::write_setting(char const *const setting_name, QCheckBox *checkbox, QLineEdit *textbox)
{
    try
    {
        auto &parameters = settings["parameters"];
        if (textbox != nullptr)
        {
            parameters[setting_name] = nlohmann::json();
            if (checkbox->isChecked())
                parameters[setting_name] = textbox->text().toStdString();
        }
        else
            parameters[setting_name] = checkbox->isChecked();
    }
    catch (std::exception const &ex)
    {
        spdlog::warn("exception while saving settings[\"{}\"]:\n{}", setting_name, ex.what());
    }
}
