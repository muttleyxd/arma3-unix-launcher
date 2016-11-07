/*
 * main.cpp
 *
 *  Created on: 8 Oct 2016
 *      Author: muttley
 */

#include <iostream>
#include <gtkmm.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <regex>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "VDF.h"
#include "Filesystem.h"
#include "Settings.h"
#include "Utils.h"
#include "Logger.h"

#include "MainWindow.h"

using namespace std;

int main(int argc, char *argv[])
{
   /*Mod m("/mnt/nagrania/SteamLibraryLinux/steamapps/common/Arma 3/@CBA_A3", "-1");
    cout << m.Path << endl << m.DirName << endl << m.WorkshopId << endl << m.Name << endl;

    return 33;*/
    //Checking parameters for --verbose or -v
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0)
            LogLevel = 0;
    }

    LOG(1, "ArmA 3 Launcher started");

    //~/.config/a3linuxlauncher
    if (!Filesystem::DirectoryExists(Filesystem::HomeDirectory
            + Filesystem::LauncherSettingsDirectory))
    {
        bool result = Filesystem::CreateDirectory(Filesystem::HomeDirectory
                + Filesystem::LauncherSettingsDirectory);
        if (!result)
            return 1;
    }

    if (!Filesystem::FileExists(Filesystem::HomeDirectory
            + Filesystem::LauncherSettingsDirectory
            + Filesystem::LauncherSettingsFilename))
    {
        Settings::Save(Filesystem::HomeDirectory
                + Filesystem::LauncherSettingsDirectory
                + Filesystem::LauncherSettingsFilename);
    }

    Settings::Load(Filesystem::HomeDirectory
            + Filesystem::LauncherSettingsDirectory
            + Filesystem::LauncherSettingsFilename);


    if (Settings::ArmaPath == Filesystem::DIR_NOT_FOUND)
        Settings::ArmaPath = Filesystem::GetDirectory(DirectoryToFind::ArmaInstall);

    if (Settings::WorkshopPath == Filesystem::DIR_NOT_FOUND)
        Settings::WorkshopPath = Filesystem::GetDirectory(DirectoryToFind::WorkshopMods);

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--purge") == 0)
        {
            if (Settings::ArmaPath != Filesystem::DIR_NOT_FOUND)
            {
                string removeWorkshop = "rm -rf " + Utils::BashAdaptPath(Settings::ArmaPath + Filesystem::ArmaDirWorkshop);
                string removeCustom = "rm -rf " + Utils::BashAdaptPath(Settings::ArmaPath + Filesystem::ArmaDirCustom);
                system(removeWorkshop.c_str());
                system(removeCustom.c_str());
            }
            system(("rm -rf " + Filesystem::HomeDirectory + Filesystem::LauncherSettingsDirectory).c_str());
            LOG(1, "Purged config file, !workshop, !custom directories");
            exit(0);
        }
    }

    //Dirty fix to Gtk::Application trying to parse arguments on its own
    argc = 0;
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "muttley.a3linuxlauncher");

    string MainFormPath = "/usr/share/arma3-linux-launcher/MainForm.glade";
    if (!Filesystem::FileExists(MainFormPath))
        MainFormPath = "MainForm.glade";

    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file(MainFormPath);

    cout << "GTK+ version: " << gtk_major_version << "." << gtk_minor_version << "." << gtk_micro_version << endl
         << "Glib version: " << glib_major_version << "." << glib_minor_version << "." << glib_micro_version << endl;

    //if autodetection fails
    while (Settings::ArmaPath == Filesystem::DIR_NOT_FOUND)
    {
        string Message = string("Launcher couldn't detect ArmA 3 installation directory") +
                "\nClick 'Yes' to select appropriate directory" +
                "\nClick 'No' to close the program";

        Gtk::MessageDialog msg(Message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_YES_NO);
        int result = msg.run();

        if (result == Gtk::RESPONSE_NO)
            exit(2);

        Gtk::FileChooserDialog fcDialog("Select ArmA 3 install directory", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
        fcDialog.add_button("_Open", 1);
        result = fcDialog.run();
        if (result == 1)
        {
            string currentFolder = fcDialog.get_current_folder();
            if (Filesystem::FileExists(currentFolder + "/arma3.i386"))
                Settings::ArmaPath = currentFolder;
            currentFolder = fcDialog.get_filename();
            if (Filesystem::FileExists(currentFolder + "/arma3.i386"))
                Settings::ArmaPath = currentFolder;

            if (Settings::ArmaPath == Filesystem::DIR_NOT_FOUND)
            {
                string Message2 = string("Selected directory seems incorrect") +
                                "\n" + currentFolder + "/arma3.i386 doesn't exist";

                Gtk::MessageDialog msg2(Message2);
                msg2.run();
            }
        }
    }

    MainWindow* mainWindow = NULL;

    builder->get_widget_derived("MainForm", mainWindow);

    if (mainWindow)
        app->run(*mainWindow);

    delete mainWindow;

    return 0;
}
