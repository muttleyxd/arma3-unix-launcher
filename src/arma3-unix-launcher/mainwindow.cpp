#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <Qt>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QListView>
#include <QMessageBox>
#include <QTabBar>
#include <QTimer>
#include <QTreeView>
#include <QUrl>

#include <QResizeEvent>

#include <array>
#include <iostream>
#include <optional>
#include <ranges>
#include <set>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <scope_guard.hpp>
#include <spdlog/spdlog.h>

#include "fmt_custom_formatters.hpp"
#include "filesystem_utils.hpp"
#include "html_preset_export.hpp"
#include "steam_integration_impl.hpp"
#include "string_utils.hpp"
#include "std_utils.hpp"
#include "ui_mod.hpp"
#include "update_checker.hpp"

#include "exceptions/preset_loading_failed.hpp"
#include "exceptions/steam_api_not_initialized.hpp"
#include "html_preset_parser.hpp"
#include "arma_path_chooser_dialog.h"

#ifndef REPOSITORY_VERSION
    #define REPOSITORY_VERSION 0
#endif

namespace fs = FilesystemUtils;

Q_DECLARE_METATYPE(std::string)

namespace
{
    constexpr char const *workshop_url = "https://steamcommunity.com/app/107410/workshop/";
    constexpr char const *workshop_subscriptions_url =
        "https://steamcommunity.com/my/myworkshopfiles/?appid=107410&browsefilter=mysubscriptions";
    constexpr char const *workshop_item_url_template = "https://steamcommunity.com/sharedfiles/filedetails/?id={}";

    std::string format_size(std::uintmax_t bytes)
    {
        constexpr double kib = 1024.0;
        auto const value = static_cast<double>(bytes);
        if (value >= kib * kib * kib)
            return fmt::format("{:.1f} GB", value / (kib * kib * kib));
        if (value >= kib * kib)
            return fmt::format("{:.0f} MB", value / (kib * kib));
        return fmt::format("{:.0f} KB", value / kib);
    }

    // Mod constructor throws when there is no addons directory, check it upfront without exceptions
    bool has_addons_directory(std::filesystem::path const &mod_path)
    {
        std::error_code ec;
        for (std::filesystem::directory_iterator it(mod_path, ec), end; !ec && it != end; it.increment(ec))
        {
            if (StringUtils::Lowercase(it->path().filename().string()) == "addons")
                return true;
        }
        return false;
    }
}

MainWindow::MainWindow(std::unique_ptr<ARMA3::Client> arma3_client, std::filesystem::path const &config_file_path,
                       bool use_steam_integration, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    client(std::move(arma3_client)),
    config_file(config_file_path),
    manager(config_file_path)
{
    if (use_steam_integration)
        steam_integration = std::make_unique<Steam::SteamIntegration>(ARMA3::Definitions::app_id);
    else
        steam_integration = std::make_unique<Steam::IntegrationStub>(ARMA3::Definitions::app_id);

    qRegisterMetaType<std::string>("std::string");

    ui->setupUi(this);
    ui->table_mods->set_mod_counter_callback([&](int workshop_mod_count, int custom_mod_count)
    {
        this->update_mod_selection_counters(workshop_mod_count, custom_mod_count);
    });

    setWindowIcon(QIcon(":/icons/blagoicons/arma3.png"));
    ui->label_arma_path->setText(QString::fromStdString(client->GetPath().string()));

    initialize_theme_combobox();

    if (REPOSITORY_VERSION == 0)
        manager.settings["settings"]["checkForUpdates"] = false;

    if (manager.settings["settings"]["checkForUpdates"] == nullptr)
    {
        auto const message = R"(Do you want to check for update availability on launcher's start?

If yes then launcher on every start will connect to api.github.com and check if there's a new version available.

If there is an update, then you will get a notification, nothing will be downloaded or installed)";
        auto const result = QMessageBox(QMessageBox::Icon::Question, "Update notifications", QString::fromStdString(message),
                                        QMessageBox::Yes | QMessageBox::No).exec();
        manager.settings["settings"]["checkForUpdates"] = result == QMessageBox::Yes;
    }

    if (manager.settings["settings"]["checkForUpdates"])
    {
        update_notification_thread = UpdateChecker::is_update_available([&](bool is_there_a_new_version, std::string content)
        {
            QMetaObject::invokeMethod(this, "on_updateNotification", Qt::AutoConnection, Q_ARG(bool, is_there_a_new_version),
                                      Q_ARG(std::string, content));
        });
    }

    for (auto const &mod : manager.settings["mods"])
    {
        try
        {
            auto const m = get_mod(ui_path_to_full_path(mod["path"]));
            ui->table_mods->add_mod({mod["enabled"], m.GetName(), full_path_to_ui_path(mod["path"]), m.IsWorkshopMod(client->GetPathWorkshop())});
        }
        catch (std::exception const &e)
        {
            spdlog::warn("Error loading mod from config:\n---\n{}\n--- Reason: {}", mod.dump(2), e.what());
        }
    }

    spdlog::trace("trying to get workshop mods");
    for (auto const &mod : client->GetWorkshopMods())
    {
        auto mod_id = mod.GetValueOrReturnDefault("publishedid", "cannot read id");

        if (!ui->table_mods->contains_mod(mod_id))
            ui->table_mods->add_mod({false, mod.GetName(), mod_id, true});

        try
        {
            steam_integration->get_item_title(std::stoull(mod_id));
        }
        catch (SteamApiNotInitializedException const &)
        {
            spdlog::trace("exception SteamApiNotInitializedException while parsing mod {}", mod_id);
        }
        catch (std::exception const &e)
        {
            spdlog::debug("exception std::exception '{}' while parsing mod {}", e.what(), mod_id);
        }
    }
    for (auto const &i : client->GetHomeMods())
    {
        std::string shortname = full_path_to_ui_path(i.path_);
        if (!ui->table_mods->contains_mod(shortname))
            ui->table_mods->add_mod({false, i.GetValueOrReturnDefault("name", "cannot read name"), shortname, false});
    }

    manager.load_settings_to_ui(ui);
    ui->table_mods->update_mod_selection_counters();

    connect(&arma_status_checker, &QTimer::timeout, this, QOverload<>::of(&MainWindow::check_if_arma_is_running));
    arma_status_checker.start(2000);

    ui->label_workshop_status->setVisible(false);
    if (steam_integration->is_initialized())
        setup_steam_integration();

    ui->table_mods->addAction(ui->action_mods_enable_all);
    ui->table_mods->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    connect(ui->action_mods_enable_all, &QAction::triggered, this,
            &MainWindow::on_mods_enable_all_mods);

    ui->table_mods->addAction(ui->action_mods_disable_all);
    ui->table_mods->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    connect(ui->action_mods_disable_all, &QAction::triggered, this,
            &MainWindow::on_mods_disable_all_mods);

    auto separator = new QAction(this);
    separator->setSeparator(true);
    ui->table_mods->addAction(separator);
    ui->table_mods->addAction(ui->action_mod_open_workshop);
    ui->table_mods->addAction(ui->action_mod_open_folder);
    ui->table_mods->addAction(ui->action_mod_copy_id);

    menu_button_save.addAction(ui->action_mods_save_html);
    menu_button_save.addAction(ui->action_mods_save_json);
    ui->button_mod_preset_save->setMenu(&menu_button_save);

    connect(ui->table_mods, &QTableWidget::itemSelectionChanged, this, &MainWindow::update_mod_preview);

    ui->splitter_mods->setStretchFactor(0, 1);
    ui->splitter_mods->setStretchFactor(1, 0);
    ui->splitter_mods->setSizes({700, 320});

    setup_navigation();
    setup_mod_filters();
    setup_launch_command_preview();
    update_mod_preview();
}

