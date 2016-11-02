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
			exit(1);
	}

	//~/.config/a3linuxlauncher/custommods
	if (!Filesystem::DirectoryExists(Filesystem::HomeDirectory
	            + Filesystem::LauncherSettingsDirectory
	            + Filesystem::LauncherCustomModDirectory))
    {
        bool result = Filesystem::CreateDirectory(Filesystem::HomeDirectory
                + Filesystem::LauncherSettingsDirectory
                + Filesystem::LauncherCustomModDirectory);
        if (!result)
            exit(1);
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

	//Dirty fix to Gtk::Application trying to parse arguments on its own
	argc = 0;
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "muttley.a3linuxlauncher");
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("MainForm.glade");

	cout << "GTK+ version: " << gtk_major_version << "." << gtk_minor_version << "." << gtk_micro_version << endl
		 << "Glib version: " << glib_major_version << "." << glib_minor_version << "." << glib_micro_version << endl;

	MainWindow* mainWindow = NULL;

	builder->get_widget_derived("MainForm", mainWindow);

	if (mainWindow)
		app->run(*mainWindow);

	delete mainWindow;

	return 0;
}
