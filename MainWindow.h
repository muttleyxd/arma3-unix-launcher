/*
 * MainWindow.h
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>
#include "Mod.h"
#include <vector>

class MainWindow : public Gtk::Window
{
	protected:
		Glib::RefPtr<Gtk::Builder> builder;

		Glib::RefPtr<Gtk::ListStore> workshopModsStore;
		Glib::RefPtr<Gtk::ListStore> customModsStore;

		//Mods tab
			Gtk::TreeView* tvWorkshopMods;
			Gtk::TreeView* tvCustomMods;

			Gtk::Button* btnAdd;
			Gtk::Button* btnRemove;

			Glib::RefPtr<Gtk::CellRendererToggle> workshopToggleBox;
			Glib::RefPtr<Gtk::CellRendererToggle> customToggleBox;

		//Parameters tab

			//Basic
			Gtk::CheckButton* cbSkipIntro;
			Gtk::CheckButton* cbNosplash;
			Gtk::CheckButton* cbWindow;
			Gtk::CheckButton* cbName;
			Gtk::Entry* tbName;

			//Advanced
			Gtk::CheckButton* cbParameterFile;
			Gtk::Entry* tbParameterFile;
			Gtk::Button* btnParameterFileBrowse;

			Gtk::CheckButton* cbCheckSignatures;

			Gtk::CheckButton* cbCpuCount;
			Gtk::SpinButton* numCpuCount;

			Gtk::CheckButton* cbExThreads;
			Gtk::CheckButton* cbExThreadsFileOperations;
			Gtk::CheckButton* cbExThreadsTextureLoading;
			Gtk::CheckButton* cbExThreadsGeometryLoading;

			Gtk::CheckButton* cbEnableHT;
			Gtk::CheckButton* cbFilePatching;
			Gtk::CheckButton* cbNoLogs;

			Gtk::CheckButton* cbWorld;
			Gtk::Entry* tbWorld;

			Gtk::CheckButton* cbNoPause;

			//Client
			Gtk::CheckButton* cbConnect;
			Gtk::Entry* tbConnect;

			Gtk::CheckButton* cbPort;
			Gtk::Entry* tbPort;

			Gtk::CheckButton* cbPassword;
			Gtk::Entry* tbPassword;

			Gtk::CheckButton* cbHost;

		//Visible everywhere
			Gtk::Button* btnPlay;

			Gtk::Label* lblSelectedMods;
			Gtk::Label* lblStatus;

		class WorkshopModelColumns : public Gtk::TreeModel::ColumnRecord
		{
			public:
				WorkshopModelColumns() { add(enabled); add(name); add(workshopid); }

				Gtk::TreeModelColumn<bool> enabled;
				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<Glib::ustring> workshopid;
		};

		class CustomModelColumns : public Gtk::TreeModel::ColumnRecord
        {
            public:
                CustomModelColumns() { add(enabled); add(name); add(path); }

                Gtk::TreeModelColumn<bool> enabled;
                Gtk::TreeModelColumn<Glib::ustring> name;
                Gtk::TreeModelColumn<Glib::ustring> path;
        };

		WorkshopModelColumns workshopColumns;
		CustomModelColumns customColumns;

        std::vector<Mod> WorkshopMods;
        std::vector<Mod> CustomMods;

	public:
		MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);

	protected:
		bool ignore;

		void btnAdd_Clicked();
		void btnRemove_Clicked();

		bool onExit(GdkEventAny* event);
		void WorkshopToggleBox_Toggled(Glib::ustring path);
		void CustomToggleBox_Toggled(Glib::ustring path);

		void cbSkipIntro_Toggled();
		void cbNosplash_Toggled();
		void cbWindow_Toggled();
		void cbName_Toggled();
		void tbName_Changed();

		void cbParameterFile_Toggled();
		void tbParameterFile_Changed();
		void btnParameterFileBrowse_Clicked();

		void cbCheckSignatures_Toggled();

		void cbCpuCount_Toggled();
		void numCpuCount_Changed();

		void cbExThreads_Toggled();
		void cbExThreadsFileOperations_Toggled();
		void cbExThreadsTextureLoading_Toggled();
		void cbExThreadsGeometryLoading_Toggled();

		void cbEnableHT_Toggled();
		void cbFilePatching_Toggled();
		void cbNoLogs_Toggled();

		void cbWorld_Toggled();
		void tbWorld_Changed();

		void cbNoPause_Toggled();

		void cbConnect_Toggled();
		void tbConnect_Changed();

		void cbPort_Toggled();
		void tbPort_Changed();

		void cbPassword_Toggled();
		void tbPassword_Changed();

		void cbHost_Toggled();

		void btnPlay_Clicked();

		void RefreshStatusLabel();

		void Init();
};

#endif /* MAINWINDOW_H_ */
