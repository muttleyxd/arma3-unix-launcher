/*
 * MainWindow.cpp
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */

#include "MainWindow.h"
#include <iostream>
#include <string>
#include "Settings.h"
#include "Filesystem.h"
#include "Logger.h"
#include "Utils.h"

#include <cstdlib>
#include <chrono>

MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refGlade) :
    Gtk::Window(cobject), builder(refGlade)
{
    Init();

    dispatcher_.connect(sigc::mem_fun(*this, &MainWindow::notification_from_thread));

    workshopModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("workshopModsStore"));
    customModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("customModsStore"));

    //Mods tab
    builder->get_widget("tvCustomMods", tvCustomMods);
    builder->get_widget("tvWorkshopMods", tvWorkshopMods);

    builder->get_widget("btnAdd", btnAdd);
    builder->get_widget("btnRemove", btnRemove);

    builder->get_widget("btnPresetLoad", btnPresetLoad);
    builder->get_widget("btnPresetSave", btnPresetSave);

    workshopToggleBox = Glib::RefPtr<Gtk::CellRendererToggle>::cast_dynamic(builder->get_object("workshopToggleBox"));
    customToggleBox = Glib::RefPtr<Gtk::CellRendererToggle>::cast_dynamic(builder->get_object("customToggleBox"));

    //Parameters tab
    //Basic
    builder->get_widget("cbSkipIntro", cbSkipIntro);
    builder->get_widget("cbNosplash", cbNosplash);
    builder->get_widget("cbWindow", cbWindow);
    builder->get_widget("cbName", cbName);
    builder->get_widget("tbName", tbName);

    //Advanced
    builder->get_widget("cbParameterFile", cbParameterFile);
    builder->get_widget("tbParameterFile", tbParameterFile);
    builder->get_widget("btnParameterFileBrowse", btnParameterFileBrowse);

    builder->get_widget("cbCheckSignatures", cbCheckSignatures);

    builder->get_widget("cbCpuCount", cbCpuCount);
    builder->get_widget("numCpuCount", numCpuCount);

    builder->get_widget("cbExThreads", cbExThreads);
    builder->get_widget("cbExThreadsFileOperations", cbExThreadsFileOperations);
    builder->get_widget("cbExThreadsTextureLoading", cbExThreadsTextureLoading);
    builder->get_widget("cbExThreadsGeometryLoading", cbExThreadsGeometryLoading);

    builder->get_widget("cbEnableHT", cbEnableHT);
    builder->get_widget("cbDisableMulticore", cbDisableMulticore);
    builder->get_widget("cbHugePages", cbHugePages);

    builder->get_widget("cbFilePatching", cbFilePatching);
    builder->get_widget("cbNoLogs", cbNoLogs);
    builder->get_widget("cbShowScriptErrors", cbShowScriptErrors);

    builder->get_widget("cbWorld", cbWorld);
    builder->get_widget("tbWorld", tbWorld);

    builder->get_widget("cbNoPause", cbNoPause);

    //Client
    builder->get_widget("cbConnect", cbConnect);
    builder->get_widget("tbConnect", tbConnect);

    builder->get_widget("cbPort", cbPort);
    builder->get_widget("tbPort", tbPort);

    builder->get_widget("cbPassword", cbPassword);
    builder->get_widget("tbPassword", tbPassword);

    builder->get_widget("cbHost", cbHost);

    //Other
    builder->get_widget("btnQuit", btnQuit);

    //Visible everywhere
    builder->get_widget("btnPlay", btnPlay);

    builder->get_widget("lblSelectedMods", lblSelectedMods);
    builder->get_widget("lblStatus", lblStatus);

    //synchronize values with Settings
    cbSkipIntro->set_active(Settings::SkipIntro);
    cbNosplash->set_active(Settings::Nosplash);
    cbWindow->set_active(Settings::Window);
    cbName->set_active(Settings::Name);
    tbName->set_text(Settings::NameValue);

    cbParameterFile->set_active(Settings::ParameterFile);
    tbParameterFile->set_text(Settings::ParameterFileValue);

    cbCheckSignatures->set_active(Settings::CheckSignatures);

    cbCpuCount->set_active(Settings::CpuCount);
    numCpuCount->set_value(Settings::CpuCountValue);

    cbExThreads->set_active(Settings::ExThreads);
    cbExThreadsFileOperations->set_active(Settings::ExThreadsFileOperations);
    cbExThreadsTextureLoading->set_active(Settings::ExThreadsTextureLoading);
    cbExThreadsGeometryLoading->set_active(Settings::ExThreadsGeometryLoading);

    cbEnableHT->set_active(Settings::EnableHT);
    cbDisableMulticore->set_active(Settings::DisableMulticore);
    cbHugePages->set_active(Settings::HugePages);

    cbFilePatching->set_active(Settings::FilePatching);
    cbNoLogs->set_active(Settings::NoLogs);
    cbShowScriptErrors->set_active(Settings::ShowScriptErrors);

    cbWorld->set_active(Settings::World);
    tbWorld->set_text(Settings::WorldValue);

    cbNoPause->set_active(Settings::NoPause);

    cbConnect->set_active(Settings::Connect);
    tbConnect->set_text(Settings::ConnectValue);

    cbPort->set_active(Settings::Port);
    tbPort->set_text(Settings::PortValue);

    cbPassword->set_active(Settings::Password);
    tbPassword->set_text(Settings::PasswordValue);

    cbHost->set_active(Settings::Host);

    btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnAdd_Clicked));
    btnRemove->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnRemove_Clicked));

    btnPresetLoad->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnPresetLoad_Clicked));
    btnPresetSave->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnPresetSave_Clicked));

    cbSkipIntro->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbSkipIntro_Toggled));
    cbNosplash->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbNosplash_Toggled));
    cbWindow->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbWindow_Toggled));
    cbName->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbName_Toggled));
    tbName->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbName_Changed));

    cbParameterFile->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbParameterFile_Toggled));
    tbParameterFile->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbParameterFile_Changed));
    btnParameterFileBrowse->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnParameterFileBrowse_Clicked));

    cbCheckSignatures->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbCheckSignatures_Toggled));

    cbCpuCount->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbCpuCount_Toggled));
    numCpuCount->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::numCpuCount_Changed));

    cbExThreads->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbExThreads_Toggled));
    cbExThreadsFileOperations->signal_toggled().connect(sigc::mem_fun(*this,
            &MainWindow::cbExThreadsFileOperations_Toggled));
    cbExThreadsTextureLoading->signal_toggled().connect(sigc::mem_fun(*this,
            &MainWindow::cbExThreadsTextureLoading_Toggled));
    cbExThreadsGeometryLoading->signal_toggled().connect(sigc::mem_fun(*this,
            &MainWindow::cbExThreadsGeometryLoading_Toggled));

    cbEnableHT->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbEnableHT_Toggled));
    cbDisableMulticore->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbDisableMulticore_Toggled));
    cbHugePages->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbHugePages_Toggled));

    cbFilePatching->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbFilePatching_Toggled));
    cbNoLogs->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbNoLogs_Toggled));
    cbShowScriptErrors->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbShowScriptErrors_Toggled));

    cbWorld->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbWorld_Toggled));
    tbWorld->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbWorld_Changed));

    cbNoPause->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbNoPause_Toggled));

    cbConnect->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbConnect_Toggled));
    tbConnect->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbConnect_Changed));

    cbPort->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbPort_Toggled));
    tbPort->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbPort_Changed));

    cbPassword->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbPassword_Toggled));
    tbPassword->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbPassword_Changed));

    cbHost->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbHost_Toggled));

    btnQuit->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnQuit_Clicked));

    btnPlay->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnPlay_Clicked));

    cbNosplash->set_tooltip_text("(1.70) Game will Crash if you enable this and alt tab during the initial loading screen!"
                                 "\nWait until you're in the main menu!");

    /////Executing every event - need to make sure UI represents actual Settings
    ignore = true;
    cbSkipIntro_Toggled();
    cbNosplash_Toggled();
    cbWindow_Toggled();
    cbName_Toggled();
    tbName_Changed();
    cbParameterFile_Toggled();
    tbParameterFile_Changed();
    cbCheckSignatures_Toggled();
    cbCpuCount_Toggled();
    numCpuCount_Changed();

    cbExThreads_Toggled();
    cbExThreadsFileOperations_Toggled();
    cbExThreadsTextureLoading_Toggled();
    cbExThreadsGeometryLoading_Toggled();
    cbEnableHT_Toggled();
    cbDisableMulticore_Toggled();
    cbHugePages_Toggled();
    cbFilePatching_Toggled();
    cbNoLogs_Toggled();
    cbShowScriptErrors_Toggled();
    cbWorld_Toggled();
    tbWorld_Changed();
    cbNoPause_Toggled();
    cbConnect_Toggled();
    tbConnect_Changed();
    cbPort_Toggled();
    tbPort_Changed();
    cbPassword_Toggled();
    tbPassword_Changed();
    cbHost_Toggled();
    ignore = false;
    /////

    set_title("ArmA 3 Unix Launcher");
    set_default_size(Settings::WindowSizeX, Settings::WindowSizeY);
    move(Settings::WindowPosX, Settings::WindowPosY);

    for (std::string s : Settings::WorkshopModsOrder)
    {
        for (int i = 0; i < WorkshopMods.size(); i++)
        {
            if (WorkshopMods[i].WorkshopId == s)
            {
                Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->append());
                row[workshopColumns.enabled] = Settings::ModEnabled(WorkshopMods[i].WorkshopId);
                row[workshopColumns.name] = WorkshopMods[i].Name;
                row[workshopColumns.workshopid] = WorkshopMods[i].WorkshopId;
                WorkshopMods.erase(WorkshopMods.begin() + i);
                break;
            }
        }
    }

    for (Mod m : WorkshopMods)
    {
        Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->append());
        row[workshopColumns.enabled] = Settings::ModEnabled(m.WorkshopId);
        row[workshopColumns.name] = m.Name;
        row[workshopColumns.workshopid] = m.WorkshopId;
    }

    for (std::string s : Settings::CustomModsOrder)
    {
        for (int i = 0; i < CustomMods.size(); i++)
        {
            if (CustomMods[i].WorkshopId == s)
            {
                Gtk::TreeModel::Row row = *(customModsStore.operator ->()->append());
                row[customColumns.enabled] = Settings::ModEnabled(CustomMods[i].Path);
                row[customColumns.name] = CustomMods[i].Name;
                row[customColumns.path] = Utils::Replace(CustomMods[i].Path, Settings::ArmaPath, Filesystem::ArmaDirMark);
                CustomMods.erase(CustomMods.begin() + i);
                break;
            }
        }
    }

    for (Mod m : CustomMods)
    {
        Gtk::TreeModel::Row row = *(customModsStore.operator ->()->append());
        row[customColumns.enabled] = Settings::ModEnabled(m.Path);
        row[customColumns.name] = m.Name;
        row[customColumns.path] = Utils::Replace(m.Path, Settings::ArmaPath, Filesystem::ArmaDirMark);
    }

    workshopToggleBox->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::WorkshopToggleBox_Toggled));
    customToggleBox->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::CustomToggleBox_Toggled));

    this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::onExit));

    RefreshStatusLabel();

    stop_arma_thread.store(false);
    armaStatusThread = new std::thread(&MainWindow::ArmaStatusThread, this);
}

