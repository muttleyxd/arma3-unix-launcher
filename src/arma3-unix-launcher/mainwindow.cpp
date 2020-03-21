#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Qt>

#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabBar>
#include <QTimer>

#include <QResizeEvent>

#include <iostream>
#include <optional>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "filesystem_utils.hpp"
#include "string_utils.hpp"
#include "ui_mod.hpp"

#include "std_utils.hpp"

#include "static_todo.hpp"

namespace fs = FilesystemUtils;

MainWindow::MainWindow(std::unique_ptr<ARMA3::Client> arma3_client, std::filesystem::path const &config_file_path,
                       QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    client(std::move(arma3_client)),
    config_file(config_file_path),
    manager(config_file_path)
{
    ui->setupUi(this);

    initialize_table_widget(*ui->table_workshop_mods, {"Enabled", "Name", "Workshop ID"});
    initialize_table_widget(*ui->table_custom_mods, {"Enabled", "Name", "Path"});
    ui->table_custom_mods->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    for (auto const &i : client->GetWorkshopMods())
    {
        auto is_mod_enabled = [&](std::string const & workshop_id)
        {
            for (auto const &mod : manager.settings["mods"]["workshop"])
                if (mod["id"] == workshop_id)
                    return true;
            return false;
        };

        auto mod_id = i.GetValueOrReturnDefault("publishedid", "cannot read id");
        add_item(*ui->table_workshop_mods, {is_mod_enabled(mod_id), i.GetValueOrReturnDefault("name", "cannot read name"),
                                            mod_id
                                           });
    }

    for (auto const &i : client->GetHomeMods())
    {
        std::string shortname = StringUtils::Replace(i.path_, client->GetPath().string(), "~arma");
        auto is_mod_enabled = [&](std::string const & shortname)
        {
            for (auto const &mod : manager.settings["mods"]["custom"])
                if (mod["path"] == shortname && mod["enabled"] == true)
                    return true;
            return false;
        };
        add_item(*ui->table_custom_mods, {is_mod_enabled(shortname),
                                          i.GetValueOrReturnDefault("name", "cannot read name"),
                                          shortname
                                         });
    }

    for (auto const &mod : manager.settings["mods"]["custom"])
    {
        if (StringUtils::StartsWith(mod["path"], "~arma"))
            continue;
        try
        {
            Mod i(mod["path"]);
            add_item(*ui->table_custom_mods, {mod["enabled"], i.GetValueOrReturnDefault("name", "cannot read name"), mod["path"]});
        }
        catch (std::exception const &ex)
        {
            fmt::print("{}(): Uncaught exception: {}\n", __FUNCTION__, ex.what());
        }
    }

    manager.load_settings_to_ui(ui);

    connect(&arma_status_checker, &QTimer::timeout, this, QOverload<>::of(&MainWindow::check_if_arma_is_running));
    arma_status_checker.start(2000);
}

MainWindow::~MainWindow()
{
    manager.save_settings_from_ui(ui);

    put_mods_from_ui_to_manager_settings();

    StdUtils::FileWriteAllText(config_file, manager.settings.dump(4));
    delete ui;
}

void MainWindow::on_button_start_clicked()
try
{
    manager.save_settings_from_ui(ui);

    auto parameters = manager.settings["parameters"];
    if (parameters["name"].is_string())
    {
        std::string profile = parameters["name"];
        if (StringUtils::trim(profile).empty())
            throw std::invalid_argument("Parameters -> Profile cannot be empty");
    }
    if (parameters["par"].is_string())
    {
        std::string parameter_file = parameters["par"];
        if (StringUtils::trim(parameter_file).empty())
            throw std::invalid_argument("Parameters -> Parameter file cannot be empty");
    }

    std::vector<std::string> workshop_mod_ids;
    for (auto const &mod : get_mods(*ui->table_workshop_mods))
        if (mod.enabled)
            workshop_mod_ids.push_back(mod.path_or_workshop_id);

    std::vector<std::filesystem::path> custom_mods;
    for (auto const &mod : get_mods(*ui->table_custom_mods))
        if (mod.enabled)
            custom_mods.push_back(StringUtils::Replace(mod.path_or_workshop_id, "~arma", client->GetPath()));

    client->CreateArmaCfg(workshop_mod_ids, custom_mods);
    client->Start(manager.get_launch_parameters());
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Information, "Cannot start game", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::add_item(QTableWidget &table_widget, UiMod const &mod)
{
    int id = table_widget.rowCount();
    table_widget.insertRow(id);

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

    table_widget.setCellWidget(id, 0, checkbox_widget);
    table_widget.setItem(id, 1, new QTableWidgetItem(mod.name.c_str()));
    table_widget.setItem(id, 2, new QTableWidgetItem(mod.path_or_workshop_id.c_str()));

    QObject::connect(checkbox, &QCheckBox::stateChanged, this, &MainWindow::checkbox_changed);

    for (int i = 1; i <= 2; ++i)
    {
        auto item = table_widget.item(id, i);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::initialize_table_widget(QTableWidget &table_widget, QStringList const &column_names)
{
    table_widget.clear();
    table_widget.setHorizontalHeaderLabels(column_names);
    table_widget.horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

std::vector<UiMod> MainWindow::get_mods(QTableWidget const &table_widget)
{
    std::vector<UiMod> mods;

    for (int i = 0; i < table_widget.rowCount(); ++i)
        mods.emplace_back(get_mod_from_nth_row(table_widget, i));
    return mods;
}

UiMod MainWindow::get_mod_from_nth_row(const QTableWidget &table_widget, int row)
{
    auto cell_widget = table_widget.cellWidget(row, 0);
    auto checkbox = cell_widget->findChild<QCheckBox *>();

    bool enabled = checkbox->checkState() == Qt::CheckState::Checked;
    std::string name = table_widget.item(row, 1)->text().toStdString();
    std::string path_or_workshop_id = table_widget.item(row, 2)->text().toStdString();
    return {enabled, name, path_or_workshop_id};
}

void MainWindow::put_mods_from_ui_to_manager_settings()
{
    auto &custom_mods = manager.settings["mods"]["custom"];
    custom_mods = nlohmann::json::array();

    for (auto const &mod : get_mods(*ui->table_custom_mods))
        custom_mods.push_back({{"enabled", mod.enabled}, {"path", mod.path_or_workshop_id}});

    auto &workshop_mods = manager.settings["mods"]["workshop"];
    workshop_mods = nlohmann::json::array();

    for (auto const &mod : get_mods(*ui->table_workshop_mods))
        if (mod.enabled)
            workshop_mods.push_back({{"enabled", true}, {"id", mod.path_or_workshop_id}});
}

void MainWindow::checkbox_changed(int)
{
    auto workshop_mods = get_mods(*ui->table_workshop_mods);
    auto custom_mods = get_mods(*ui->table_custom_mods);

    int count_workshop = 0;
    int count_custom = 0;

    for (auto const &mod : workshop_mods)
        if (mod.enabled)
            ++count_workshop;

    for (auto const &mod : custom_mods)
        if (mod.enabled)
            ++count_custom;

    auto text = fmt::format("Selected {} mods ({} from workshop, {} custom)", count_workshop + count_custom, count_workshop,
                            count_custom);
    ui->label_selected_mods->setText(QString::fromStdString(text));
}


std::unique_ptr<QFileDialog> get_open_dialog(QString const &title)
{
    TODO_BEFORE(04, 2020, "This already exists in arma_path_chooser_dialog.cpp");
    auto dialog = std::make_unique<QFileDialog>();
    dialog->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    dialog->setWindowTitle(title);
    dialog->setFileMode(QFileDialog::FileMode::Directory);
    dialog->setViewMode(QFileDialog::ViewMode::Detail);
    dialog->setOption(QFileDialog::DontResolveSymlinks, false);
    dialog->setOption(QFileDialog::ShowDirsOnly);
    dialog->setDirectory(QDir::homePath());
    return dialog;
}

void MainWindow::on_button_add_custom_mod_clicked()
try
{
    auto open_dir_dialog = get_open_dialog("Select directory with mod to add");
    if (!open_dir_dialog->exec())
        return;

    auto mod_dir_string = open_dir_dialog->selectedFiles()[0].toStdString();

    using std::filesystem::exists;
    if (!exists(mod_dir_string))
    {
        auto error_message = fmt::format("{} does not exist.", mod_dir_string);
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mod", QString::fromStdString(error_message)).exec();
        return;
    }

    auto mod_dir = std::filesystem::canonical(mod_dir_string);
    if (mod_dir == client->GetPath())
    {
        auto error_message = fmt::format("{} is Arma's main directory.", mod_dir);
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mod", QString::fromStdString(error_message)).exec();
        return;
    }

    auto dir_listing = fs::Ls(mod_dir, true);
    if (!StdUtils::Contains(dir_listing, "addons"))
    {
        auto error_message = fmt::format("{} does not exist.", mod_dir / "addons");
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mod", QString::fromStdString(error_message)).exec();
        return;
    }

    auto &settings_mods_custom = manager.settings["mods"]["custom"];
    auto existing_mod = std::find_if(settings_mods_custom.begin(),
                                     settings_mods_custom.end(), [&mod_dir](nlohmann::json const & item)
    {
        auto it = item.find("path");
        return it != item.end() && it.value() == mod_dir.string();
    });

    if (existing_mod != settings_mods_custom.end())
    {
        auto error_message = fmt::format("{} already exists.", mod_dir);
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mod", QString::fromStdString(error_message)).exec();
        return;
    }

    try
    {
        Mod m(mod_dir);
        add_item(*ui->table_custom_mods, UiMod{false, m.GetValueOrReturnDefault("name", "cannot read name"), mod_dir});
        settings_mods_custom.push_back({{"enabled", false}, {"path", mod_dir}});
    }
    catch (std::exception const &e)
    {
        auto error_message = fmt::format("{}.", e.what());
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mod", QString::fromStdString(error_message)).exec();
        return;
    }

}
catch (std::exception const &ex)
{
    fmt::print("{}(): Uncaught exception: {}\n", __FUNCTION__, ex.what());
}

void MainWindow::on_button_remove_custom_mod_clicked()
try
{
    auto selected_items = ui->table_custom_mods->selectedItems();
    if (selected_items.size() < 1)
    {
        QMessageBox(QMessageBox::Icon::Information, "Cannot remove custom mod", "Need to select a mod to remove").exec();
        return;
    }

    auto mod = get_mod_from_nth_row(*ui->table_custom_mods, selected_items[0]->row());
    if (StringUtils::StartsWith(mod.path_or_workshop_id, "~arma"))
    {
        QMessageBox(QMessageBox::Icon::Information, "Cannot remove custom mod",
                    "Selected mod is in home directory, delete its files manually").exec();
        return;
    }

    auto &settings_mods_custom = manager.settings["mods"]["custom"];
    for (auto it = settings_mods_custom.begin(); it < settings_mods_custom.end(); ++it)
    {
        auto &json_mod = *it;
        if (json_mod["path"] == mod.path_or_workshop_id)
            settings_mods_custom.erase(it);
    }

    ui->table_custom_mods->removeRow(selected_items[0]->row());
}
catch (std::exception const &ex)
{
    fmt::print("{}(): Uncaught exception: {}\n", __FUNCTION__, ex.what());
}

void MainWindow::check_if_arma_is_running()
{
    std::string text = "Status: Arma 3 is not running";
    for (auto const &executable_name : ARMA3::Definitions::executable_names)
        if (auto pid = StdUtils::IsProcessRunning(executable_name, true); pid != -1)
            text = fmt::format("Status: Arma 3 is running, PID: {}", pid);

    ui->label_arma_status->setText(QString::fromStdString(text));
}

void MainWindow::on_checkbox_extra_threads_stateChanged(int arg1)
{
    bool value = arg1 == Qt::CheckState::Checked;
    ui->checkbox_extra_threads_file_operations->setEnabled(value);
    ui->checkbox_extra_threads_texture_loading->setEnabled(value);
    ui->checkbox_extra_threads_geometry_loading->setEnabled(value);
}

void MainWindow::on_checkbox_cpu_count_stateChanged(int arg1)
{
    ui->spinbox_cpu_count->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_parameter_file_stateChanged(int arg1)
{
    bool value = arg1 == Qt::CheckState::Checked;
    ui->textbox_parameter_file->setEnabled(value);
    ui->button_parameter_file_open->setEnabled(value);
}

void MainWindow::on_checkbox_world_stateChanged(int arg1)
{
    ui->textbox_world->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_custom_parameters_stateChanged(int arg1)
{
    ui->textbox_custom_parameters->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_server_address_stateChanged(int arg1)
{
    ui->textbox_server_address->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_server_port_stateChanged(int arg1)
{
    ui->textbox_server_port->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_server_password_stateChanged(int arg1)
{
    ui->textbox_server_password->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_checkbox_profile_stateChanged(int arg1)
{
    ui->textbox_profile->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_button_parameter_file_open_clicked()
{
    auto open_parameter_file_dialog = get_open_dialog("Select parameter file");
    open_parameter_file_dialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    open_parameter_file_dialog->setOption(QFileDialog::ShowDirsOnly, false);
    if (!open_parameter_file_dialog->exec())
        return;

    auto param_file = open_parameter_file_dialog->selectedFiles()[0].toStdString();

    using std::filesystem::exists;
    if (!exists(param_file))
    {
        auto error_message = fmt::format("{} does not exist.", param_file);
        QMessageBox(QMessageBox::Icon::Information, "Cannot set parameter file", QString::fromStdString(error_message)).exec();
        return;
    }

    ui->textbox_parameter_file->setText(QString::fromStdString(param_file));
}

void MainWindow::on_button_mod_preset_open_clicked()
try
{
    auto config_dir = QString::fromStdString(config_file.parent_path().string());
    auto filename = QFileDialog::getOpenFileName(this, tr("Save mod preset"), config_dir,
                    tr("A3UL mod list | *.a3ulml (*.a3ulml)"));
    if (filename.isEmpty())
        return;

    nlohmann::json preset = nlohmann::json::parse(StdUtils::FileReadAllText(filename.toStdString()));
    auto &mods = preset.at("mods");

    auto const &workshop_mods = mods.at("workshop");
    auto workshop_contains_mod = [&](std::string id)
    {
        for (auto const &mod : workshop_mods)
            if (mod.at("id") == id)
                return true;
        return false;
    };

    for (int row = 0; row < ui->table_workshop_mods->rowCount(); ++row)
    {
        auto cell_widget = ui->table_workshop_mods->cellWidget(row, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();
        std::string workshop_id = ui->table_workshop_mods->item(row, 2)->text().toStdString();

        if (workshop_contains_mod(workshop_id))
            checkbox->setCheckState(Qt::CheckState::Checked);
        else
            checkbox->setCheckState(Qt::CheckState::Unchecked);
    }

    auto &custom_mods = mods.at("custom");
    for (int row = 0; row < ui->table_custom_mods->rowCount(); ++row)
    {
        auto cell_widget = ui->table_custom_mods->cellWidget(row, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();
        std::string path = ui->table_custom_mods->item(row, 2)->text().toStdString();

        auto custom_mod = std::find_if(custom_mods.begin(), custom_mods.end(), [&](nlohmann::json & mod)
        {
            return mod.at("path") == path;
        });

        if (custom_mod != custom_mods.end())
            (*custom_mod)["done"] = true;

        if (custom_mod != custom_mods.end() && custom_mod->at("enabled"))
            checkbox->setCheckState(Qt::CheckState::Checked);
        else
            checkbox->setCheckState(Qt::CheckState::Unchecked);
    }

    std::vector<std::pair<std::string, std::string>> failed_custom_mods;
    for (auto const &mod : custom_mods)
    {
        try
        {
            if (mod.contains("done"))
                continue;
            auto full_path = StringUtils::Replace(mod.at("path"), "~arma", client->GetPath().string());
            Mod m(full_path);
            UiMod ui_mod{mod.at("enabled"), m.GetValueOrReturnDefault("name", "cannot read name"), mod.at("path")};
            add_item(*ui->table_custom_mods, ui_mod);
        }
        catch (std::exception const &e)
        {
            failed_custom_mods.emplace_back(mod.at("path"), e.what());
        }
    }

    if (failed_custom_mods.size() > 0)
    {
        std::string message = "Mod import ok, failed importing custom mods:\n";
        for (auto const &failed_mod : failed_custom_mods)
            message += fmt::format("\nmod: {}, reason: {}", failed_mod.first, failed_mod.second);
        QMessageBox(QMessageBox::Icon::Warning, "Imported mod preset, issues found", QString::fromStdString(message)).exec();
        return;
    }
    put_mods_from_ui_to_manager_settings();
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot open mod preset", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::on_button_mod_preset_save_clicked()
try
{
    auto config_dir = QString::fromStdString(config_file.parent_path().string());
    auto filename = QFileDialog::getSaveFileName(this, tr("Save mod preset"), config_dir,
                    tr("A3UL mod list | *.a3ulml (*.a3ulml)"));
    if (filename.isEmpty())
        return;
    auto filename_str = filename.toStdString();
    if (!StringUtils::EndsWith(filename_str, ".a3ulml"))
        filename_str += ".a3ulml";

    put_mods_from_ui_to_manager_settings();
    nlohmann::json json;
    json["mods"] = manager.settings["mods"];
    StdUtils::FileWriteAllText(filename_str, json.dump(4));
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(error_message)).exec();
    return;
}