MainWindow::~MainWindow()
{
    if (update_notification_thread.joinable())
        update_notification_thread.join();
    manager.save_settings_from_ui(ui);

    put_mods_from_ui_to_manager_settings();

    StdUtils::FileWriteAllText(config_file, manager.settings.dump(4));
    delete ui;
}

void MainWindow::on_button_start_clicked()
try
{
    spdlog::trace("{}:{}", __PRETTY_FUNCTION__, __LINE__);
    for (auto const &executable_name : ARMA3::Definitions::executable_names)
        if (auto pid = StdUtils::IsProcessRunning(executable_name, true); pid != -1)
            throw std::runtime_error("Arma is already running");

    manager.save_settings_from_ui(ui);

    auto const parameters = manager.settings["parameters"];
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
    if (parameters["profiles"].is_string())
    {
        std::string profiles_dir = parameters["profiles"];
        if (StringUtils::trim(profiles_dir).empty())
            throw std::invalid_argument("Parameters -> Profiles Directory cannot be empty");
    }

    std::vector<std::filesystem::path> mods;
    for (auto const &mod : ui->table_mods->get_mods())
    {
        if (!mod.enabled)
            continue;
        auto m = get_mod(ui_path_to_full_path(mod.path_or_workshop_id));
        mods.push_back(m.path_);
    }

    std::string environment_variables = "";

    if (parameters["dlcContact"])
        mods.push_back(client->GetPath() / "Contact");
    if (parameters["dlcGlobalMobilization"])
        mods.push_back(client->GetPath() / "GM");
    if (parameters["dlcSogPrairieFire"])
        mods.push_back(client->GetPath() / "vn");
    if (parameters["dlcReactionForces"])
        mods.push_back(client->GetPath() / "RF");
    if (parameters["dlcExpeditionaryForces"])
        mods.push_back(client->GetPath() / "EF");
    if (parameters["dlcCSLA"])
        mods.push_back(client->GetPath() / "CSLA");
    if (parameters["dlcSpearhead1944"])
        mods.push_back(client->GetPath() / "SPE");
    if (parameters["dlcWesternSahara"])
        mods.push_back(client->GetPath() / "WS");
    if (!parameters["environmentVariables"].is_null())
        environment_variables = parameters["environmentVariables"];

    spdlog::trace("Mod list: ");
    for (auto const &mod : mods)
        spdlog::trace("path: {}", mod.string());
    if (!parameters["profiles"].is_null())
    {
        std::string profiles = parameters["profiles"];
        std::filesystem::path profiles_dir = profiles + "/Users/steamuser/Arma3.cfg";
        client->CreateArmaCfg(mods, profiles_dir);
    }
    else
        client->CreateArmaCfg(mods);
    client->Start(manager.get_launch_parameters(), environment_variables, steam_integration->is_initialized(),
                  parameters["protonDisableEsync"]);
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Information, "Cannot start game", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::put_mods_from_ui_to_manager_settings()
{
    auto &mods = manager.settings["mods"];
    mods = nlohmann::json::array();

    for (auto const &mod : ui->table_mods->get_mods())
        mods.push_back({{"enabled", mod.enabled}, {"name", mod.name}, {"path", mod.path_or_workshop_id}});
}

void MainWindow::update_mod_selection_counters(int workshop_mod_count, int custom_mod_count)
{
    auto const mod_count = workshop_mod_count + custom_mod_count;
    auto text = fmt::format("{} {} SELECTED", mod_count, mod_count == 1 ? "MOD" : "MODS");
    ui->label_selected_mods->setText(QString::fromStdString(text));
    ui->label_selected_mods->setToolTip(QString::fromStdString(fmt::format("{} from Workshop, {} local",
                                        workshop_mod_count, custom_mod_count)));

    update_footer_details();
    update_mod_preview();
}

void MainWindow::setup_steam_integration()
{
    using Steam::Structs::ItemDownloadedInfo;
    steam_integration->set_item_downloaded_callback([this](ItemDownloadedInfo const & info)
    {
        this->on_workshop_mod_installed(info);
    });

    connect(&steam_api_checker, &QTimer::timeout, this, QOverload<>::of(&MainWindow::check_steam_api));
    steam_api_checker.start(1000);
}

void MainWindow::check_steam_api()
{
    steam_integration->poll_events();

    bool visible = false;

    auto const mods = steam_integration->get_subscribed_items();
    for (auto const &mod : mods)
    {
        auto mod_status = steam_integration->get_mod_status(mod);
        if (mod_status.downloading)
        {
            auto const download_info = steam_integration->get_download_info(mod);
            auto const percentage = static_cast<uint64_t>((static_cast<float>(download_info.bytes_downloaded) / static_cast<float>
                                    (download_info.bytes_total)) * 100);
            auto const mod_name = steam_integration->get_item_title(mod).substr(0, 40);
            auto const downloading_label = fmt::format("Downloading {} · {}%", mod_name, percentage);
            ui->label_workshop_status->setText(QString::fromStdString(downloading_label));
            visible = true;
            break;
        }
    }

    ui->label_workshop_status->setVisible(visible);
}

void MainWindow::on_workshop_mod_installed(Steam::Structs::ItemDownloadedInfo const &info)
{
    if (std::to_string(info.app_id) != ARMA3::Definitions::app_id)
        return;

    spdlog::debug("Workshop event - mod installed {}\n", info.workshop_id);
    try
    {
        auto const workshop_id = std::to_string(info.workshop_id);
        Mod mod(client->GetPathWorkshop() / workshop_id);

        if (ui->table_mods->contains_mod(workshop_id))
            return;

        auto const enabled = std::ranges::find(mods_to_enable, info.workshop_id) != mods_to_enable.end();
        ui->table_mods->add_mod({enabled, mod.GetValueOrReturnDefault("name", "cannot read name"), workshop_id, true});
    }
    catch (std::exception const &e)
    {
        spdlog::warn("Received workshop mod install event, exception: {}", e.what());
    }
}

void MainWindow::on_button_add_custom_mod_clicked()
try
{
    QFileDialog open_dir_dialog(nullptr, tr("Select mod directories to add"), QDir::homePath());
    open_dir_dialog.setFileMode(QFileDialog::FileMode::Directory);
    open_dir_dialog.setOption(QFileDialog::Option::ShowDirsOnly);
    open_dir_dialog.setOption(QFileDialog::Option::DontUseNativeDialog);
    if (auto list_view = open_dir_dialog.findChild<QListView *>("listView"))
        list_view->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    if (auto tree_view = open_dir_dialog.findChild<QTreeView *>())
        tree_view->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    if (!open_dir_dialog.exec())
        return;

    auto selected_dirs = open_dir_dialog.selectedFiles();
    if (selected_dirs.size() > 0 && selected_dirs[0] == open_dir_dialog.directory().path())
        selected_dirs.removeFirst();

    std::string failed_mods;
    for (auto const &dir : selected_dirs)
    {
        auto const mod_dir_string = dir.toStdString();

        using std::filesystem::exists;
        if (!exists(mod_dir_string))
        {
            failed_mods += fmt::format("{} does not exist.\n", mod_dir_string);
            continue;
        }

        auto mod_dir = std::filesystem::canonical(mod_dir_string);
        if (mod_dir == client->GetPath())
        {
            failed_mods += fmt::format("{} is Arma's main directory.\n", mod_dir);
            continue;
        }

        auto const workshop_path_str = client->GetPathWorkshop().string();
        if (mod_dir.string().find(workshop_path_str) != std::string::npos)
        {
            failed_mods += fmt::format("{} is a workshop mod.\n", mod_dir);
            continue;
        }

        auto dir_listing = fs::Ls(mod_dir, true);
        if (std::ranges::find(dir_listing, "addons") == dir_listing.end())
        {
            failed_mods += fmt::format("{} does not exist.\n", mod_dir / "addons");
            continue;
        }

        auto &settings_mods = manager.settings["mods"];
        auto existing_mod = std::find_if(settings_mods.begin(),
                                         settings_mods.end(), [&mod_dir](nlohmann::json const & item)
        {
            auto it = item.find("path");
            return it != item.end() && it.value() == mod_dir.string();
        });

        if (existing_mod != settings_mods.end())
        {
            failed_mods += fmt::format("{} already exists.\n", mod_dir);
            continue;
        }

        try
        {
            Mod m(mod_dir);
            ui->table_mods->add_mod(UiMod{false, m.GetName(), mod_dir, false});
            settings_mods.push_back({{"enabled", false}, {"path", mod_dir}});
        }
        catch (std::exception const &e)
        {
            failed_mods += fmt::format("{}.\n", e.what());
        }
    }

    if (failed_mods.length() > 0)
    {
        auto error_message = fmt::format("Failed when adding mods:\n{}", failed_mods);
        QMessageBox(QMessageBox::Icon::Information, "Cannot add mods", QString::fromStdString(error_message)).exec();
        return;
    }
}
catch (std::exception const &ex)
{
    spdlog::critical("{}(): Uncaught exception: {}", __FUNCTION__, ex.what());
}

void MainWindow::on_button_remove_custom_mod_clicked()
try
{
    std::set<int> selected_items;
    for (auto const index : ui->table_mods->selectedItems())
        selected_items.insert(index->row());

    if (selected_items.size() < 1)
    {
        QMessageBox(QMessageBox::Icon::Information, "Cannot remove mod", "Need to select a mod to remove").exec();
        return;
    }

    for (auto it = selected_items.rbegin(); it != selected_items.rend(); ++it)
    {
        auto const mod = ui->table_mods->get_mod_at(*it);
        if (mod.is_workshop_mod)
        {
            QMessageBox(QMessageBox::Icon::Information, "Cannot remove workshop mod",
                        "Selected mod is in workshop mod, please unsubscribe from it in Steam's options").exec();
            return;
        }
        else if (StringUtils::StartsWith(mod.path_or_workshop_id, "~arma"))
        {
            QMessageBox(QMessageBox::Icon::Information, "Cannot remove custom mod",
                        "Selected mod is in home directory, delete its files manually").exec();
            return;
        }

        auto &settings_mods_custom = manager.settings["mods"];
        auto const full_path = ui_path_to_full_path(mod.path_or_workshop_id);
        for (auto it = settings_mods_custom.begin(); it < settings_mods_custom.end(); ++it)
        {
            auto &json_mod = *it;
            if (json_mod["path"] == full_path)
                settings_mods_custom.erase(it);
        }

        ui->table_mods->removeRow(*it);
    }
}
catch (std::exception const &ex)
{
    spdlog::critical("{}(): Uncaught exception: {}", __FUNCTION__, ex.what());
}

void MainWindow::check_if_arma_is_running()
{
    std::string text = "Arma 3 is not running";
    for (auto const &executable_name : ARMA3::Definitions::executable_names)
        if (auto pid = StdUtils::IsProcessRunning(executable_name, true); pid != -1)
        {
            text = fmt::format("Arma 3 is running · PID {}", pid);
            break;
        }

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

void MainWindow::on_checkbox_profiles_stateChanged(int arg1)
{
    ui->textbox_profiles->setEnabled(arg1 == Qt::CheckState::Checked);
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

void MainWindow::on_checkbox_environment_variables_stateChanged(int arg1)
{
    ui->textbox_environment_variables->setEnabled(arg1 == Qt::CheckState::Checked);
}

void MainWindow::on_button_parameter_file_open_clicked()
{
    ArmaPathChooserDialog apcd;
    auto open_parameter_file_dialog = apcd.get_open_dialog("Select parameter file");
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
    // todo: loading mod preset should order loaded mods next to each other, so original load order gets preserved
    auto config_dir = QString::fromStdString(config_file.parent_path().string());
    auto filename = QFileDialog::getOpenFileName(this, tr("Mod preset"), config_dir,
                    tr("Mod preset files | *.a3ulml, *.html (*.a3ulml *.html);;A3UL mod list | *.a3ulml (*.a3ulml);;HTML preset | *.html (*.html)"));
    if (filename.isEmpty())
        return;

    ui->table_mods->setSortingEnabled(false); // todo: investigate why loading mods when sorting is enabled causes a crash
    auto guard = sg::make_scope_guard([&]()
    {
        ui->table_mods->setSortingEnabled(true);
    });
    ui->table_mods->disable_all_mods();

    bool loaded = false;
    std::string error_messages;
    try
    {
        nlohmann::json preset = nlohmann::json::parse(StdUtils::FileReadAllText(filename.toStdString()));
        load_mods_from_json(std::move(preset));
        loaded = true;
    }
    catch (std::exception const &e)
    {
        error_messages += fmt::format("JSON parser: {}\n", e.what());
    }

    if (!loaded)
    {
        try
        {
            load_mods_from_html(filename.toStdString());
            loaded = true;
        }
        catch (std::exception const &e)
        {
            error_messages += fmt::format("HTML parser: {}\n", e.what());
        }
    }

    if (!loaded)
        throw PresetLoadingFailedException(error_messages);

    ui->table_mods->update_mod_selection_counters();
    put_mods_from_ui_to_manager_settings();
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot open mod preset", QString::fromStdString(error_message)).exec();
    return;
}

struct FailedMod
{
    std::string path;
    std::string name;
    std::string reason;
};

void MainWindow::load_mods_from_json(nlohmann::json preset)
{
    auto &mods = preset.at("mods");
    if (is_old_mod_format(mods))
        mods = convert_old_mod_format_to_new_format(mods);

    auto find_mod = [&](std::string const & workshop_id_or_path)
    {
        auto full_path = ui_path_to_full_path(workshop_id_or_path);
        for (auto const &mod : mods)
            if (mod.at("path") == full_path)
                return mod;
        return nlohmann::json{};
    };

    for (int row = ui->table_mods->rowCount() - 1; row >= 0; --row)
    {
        auto const ui_mod = ui->table_mods->get_mod_at(row);
        auto const mod = find_mod(ui_mod.path_or_workshop_id);
        if (mod.empty())
            continue;

        ui->table_mods->removeRow(row);
    }

    auto const all_mods = ui->table_mods->get_mods();

    auto const workshop_path = client->GetPathWorkshop();
    std::vector<FailedMod> failed_mods;
    for (auto it = mods.rbegin(); it < mods.rend(); ++it)
    {
        std::string const name = it->at("name");
        std::string const path = it->at("path");
        try
        {
            Mod m = get_mod(path);
            ui->table_mods->add_mod(UiMod{it->at("enabled"), m.GetName(), full_path_to_ui_path(it->at("path")), m.IsWorkshopMod(workshop_path)},
                                    0);
            (*it)["done"] = true;
        }
        catch (std::exception const &e)
        {
            spdlog::warn("Error parsing json mod \"{}\" ({}): {}", name, path, e.what());
            failed_mods.push_back({path, name, e.what()});
        }
    }

    if (failed_mods.size() > 0)
    {
        std::string message = "Mod import ok, failed importing mods:\n";
        std::string mod_list = "";
        int mod_count = 0;
        int mod_count_overload = 0;
        constexpr int mod_count_max = 25;

        if (mod_count >= mod_count_max)
            mod_list += fmt::format("and {} more...\n", std::to_string(mod_count_overload));

        std::vector<std::uint64_t> workshop_mods;

        for (auto const &failed_mod : failed_mods)
        {
            message += fmt::format("\nmod: \"{}\" ({}), reason: {}", failed_mod.name, failed_mod.path, failed_mod.reason);

            char const *prefix = "Local";
            if (is_workshop_mod(failed_mod.path))
            {
                prefix = "Steam";
                workshop_mods.push_back(std::stoull(failed_mod.path));
            }

            if (mod_count >= mod_count_max)
                mod_count_overload++;
            else
            {
                mod_count++;
                mod_list += fmt::format("{}: \"{}\" ({})\n", prefix, failed_mod.name, failed_mod.path);
            }
        }

        if (steam_integration->is_initialized())
            propose_subscribing_to_mods(mod_list, workshop_mods);
        else
            QMessageBox(QMessageBox::Icon::Warning, "Imported mod preset, issues found", QString::fromStdString(message)).exec();
    }
}

void MainWindow::load_mods_from_html(std::string const &path)
{
    auto const content = StdUtils::FileReadAllText(path);
    auto const mods = Html::Preset::Parser::html_to_json(content);

    nlohmann::json existing_mods = nlohmann::json::array();
    nlohmann::json not_existing_local_mods = nlohmann::json::array();
    nlohmann::json not_existing_steam_mods = nlohmann::json::array();

    for (auto mod : mods)
    {
        try
        {
            get_mod(mod.at("path")); // will throw on failure
            existing_mods.push_back(mod);
        }
        catch (std::exception const &e)
        {
            spdlog::info("Found not existing mod, error: {}", e.what());
            if (is_workshop_mod(mod.at("path")))
                not_existing_steam_mods.push_back(mod);
            else
                not_existing_local_mods.push_back(mod);
        }
    }

    auto contains_mod = [&](std::string const & path_or_workshop_id)
    {
        for (auto const &json_mod : existing_mods)
            if (json_mod["path"] == ui_path_to_full_path(path_or_workshop_id))
                return true;
        return false;
    };

    for (int row = ui->table_mods->rowCount() - 1; row >= 0; --row)
    {
        auto const ui_mod = ui->table_mods->get_mod_at(row);
        if (contains_mod(ui_mod.path_or_workshop_id))
            ui->table_mods->removeRow(row);
    }

    for (auto it = existing_mods.crbegin(); it < existing_mods.crend(); ++it)
    {
        try
        {
            auto const path = it->at("path");
            auto const full_path = ui_path_to_full_path(path);

            Mod m = get_mod(path);
            ui->table_mods->add_mod({true, m.GetName(), full_path, m.IsWorkshopMod(client->GetPathWorkshop())}, 0);
        }
        catch (std::exception const &e)
        {
            spdlog::warn("Failed adding apparently existing mod '{}', reason: {}", it->at("path").get<std::string>(), e.what());
        }
    }

    if (not_existing_local_mods.empty() && not_existing_steam_mods.empty())
        return;

    std::string mod_list;
    int mod_count = 0;
    int mod_count_overload = 0;
    constexpr int mod_count_max = 25;
    for (auto const &json_mod : not_existing_steam_mods)
    {
        if (mod_count >= mod_count_max)
            mod_count_overload++;
        else
        {
            mod_list += fmt::format("Steam: {}\n", std::string(json_mod["name"]));
            mod_count++;
        }
    }
    for (auto const &json_mod : not_existing_local_mods)
    {
        if (mod_count >= mod_count_max)
            mod_count_overload++;
        else
        {
            mod_list += fmt::format("Local: {}\n", std::string(json_mod["name"]));
            mod_count++;
        }
    }

    if (mod_count >= mod_count_max)
        mod_list += fmt::format("and {} more...\n", std::to_string(mod_count_overload));


    if (steam_integration->is_initialized() && not_existing_steam_mods.size() > 0)
    {
        std::vector<std::uint64_t> workshop_mods;
        for (auto const &json_mod : not_existing_steam_mods)
        {
            if (json_mod["origin"] != "Steam")
                continue;
            auto workshop_id = std::stoull(std::string(json_mod["path"]));
            workshop_mods.push_back(std::move(workshop_id));
        }
        propose_subscribing_to_mods(mod_list, workshop_mods);
    }
    else
    {
        char const *steam_integration_message = "(Steam integration is disabled)";
        if (steam_integration->is_initialized())
            steam_integration_message = "";
        auto const message = fmt::format("Following mods cannot be loaded {}:\n{}", steam_integration_message, mod_list);
        QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(message)).exec();
        return;
    }
}

void MainWindow::propose_subscribing_to_mods(std::string const &mod_list_message,
        std::vector<std::uint64_t> const &workshop_mods)
{
    auto const message =
        fmt::format("Following mods cannot be loaded now:\n{}\nDo you want to subscribe to missing Steam mods now?",
                    mod_list_message);
    auto const result = QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(message),
                                    QMessageBox::Yes | QMessageBox::No).exec();
    if (result != QMessageBox::Yes)
        return;

    for (auto const &workshop_id : workshop_mods)
    {
        steam_integration->subscribe(workshop_id);
        mods_to_enable.push_back(workshop_id);
    }
}

void MainWindow::on_button_mod_preset_save_clicked()
try
{

}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::on_action_mods_save_json_triggered()
try
{
    auto const config_dir = QString::fromStdString(config_file.parent_path().string());
    auto const filename = QFileDialog::getSaveFileName(this, tr("Save mod preset"), config_dir,
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

void MainWindow::on_action_mods_save_html_triggered()
try
{
    auto const config_dir = QString::fromStdString(config_file.parent_path().string());
    auto const filename = QFileDialog::getSaveFileName(this, tr("Save mod preset"), config_dir,
                          tr("HTML mod list | *.html (*.html)"));
    if (filename.isEmpty())
        return;
    auto filename_str = filename.toStdString();
    if (!StringUtils::EndsWith(filename_str, ".html"))
        filename_str += ".html";

    put_mods_from_ui_to_manager_settings();

    std::vector<Mod> mods;
    auto const workshop_path = client->GetPathWorkshop();

    for (auto const &ui_mod : ui->table_mods->get_mods())
    {
        if (!ui_mod.enabled)
            continue;

        if (ui_mod.is_workshop_mod)
            mods.push_back(Mod(workshop_path / ui_mod.path_or_workshop_id));
        else
            mods.push_back(Mod(ui_path_to_full_path(ui_mod.path_or_workshop_id)));
    }

    std::filesystem::path const filename_as_path = filename_str;
    auto const preset = ARMA3::HtmlPresetExport::export_mods(filename_as_path.stem().string(), mods, workshop_path);
    StdUtils::FileWriteAllText(filename_as_path, preset);
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::on_updateNotification(bool is_there_a_new_version, std::string content)
{
    spdlog::info("{}:{} Update status - new version available: {}", __PRETTY_FUNCTION__, __LINE__, is_there_a_new_version);
    if (is_there_a_new_version == false)
        return;

    constexpr auto message_template = R"(There is a new version of dayz-linux-launcher available

Do you want to open {}?

Changelog:
{})";
    constexpr char const *url = "https://github.com/muttleyxd/arma3-unix-launcher/releases/latest";
    auto const message = fmt::format(message_template, url, content);
    auto const result = QMessageBox(QMessageBox::Icon::Question, "Update notification", QString::fromStdString(message),
                                    QMessageBox::Yes | QMessageBox::No).exec();
    if (result == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::on_button_quit_clicked()
{
    this->close();
}

void MainWindow::on_button_export_workshop_to_txt_clicked()
try
{
    auto config_dir = QString::fromStdString(config_file.parent_path().string());
    auto filename = QFileDialog::getSaveFileName(this, tr("Save workshop mods to .txt file"), config_dir,
                    tr("Workshop mod list | *.txt (*.txt)"));
    if (filename.isEmpty())
        return;
    auto filename_str = filename.toStdString();
    if (!StringUtils::EndsWith(filename_str, ".txt"))
        filename_str += ".txt";

    put_mods_from_ui_to_manager_settings();
    std::string output;

    for (auto const &mod : ui->table_mods->get_mods())
        if (mod.enabled && mod.is_workshop_mod)
            output += fmt::format("{} - https://steamcommunity.com/sharedfiles/filedetails/?id={}\n", mod.name,
                                  mod.path_or_workshop_id);
    StdUtils::FileWriteAllText(filename_str, output);
}
catch (std::exception const &e)
{
    auto error_message = fmt::format("{}.", e.what());
    QMessageBox(QMessageBox::Icon::Critical, "Cannot save mod preset", QString::fromStdString(error_message)).exec();
    return;
}

void MainWindow::on_mods_disable_all_mods()
{
    ui->table_mods->disable_all_mods();
    put_mods_from_ui_to_manager_settings();
}

void MainWindow::on_mods_enable_all_mods()
{
    ui->table_mods->enable_all_mods();
    put_mods_from_ui_to_manager_settings();
}

void MainWindow::initialize_theme_combobox()
{
    auto &theme_combobox = ui->combobox_theme;

    theme_combobox->addItem("System");

    auto const entries = QDir(":/stylesheet").entryList();

    for (auto const &entry : entries)
    {
        auto const theme_name = StringUtils::Replace(entry.toStdString(), ".stylesheet", "");
        theme_combobox->addItem(QString::fromStdString(theme_name));
    }
}

std::string MainWindow::ui_path_to_full_path(std::string ui_path) const
{
    return StringUtils::Replace(ui_path, "~arma", client->GetPath().string());
}

std::string MainWindow::full_path_to_ui_path(std::string ui_path) const
{
    return StringUtils::Replace(ui_path, client->GetPath().string(), "~arma");
}

bool MainWindow::is_workshop_mod(std::string const &path_or_workshop_id)
{
    return !path_or_workshop_id.empty()
           && std::all_of(path_or_workshop_id.begin(), path_or_workshop_id.end(), ::isdigit);
}

Mod MainWindow::get_mod(std::string const &path_or_workshop_id)
{
    if (is_workshop_mod(path_or_workshop_id))
    {
        std::filesystem::path const workshop_mod_path = client->GetPathWorkshop() / path_or_workshop_id;
        return Mod(workshop_mod_path);
    }
    return Mod(path_or_workshop_id);
}

void MainWindow::on_combobox_theme_currentTextChanged(QString const &combobox_theme)
{
    QString style;

    if (combobox_theme != "System")
    {
        QFile style_resource(fmt::format(":/stylesheet/{}.stylesheet", combobox_theme.toStdString()).c_str());
        if (style_resource.open(QIODevice::ReadOnly))
        {
            style = style_resource.readAll();
            style_resource.close();
        }
    }

    qApp->setStyleSheet(style);
}

void MainWindow::setup_navigation()
{
    std::array<std::pair<QPushButton *, QWidget *>, 4> const pages{{
            {ui->button_nav_mods, ui->page_mods},
            {ui->button_nav_parameters, ui->page_parameters},
            {ui->button_nav_server, ui->page_server},
            {ui->button_nav_settings, ui->page_settings}
        }};

    for (auto const &entry : pages)
    {
        auto page = entry.second;
        connect(entry.first, &QPushButton::clicked, this, [this, page]()
        {
            ui->stacked_pages->setCurrentWidget(page);
        });
    }
}

void MainWindow::setup_mod_filters()
{
    for (auto button : {ui->button_filter_all, ui->button_filter_enabled, ui->button_filter_disabled, ui->button_filter_workshop,
                        ui->button_filter_local})
    {
        connect(button, &QPushButton::toggled, this, [this](bool checked)
        {
            if (checked)
                apply_mod_filter();
        });
    }
}

void MainWindow::setup_launch_command_preview()
{
    for (auto container : {ui->widget_parameters_content, static_cast<QWidget *>(ui->groupbox_server_direct_connect)})
    {
        for (auto checkbox : container->findChildren<QCheckBox *>())
            connect(checkbox, &QCheckBox::toggled, this, &MainWindow::update_launch_command_preview);
        for (auto textbox : container->findChildren<QLineEdit *>())
            connect(textbox, &QLineEdit::textChanged, this, &MainWindow::update_launch_command_preview);
    }
    connect(ui->spinbox_cpu_count, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::update_launch_command_preview);

    update_launch_command_preview();
}

void MainWindow::apply_mod_filter()
{
    using Filter = ModTableWidget::Filter;

    auto filter = Filter::all;
    if (ui->button_filter_enabled->isChecked())
        filter = Filter::enabled;
    else if (ui->button_filter_disabled->isChecked())
        filter = Filter::disabled;
    else if (ui->button_filter_workshop->isChecked())
        filter = Filter::workshop;
    else if (ui->button_filter_local->isChecked())
        filter = Filter::local;

    ui->table_mods->set_filter(ui->textbox_mod_search->text(), filter);
    update_mod_preview();
}

void MainWindow::on_textbox_mod_search_textChanged(QString const &)
{
    apply_mod_filter();
}

std::optional<int> MainWindow::selected_mod_row() const
{
    auto const selected_items = ui->table_mods->selectedItems();
    if (selected_items.isEmpty())
        return std::nullopt;

    auto row = ui->table_mods->currentRow();
    if (row < 0 || ui->table_mods->item(row, 1) == nullptr || !ui->table_mods->item(row, 1)->isSelected())
        row = selected_items.first()->row();

    if (ui->table_mods->isRowHidden(row) || !ui->table_mods->is_row_complete(row))
        return std::nullopt;
    return row;
}

std::filesystem::path MainWindow::get_mod_full_path(UiMod const &mod) const
{
    if (mod.is_workshop_mod)
        return client->GetPathWorkshop() / mod.path_or_workshop_id;
    return ui_path_to_full_path(mod.path_or_workshop_id);
}

std::uintmax_t MainWindow::get_mod_size(std::filesystem::path const &path)
{
    auto const key = path.string();
    if (auto it = mod_size_cache.find(key); it != mod_size_cache.end())
        return it->second;

    std::uintmax_t size = 0;
    std::error_code ec;
    using std::filesystem::directory_options;
    for (std::filesystem::recursive_directory_iterator it(path, directory_options::skip_permission_denied, ec), end;
            !ec && it != end; it.increment(ec))
    {
        std::error_code entry_ec;
        if (it->is_regular_file(entry_ec))
        {
            auto const file_size = it->file_size(entry_ec);
            if (!entry_ec)
                size += file_size;
        }
    }

    mod_size_cache[key] = size;
    return size;
}

void MainWindow::update_mod_preview()
{
    auto const row = selected_mod_row();

    ui->button_preview_open_workshop->setVisible(true);
    ui->button_preview_website->setVisible(false);
    ui->label_preview_overview->setVisible(false);
    mod_preview_website.clear();

    if (!row)
    {
        ui->label_preview_kicker->setText("PREVIEW");
        ui->label_preview_name->setText("No mod selected");
        ui->label_preview_author->setText("Select a mod in the list to see its details.");
        ui->label_preview_size->setText("—");
        ui->label_preview_source->setText("—");
        ui->label_preview_load_order->setText("—");
        ui->label_preview_path->setText("—");
        ui->label_preview_overview->clear();
        for (auto button : {ui->button_preview_open_workshop, ui->button_preview_open_folder, ui->button_preview_copy_id,
                            ui->button_preview_toggle})
            button->setEnabled(false);
        ui->button_preview_toggle->setText("Enable");
        return;
    }

    auto const mod = ui->table_mods->get_mod_at(*row);
    auto const full_path = get_mod_full_path(mod);

    std::string author;
    std::string overview;
    bool const installed = has_addons_directory(full_path);
    if (installed)
    {
        try
        {
            Mod const m(full_path);
            author = m.GetValueOrReturnDefault("author", "");
            overview = m.GetValueOrReturnDefault("overview", "");
            mod_preview_website = m.GetValueOrReturnDefault("action", "");
        }
        catch (std::exception const &e)
        {
            spdlog::debug("Cannot read mod details for preview {}: {}", full_path.string(), e.what());
        }
    }

    int load_order = 0;
    if (mod.enabled)
        for (int i = 0; i <= *row; ++i)
            if (ui->table_mods->is_mod_enabled_at(i))
                ++load_order;

    if (mod.is_workshop_mod)
        ui->label_preview_kicker->setText(QString::fromStdString(fmt::format("WORKSHOP · {}", mod.path_or_workshop_id)));
    else
        ui->label_preview_kicker->setText("LOCAL MOD");

    ui->label_preview_name->setText(QString::fromStdString(mod.name));
    if (!author.empty())
        ui->label_preview_author->setText(QString::fromStdString(fmt::format("by {}", author)));
    else
        ui->label_preview_author->setText(mod.is_workshop_mod ? "Steam Workshop item" : "Local folder");

    ui->label_preview_size->setText(installed ? QString::fromStdString(format_size(get_mod_size(full_path))) :
                                    "Not installed");
    ui->label_preview_source->setText(mod.is_workshop_mod ? "Workshop" : "Local");
    ui->label_preview_load_order->setText(load_order > 0 ? QString::number(load_order) : "—");
    ui->label_preview_path->setText(QString::fromStdString(full_path.string()));

    ui->label_preview_overview->setTextFormat(Qt::PlainText);
    ui->label_preview_overview->setText(QString::fromStdString(overview));
    ui->label_preview_overview->setVisible(!overview.empty());

    ui->button_preview_open_workshop->setVisible(mod.is_workshop_mod);
    ui->button_preview_open_workshop->setEnabled(mod.is_workshop_mod);
    ui->button_preview_open_folder->setEnabled(installed);
    ui->button_preview_copy_id->setEnabled(true);
    ui->button_preview_copy_id->setText(mod.is_workshop_mod ? "Copy ID" : "Copy path");
    ui->button_preview_website->setVisible(StringUtils::StartsWith(mod_preview_website, "http"));
    ui->button_preview_toggle->setEnabled(true);
    ui->button_preview_toggle->setText(mod.enabled ? "Disable" : "Enable");
}

void MainWindow::update_launch_command_preview()
{
    manager.save_settings_from_ui(ui);

    auto &parameters = manager.settings["parameters"];
    std::string command;
    if (parameters["protonDisableEsync"].is_boolean() && parameters["protonDisableEsync"].get<bool>())
        command += "PROTON_NO_ESYNC=1 ";
    if (parameters["environmentVariables"].is_string())
        command += parameters["environmentVariables"].get<std::string>() + " ";
    command += manager.get_launch_parameters();

    ui->textbox_launch_command->setPlainText(QString::fromStdString(std::string(StringUtils::trim(command))));
    update_footer_details();
}

void MainWindow::update_footer_details()
{
    QStringList dlcs;
    for (auto checkbox : ui->groupbox_parameters_dlc->findChildren<QCheckBox *>())
        if (checkbox->isChecked())
            dlcs.append(checkbox->text());

    QString details = "DLC: " + (dlcs.isEmpty() ? QString("none") : dlcs.join(", "));
    if (ui->checkbox_server_address->isChecked() && !ui->textbox_server_address->text().trimmed().isEmpty())
        details += " · Joins " + ui->textbox_server_address->text().trimmed();
    ui->label_footer_details->setText(details);
}

void MainWindow::open_mod_in_workshop(UiMod const &mod)
{
    if (!mod.is_workshop_mod)
        return;
    auto const url = fmt::format(workshop_item_url_template, mod.path_or_workshop_id);
    QDesktopServices::openUrl(QUrl(QString::fromStdString(url)));
}

void MainWindow::open_mod_folder(UiMod const &mod)
{
    auto const path = get_mod_full_path(mod);
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(path.string())));
}

void MainWindow::copy_mod_id(UiMod const &mod)
{
    auto const text = mod.is_workshop_mod ? mod.path_or_workshop_id : get_mod_full_path(mod).string();
    QApplication::clipboard()->setText(QString::fromStdString(text));
}

void MainWindow::on_button_open_workshop_clicked()
{
    QDesktopServices::openUrl(QUrl(workshop_url));
}

void MainWindow::on_button_browse_workshop_clicked()
{
    QDesktopServices::openUrl(QUrl(workshop_url));
}

void MainWindow::on_button_my_subscriptions_clicked()
{
    QDesktopServices::openUrl(QUrl(workshop_subscriptions_url));
}

void MainWindow::on_button_preview_open_workshop_clicked()
{
    if (auto const row = selected_mod_row())
        open_mod_in_workshop(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_button_preview_open_folder_clicked()
{
    if (auto const row = selected_mod_row())
        open_mod_folder(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_button_preview_copy_id_clicked()
{
    if (auto const row = selected_mod_row())
        copy_mod_id(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_button_preview_website_clicked()
{
    if (StringUtils::StartsWith(mod_preview_website, "http"))
        QDesktopServices::openUrl(QUrl(QString::fromStdString(mod_preview_website)));
}

void MainWindow::on_button_preview_toggle_clicked()
{
    auto const row = selected_mod_row();
    if (!row)
        return;

    auto const enabled = ui->table_mods->is_mod_enabled_at(*row);
    ui->table_mods->set_mod_enabled_at(*row, !enabled);
    put_mods_from_ui_to_manager_settings();
}

void MainWindow::on_action_mod_open_workshop_triggered()
{
    if (auto const row = selected_mod_row())
        open_mod_in_workshop(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_action_mod_open_folder_triggered()
{
    if (auto const row = selected_mod_row())
        open_mod_folder(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_action_mod_copy_id_triggered()
{
    if (auto const row = selected_mod_row())
        copy_mod_id(ui->table_mods->get_mod_at(*row));
}

void MainWindow::on_button_copy_launch_command_clicked()
{
    QApplication::clipboard()->setText(ui->textbox_launch_command->toPlainText());
}

void MainWindow::on_button_join_server_clicked()
{
    if (ui->textbox_server_address->text().trimmed().isEmpty())
    {
        QMessageBox(QMessageBox::Icon::Information, "Cannot join server", "Enter a server address first.").exec();
        ui->checkbox_server_address->setChecked(true);
        ui->textbox_server_address->setFocus();
        return;
    }

    ui->checkbox_server_address->setChecked(true);
    if (!ui->textbox_server_port->text().trimmed().isEmpty())
        ui->checkbox_server_port->setChecked(true);
    if (!ui->textbox_server_password->text().isEmpty())
        ui->checkbox_server_password->setChecked(true);

    on_button_start_clicked();
}