void MainWindow::notify()
{
    dispatcher_.emit();
}

void MainWindow::notification_from_thread()
{
    pid_t pid = armaPid;
    if (pid != -1)
        lblStatus->set_text("Status: ArmA 3 running, PID: " + std::to_string(armaPid));
    else
        lblStatus->set_text("Status: ArmA 3 not running");
}

void MainWindow::ArmaStatusThread()
{
    LOG(1, "Status monitoring thread started");
    while (!stop_arma_thread.load())
    {
        #ifdef __APPLE__
        armaPid = Utils::FindProcess("ArmA3");
        #else
        armaPid = Utils::FindProcess("./arma3.x86_64");
        if (armaPid == -1)
            armaPid = Utils::FindProcess("Arma3_x64.exe");
        #endif
        this->notify();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void MainWindow::btnAdd_Clicked()
{
    Gtk::FileChooserDialog fcDialog(*this, "Select new mod folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    /*fcDialog.set_title("Select new mod folder");
    fcDialog.set_action(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);*/
    fcDialog.add_button("_Open", 1);
    fcDialog.add_button("_Cancel", 0);
    int result = fcDialog.run();

    LOG(0, "Add dialog result: " + std::to_string(result));
    if (result)
    {
        LOG(0, "Selected filename: " + fcDialog.get_filename());
        LOG(0, "Current folder: " + fcDialog.get_current_folder());
        std::string selectedPath = fcDialog.get_filename();
        if (!Utils::ContainsAddons(selectedPath))
            selectedPath = fcDialog.get_current_folder();

        if (selectedPath == Settings::ArmaPath)
        {
            Gtk::MessageDialog msgDialog("You can't add ArmA's main directory", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            msgDialog.run();
            LOG(1, "Selected directory equals ArmA path");
            return;
        }

        if (!Utils::ContainsAddons(selectedPath))
        {
            Gtk::MessageDialog msgDialog("Couldn't find Addons folder in selected directory", false, Gtk::MESSAGE_ERROR,
                                         Gtk::BUTTONS_OK, true);
            msgDialog.run();
            LOG(1, "Selected directory doesn't contain Addons folder");
            return;
        }

        for (int i = 0; i < customModsStore.operator ->()->children().size(); i++)
        {
            Gtk::TreeModel::Row row = *(customModsStore.operator ->()->get_iter(std::to_string(i).c_str()));

            Glib::ustring path = row[customColumns.path];
            std::string pathStr = Utils::Replace(path.raw(), Filesystem::ArmaDirMark, Settings::ArmaPath);

            if (pathStr == selectedPath)
            {
                Gtk::MessageDialog msgDialog("Mod with this path exists", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                msgDialog.run();
                LOG(1, "Selected directory exists in customModsStore");
                return;
            }
        }

        Gtk::TreeModel::Row row = *(customModsStore.operator ->()->append());
        Mod m(selectedPath, "-1");
        row[customColumns.enabled] = false;
        row[customColumns.name] = m.Name;
        row[customColumns.path] = Utils::Replace(m.Path, Settings::ArmaPath, Filesystem::ArmaDirMark);

        std::vector<Mod> modList;
        modList.push_back(m);

        Filesystem::CheckFileStructure(Settings::ArmaPath, Settings::WorkshopPath, modList);
    }
    RefreshStatusLabel();
}

void MainWindow::btnRemove_Clicked()
{
    Glib::RefPtr<Gtk::TreeSelection> treeSel = tvCustomMods->get_selection();
    if (!treeSel.operator ->()->get_selected())
    {
        Gtk::MessageDialog msgDialog("Select a mod to delete first", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        msgDialog.run();
        return;
    }
    Gtk::TreeModel::Row row = *(treeSel.operator ->()->get_selected());
    Glib::ustring path = row[customColumns.path];
    if (path.raw().find(Filesystem::ArmaDirMark) != std::string::npos)
    {
        Gtk::MessageDialog msgDialog("You can't remove mods from ArmA's directory", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK,
                                     true);
        msgDialog.run();
        LOG(1, "Can't delete mods from ArmA's directory");
        return;
    }

    customModsStore.operator ->()->erase(treeSel.operator ->()->get_selected());
    RefreshStatusLabel();
}

void MainWindow::btnPresetLoad_Clicked()
{
    Gtk::FileChooserDialog fcDialog(*this, "Select a Preset", Gtk::FILE_CHOOSER_ACTION_OPEN);
    fcDialog.add_button("_Open", 1);
    fcDialog.add_button("_Cancel", 0);
    fcDialog.set_current_folder(Filesystem::HomeDirectory + Filesystem::LauncherSettingsDirectory);

    Glib::RefPtr<Gtk::FileFilter> fcFilter = Gtk::FileFilter::create();
    fcFilter->set_name("Preset files (*.a3ulm)");
    fcFilter->add_pattern("*.a3ulm");
    fcDialog.add_filter(fcFilter);

    Glib::RefPtr<Gtk::FileFilter> fcFilterAll = Gtk::FileFilter::create();
    fcFilterAll->set_name("All files (*)");
    fcFilterAll->add_pattern("*");
    fcDialog.add_filter(fcFilterAll);

    int result = fcDialog.run();

    if (result)
    {
        std::string fname = fcDialog.get_filename();
        Settings::ModPreset = fname.substr(fname.find_last_of('/') + 1);
        std::string contents = Filesystem::ReadAllText(fcDialog.get_filename());
        Settings::WorkshopModsEnabled.clear();
        Settings::CustomModsEnabled.clear();

        //load mods into settings
        for (std::string line : Utils::Split(contents, "\n"))
        {
            if (Utils::StartsWith(line, "WorkshopModsEnabled="))
            {
                std::string sub = line.substr(20);
                for (std::string s : Utils::Split(sub, ","))
                    Settings::WorkshopModsEnabled.push_back(s);
            }
            else if (Utils::StartsWith(line, "CustomModsEnabled="))
            {
                std::string sub = line.substr(18);
                for (std::string s : Utils::Split(sub, ","))
                    Settings::CustomModsEnabled.push_back(s);
            }
        }

        //update UI to reflect changes
        for (int i = 0; i < workshopModsStore.operator ->()->children().size(); i++)
        {
            Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->get_iter(std::to_string(i).c_str()));

            Glib::ustring workshopid = row[workshopColumns.workshopid];

            row[workshopColumns.enabled] = Settings::ModEnabled(workshopid.raw());
        }

        for (int i = 0; i < customModsStore.operator ->()->children().size(); i++)
        {
            Gtk::TreeModel::Row row = *(customModsStore.operator ->()->get_iter(std::to_string(i).c_str()));

            Glib::ustring path = row[customColumns.path];
            std::string pathStr = Utils::Replace(path.raw(), Filesystem::ArmaDirMark, Settings::ArmaPath);

            row[customColumns.enabled] = Settings::ModEnabled(pathStr);
        }

        RefreshStatusLabel();
    }
}

void MainWindow::btnPresetSave_Clicked()
{
    PutModsToSettings();

    Gtk::FileChooserDialog fcDialog(*this, "Name your Preset", Gtk::FILE_CHOOSER_ACTION_SAVE);
    fcDialog.add_button("_Save", 1);
    fcDialog.add_button("_Cancel", 0);
    std::string path = Filesystem::HomeDirectory + Filesystem::LauncherSettingsDirectory;
    fcDialog.set_current_folder(path);

    Glib::RefPtr<Gtk::FileFilter> fcFilter = Gtk::FileFilter::create();
    fcFilter->set_name("Preset files (*.a3ulm)");
    fcFilter->add_pattern("*.a3ulm");
    fcDialog.add_filter(fcFilter);

    int result = fcDialog.run();

    if (result)
    {
        std::string fname = fcDialog.get_filename();
        // Check if file name ends with .a3ulm, if not, then append
        if (!Utils::EndsWith(fname, ".a3ulm"))
            fname += ".a3ulm";
        Settings::ModPreset = fname.substr(fname.find_last_of('/') + 1);
        RefreshStatusLabel();

        std::string outfile = "WorkshopModsEnabled=";
        for (std::string s : Settings::WorkshopModsEnabled)
            outfile += s + ",";
        outfile += "\nCustomModsEnabled=";
        for (std::string s : Settings::CustomModsEnabled)
            outfile += Utils::Replace(s, Filesystem::ArmaDirMark, Settings::ArmaPath) + ",";

        Filesystem::WriteAllText(fname, outfile);
    }
}

bool MainWindow::onExit(GdkEventAny *event)
{
    LOG(1, "onExit()");

    this->get_position(Settings::WindowPosX, Settings::WindowPosY);
    this->get_size(Settings::WindowSizeX, Settings::WindowSizeY);

    PutModsToSettings();

    Settings::Save(Filesystem::HomeDirectory
                   + Filesystem::LauncherSettingsDirectory
                   + Filesystem::LauncherSettingsFilename);

    stop_arma_thread.store(true);
    armaStatusThread->join();
    return false;
}

void MainWindow::WorkshopToggleBox_Toggled(Glib::ustring path) //path is index number
{
    //std::cout << path << std::endl;
    Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->get_iter(path));
    row[workshopColumns.enabled] = !row[workshopColumns.enabled];
    if (Settings::ModPreset[Settings::ModPreset.length() - 1] != '*')
        Settings::ModPreset += "*";
    RefreshStatusLabel();
}

void MainWindow::CustomToggleBox_Toggled(Glib::ustring path) //path is index number
{
    //std::cout << path << std::endl;
    Gtk::TreeModel::Row row = *(customModsStore.operator ->()->get_iter(path));
    row[customColumns.enabled] = !row[customColumns.enabled];
    if (Settings::ModPreset[Settings::ModPreset.length() - 1] != '*')
        Settings::ModPreset += "*";
    RefreshStatusLabel();
}

void MainWindow::cbSkipIntro_Toggled()
{
    LOG(0, "cbSkipIntro_Toggled: " + Utils::ToString(cbSkipIntro->get_active()));
    if (!ignore) Settings::SkipIntro = cbSkipIntro->get_active();
}

void MainWindow::cbNosplash_Toggled()
{
    LOG(0, "cbNosplash_Toggled: " + Utils::ToString(cbNosplash->get_active()));
    if (!ignore) Settings::Nosplash = cbNosplash->get_active();
}

void MainWindow::cbWindow_Toggled()
{
    LOG(0, "cbWindow_Toggled: " + Utils::ToString(cbWindow->get_active()));
    if (!ignore) Settings::Window = cbWindow->get_active();
}

void MainWindow::cbName_Toggled()
{
    LOG(0, "cbName_Toggled: " + Utils::ToString(cbName->get_active()));
    if (!ignore) Settings::Name = cbName->get_active();
    tbName->set_sensitive(Settings::Name);
}

void MainWindow::tbName_Changed()
{
    LOG(0, "tbName_Changed: " + tbName->get_text());
    if (!ignore) Settings::NameValue = tbName->get_text();
}

void MainWindow::cbParameterFile_Toggled()
{
    LOG(0, "cbParameterFile_Toggled: " + Utils::ToString(cbParameterFile->get_active()));
    if (!ignore) Settings::ParameterFile = cbParameterFile->get_active();
    tbParameterFile->set_sensitive(Settings::ParameterFile);
    btnParameterFileBrowse->set_sensitive(Settings::ParameterFile);
}

void MainWindow::tbParameterFile_Changed()
{
    LOG(0, "tbParameterFile_Changed: " + tbParameterFile->get_text());
    if (!ignore) Settings::ParameterFileValue = tbParameterFile->get_text();
}

void MainWindow::btnParameterFileBrowse_Clicked()
{
    LOG(0, "btnParameterFileBrowse_Clicked");
    Gtk::FileChooserDialog fcDialog(*this, "Select parameter file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    fcDialog.add_button("_Open", 1);
    fcDialog.add_button("_Cancel", 0);
    int result = fcDialog.run();

    if (result)
        tbParameterFile->set_text(fcDialog.get_filename());
}

void MainWindow::cbCheckSignatures_Toggled()
{
    LOG(0, "cbCheckSignatures_Toggled: " + Utils::ToString(cbCheckSignatures->get_active()));
    if (!ignore) Settings::CheckSignatures = cbCheckSignatures->get_active();
}

void MainWindow::cbCpuCount_Toggled()
{
    LOG(0, "cbCpuCount_Toggled: " + Utils::ToString(cbCpuCount->get_active()));
    if (!ignore) Settings::CpuCount = cbCpuCount->get_active();
    numCpuCount->set_sensitive(Settings::CpuCount);
}

void MainWindow::numCpuCount_Changed()
{
    LOG(0, "numCpuCount_Changed: " + std::to_string(numCpuCount->get_value()));
    if (!ignore) Settings::CpuCountValue = numCpuCount->get_value();
}

void MainWindow::cbExThreads_Toggled()
{
    LOG(0, "cbExThreads_Toggled: " + Utils::ToString(cbExThreads->get_active()));
    if (!ignore) Settings::ExThreads = cbExThreads->get_active();
    cbExThreadsFileOperations->set_sensitive(Settings::ExThreads);
    cbExThreadsTextureLoading->set_sensitive(Settings::ExThreads);
    cbExThreadsGeometryLoading->set_sensitive(Settings::ExThreads);
}

void MainWindow::cbExThreadsFileOperations_Toggled()
{
    LOG(0, "cbExThreadsFileOperations_Toggled: " + Utils::ToString(cbExThreadsFileOperations->get_active()));
    if (!ignore)
    {
        Settings::ExThreadsFileOperations = cbExThreadsFileOperations->get_active();
        if (!Settings::ExThreadsFileOperations)
        {
            cbExThreadsTextureLoading->set_active(false);
            cbExThreadsGeometryLoading->set_active(false);
            Settings::ExThreadsGeometryLoading = false;
            Settings::ExThreadsTextureLoading = false;
        }
    }
}

void MainWindow::cbExThreadsTextureLoading_Toggled()
{
    LOG(0, "cbExThreadsTextureLoading_Toggled: " + Utils::ToString(cbExThreadsTextureLoading->get_active()));
    if (!ignore)
    {
        Settings::ExThreadsTextureLoading = cbExThreadsTextureLoading->get_active();
        if (Settings::ExThreadsTextureLoading)
        {
            cbExThreadsFileOperations->set_active(true);
            Settings::ExThreadsFileOperations = true;
        }
    }
}

void MainWindow::cbExThreadsGeometryLoading_Toggled()
{
    LOG(0, "cbExThreadsGeometryLoading_Toggled: " + Utils::ToString(cbExThreadsGeometryLoading->get_active()));
    if (!ignore)
    {
        Settings::ExThreadsGeometryLoading = cbExThreadsGeometryLoading->get_active();
        if (Settings::ExThreadsGeometryLoading)
        {
            cbExThreadsFileOperations->set_active(true);
            Settings::ExThreadsFileOperations = true;
        }
    }
}

void MainWindow::cbEnableHT_Toggled()
{
    LOG(0, "cbEnableHT_Toggled: " + Utils::ToString(cbEnableHT->get_active()));
    if (!ignore) Settings::EnableHT = cbEnableHT->get_active();
}

void MainWindow::cbDisableMulticore_Toggled()
{
    LOG(0, "cbDisableMulticore_Toggled: " + Utils::ToString(cbDisableMulticore->get_active()));
    if (!ignore) Settings::DisableMulticore = cbDisableMulticore->get_active();
}

void MainWindow::cbHugePages_Toggled()
{
    LOG(0, "cbHugePages_Toggled: " + Utils::ToString(cbHugePages->get_active()));
    if (!ignore) Settings::HugePages = cbHugePages->get_active();
}

void MainWindow::cbFilePatching_Toggled()
{
    LOG(0, "cbFilePatching_Toggled: " + Utils::ToString(cbFilePatching->get_active()));
    if (!ignore) Settings::FilePatching = cbFilePatching->get_active();
}

void MainWindow::cbNoLogs_Toggled()
{
    LOG(0, "cbNoLogs_Toggled: " + Utils::ToString(cbNoLogs->get_active()));
    if (!ignore) Settings::NoLogs = cbNoLogs->get_active();
}

void MainWindow::cbShowScriptErrors_Toggled()
{
    LOG(0, "cbShowScriptErrors_Toggled: " + Utils::ToString(cbShowScriptErrors->get_active()));
    if (!ignore) Settings::ShowScriptErrors = cbShowScriptErrors->get_active();
}

void MainWindow::cbWorld_Toggled()
{
    LOG(0, "cbWorld_Toggled: " + Utils::ToString(cbWorld->get_active()));
    if (!ignore) Settings::World = cbWorld->get_active();
    tbWorld->set_sensitive(Settings::World);
}

void MainWindow::tbWorld_Changed()
{
    LOG(0, "tbWorld_Changed: " + tbWorld->get_text());
    if (!ignore) Settings::WorldValue = tbWorld->get_text();
}

void MainWindow::cbNoPause_Toggled()
{
    LOG(0, "cbNoPause_Toggled: " + Utils::ToString(cbNoPause->get_active()));
    if (!ignore) Settings::NoPause = cbNoPause->get_active();
}

void MainWindow::cbConnect_Toggled()
{
    LOG(0, "cbConnect_Toggled: " + Utils::ToString(cbConnect->get_active()));
    if (!ignore) Settings::Connect = cbConnect->get_active();
    tbConnect->set_sensitive(Settings::Connect);
}

void MainWindow::tbConnect_Changed()
{
    LOG(0, "tbConnect_Changed: " + tbConnect->get_text());
    if (!ignore) Settings::ConnectValue = tbConnect->get_text();
}

void MainWindow::cbPort_Toggled()
{
    LOG(0, "cbPort_Toggled: " + Utils::ToString(cbPort->get_active()));
    if (!ignore) Settings::Port = cbPort->get_active();
    tbPort->set_sensitive(Settings::Port);
}

void MainWindow::tbPort_Changed()
{
    LOG(0, "tbPort_Changed: " + tbPort->get_text());
    if (!ignore) Settings::PortValue = tbPort->get_text();
}

void MainWindow::cbPassword_Toggled()
{
    LOG(0, "cbPassword_Toggled: " + Utils::ToString(cbPassword->get_active()));
    if (!ignore) Settings::Password = cbPassword->get_active();
    tbPassword->set_sensitive(Settings::Password);
}

void MainWindow::tbPassword_Changed()
{
    LOG(0, "tbPassword_Changed: " + tbPassword->get_text());
    if (!ignore) Settings::PasswordValue = tbPassword->get_text();
}

void MainWindow::cbHost_Toggled()
{
    LOG(0, "cbHost_Toggled: " + Utils::ToString(cbHost->get_active()));
    if (!ignore) Settings::Host = cbHost->get_active();
}

void MainWindow::btnQuit_Clicked()
{
    this->close();
}

void MainWindow::btnPlay_Clicked()
{
    LOG(0, "btnPlay_Clicked");

    if (armaPid != -1)
    {
        Gtk::MessageDialog msg("ArmA 3 is already running! PID: " + std::to_string(armaPid), false, Gtk::MESSAGE_INFO,
                               Gtk::BUTTONS_OK, true);
        msg.run();
        return;
    }

    std::string parameters;

    PutModsToSettings();

    std::vector<Mod *> modList;

    LOG(1, "FullModList size:" + std::to_string(FullModList.size()));

    for (std::string s : Settings::WorkshopModsEnabled)
    {
        for (int i = 0; i < FullModList.size(); i++)
        {
            if (FullModList[i].WorkshopId == s)
            {
                LOG(0, "Mod: " + FullModList[i].WorkshopId);
                modList.push_back(&FullModList[i]);
                break;
            }
        }
    }

    for (std::string s : Settings::CustomModsEnabled)
    {
        for (int i = 0; i < FullModList.size(); i++)
        {
            if (FullModList[i].Path == s)
            {
                LOG(0, "Mod: " + FullModList[i].Path);
                modList.push_back(&FullModList[i]);
                break;
            }
        }
    }

    LOG(1, Filesystem::ArmaConfigFile);
    std::string newArmaCfg = Filesystem::GenerateArmaCfg(Settings::ArmaPath, Filesystem::ArmaConfigFile, modList);
    Filesystem::WriteAllText(Filesystem::ArmaConfigFile, newArmaCfg);
    LOG(0, "Arma3.cfg:\n--------------------\n" + newArmaCfg + "\n--------------------");

    parameters += Settings::SkipIntro           ? "-skipIntro " : "";
    parameters += Settings::Nosplash            ? "-noSplash " : "";
    parameters += Settings::Window              ? "-window " : "";
    parameters += Settings::Name                ? "-name=" + Settings::NameValue + " " : "";

    parameters += Settings::ParameterFile       ? "-par=" + Settings::ParameterFileValue + " " : "";

    parameters += Settings::CheckSignatures     ? "-checkSignatures " : "";

    parameters += Settings::CpuCount            ? "-cpuCount=" + std::to_string(Settings::CpuCountValue) + " "  : "";

    int exThreadsValue = Settings::ExThreadsFileOperations
                         + Settings::ExThreadsTextureLoading * 2
                         + Settings::ExThreadsGeometryLoading * 4;

    parameters += Settings::ExThreads           ? "-exThreads=" + std::to_string(exThreadsValue) + " " : "";

    parameters += Settings::EnableHT            ? "-enableHT " : "";
    parameters += Settings::DisableMulticore    ? "-noCB " : "";
    parameters += Settings::HugePages           ? "-hugePages " : "";

    parameters += Settings::FilePatching        ? "-filePatching " : "";
    parameters += Settings::NoLogs              ? "-noLogs " : "";
    parameters += Settings::ShowScriptErrors    ? "-showScriptErrors " : "";

    parameters += Settings::World               ? "-world=" + Settings::WorldValue + " " : "";

    parameters += Settings::NoPause             ? "-noPause " : "";

    parameters += Settings::Connect             ? "-connect=" + Settings::ConnectValue + " " : "";
    parameters += Settings::Port                ? "-port=" + Settings::PortValue + " " : "";
    parameters += Settings::Password            ? "-password=" + Settings::PasswordValue + " " : "";

    parameters += Settings::Host                ? "-host" : "";

    std::string launch_command;
    #ifdef __APPLE__
    launch_command = "open steam://run/107410//" + Utils::Replace(parameters, " ", "%20");
    #else
    if (Filesystem::IsProton(Settings::ArmaPath))
        launch_command = "steam -applaunch 107410 -nolauncher " + parameters;
    else
        launch_command = "steam -applaunch 107410 " + parameters;
    #endif
    LOG(0, "Arma 3 launch command:\n steam -applaunch 107410 " + parameters);
    Glib::spawn_command_line_async(launch_command);
}

void MainWindow::RefreshStatusLabel()
{
    int workshopMods = 0, customMods = 0;
    for (int i = 0; i < workshopModsStore.operator ->()->children().size(); i++)
    {
        Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->get_iter(std::to_string(i).c_str()));
        if (row[workshopColumns.enabled])
            workshopMods++;
    }
    for (int i = 0; i < customModsStore.operator ->()->children().size(); i++)
    {
        Gtk::TreeModel::Row row = *(customModsStore.operator ->()->get_iter(std::to_string(i).c_str()));
        if (row[customColumns.enabled])
            customMods++;
    }

    lblSelectedMods->set_text("Selected " + std::to_string(workshopMods + customMods)
                              + " mods (" + std::to_string(workshopMods)
                              + " from workshop, " + std::to_string(customMods)
                              + " custom)"
                              + "\t"
                              + "Mod Preset: " + Settings::ModPreset);
}

void MainWindow::Init()
{
    LOG(1, "ArmA 3 Path: " + Settings::ArmaPath + "\nWorkshop mods path: " + Settings::WorkshopPath);

    WorkshopMods = Filesystem::FindMods(Settings::WorkshopPath);

    for (std::string path : Settings::CustomModsOrder)
    {
        LOG(0, "Custom mod: " + path);
        //path = Utils::Replace(path, Filesystem::ArmaDirMark, Settings::ArmaPath);
        CustomMods.push_back(Mod(path, "-1"));
    }

    std::vector<Mod> ArmaDirMods = Filesystem::FindMods(Settings::ArmaPath);
    for (Mod m : ArmaDirMods)
    {
        bool alreadyExists = false;
        for (Mod n : CustomMods)
        {
            if (n.Path == m.Path)
                alreadyExists = true;
        }
        if (!alreadyExists)
            CustomMods.push_back(m);
    }

    // Removes CustomMods that are not located inside the ArmaDir anymore.
    for (std::vector<Mod>::iterator it = CustomMods.begin(); it != CustomMods.end();)
    {
        bool found = false;
        for (Mod m : ArmaDirMods)
        {
            if (it->Path == m.Path)
            {
                found = true;
                break;
            }
        }
        if (!found)
            it = CustomMods.erase(it);
        else
            it++;
    }

    FullModList.clear();
    for (Mod m : WorkshopMods)
        FullModList.push_back(m);
    for (Mod m : CustomMods)
        FullModList.push_back(m);

    Filesystem::CheckFileStructure(Settings::ArmaPath, Settings::WorkshopPath, FullModList);
}

void MainWindow::PutModsToSettings()
{
    Settings::WorkshopModsEnabled.clear();
    Settings::WorkshopModsOrder.clear();

    Settings::CustomModsEnabled.clear();
    Settings::CustomModsOrder.clear();

    for (int i = 0; i < workshopModsStore.operator ->()->children().size(); i++)
    {
        Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->get_iter(std::to_string(i).c_str()));
        LOG(0, "[W" + Utils::ToString(row[workshopColumns.enabled]) + "] Name: " + row[workshopColumns.name] + " WorkshopId: " +
            row[workshopColumns.workshopid]);

        Glib::ustring workshopId = row[workshopColumns.workshopid];
        Settings::WorkshopModsOrder.push_back(workshopId);

        if (row[workshopColumns.enabled])
            Settings::WorkshopModsEnabled.push_back(workshopId.raw());
    }

    for (int i = 0; i < customModsStore.operator ->()->children().size(); i++)
    {
        Gtk::TreeModel::Row row = *(customModsStore.operator ->()->get_iter(std::to_string(i).c_str()));

        Glib::ustring path = row[customColumns.path];
        std::string pathStr = Utils::Replace(path.raw(), Filesystem::ArmaDirMark, Settings::ArmaPath);

        LOG(0, "[C" + Utils::ToString(row[customColumns.enabled]) + "] Name: " + row[customColumns.name] + " Path: " + pathStr);
        Settings::CustomModsOrder.push_back(pathStr);

        if (row[customColumns.enabled])
            Settings::CustomModsEnabled.push_back(pathStr);
    }
}
