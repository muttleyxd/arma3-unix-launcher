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

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) :
			Gtk::Window(cobject), builder(refGlade)
{
	Init();

	workshopModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("workshopModsStore"));
	customModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("customModsStore"));

	//Mods tab
	builder->get_widget("tvCustomMods", tvCustomMods);
	builder->get_widget("tvWorkshopMods", tvWorkshopMods);

	builder->get_widget("btnRefresh", btnRefresh);

	builder->get_widget("btnAdd", btnAdd);
	builder->get_widget("btnRemove", btnRemove);

	workshopToggleBox = Glib::RefPtr<Gtk::CellRendererToggle>::cast_dynamic(builder->get_object("workshopToggleBox"));

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
	builder->get_widget("cbFilePatching", cbFilePatching);
	builder->get_widget("cbNoLogs", cbNoLogs);

	builder->get_widget("cbWorld", cbWorld);
	builder->get_widget("tbWorld", tbWorld);

	builder->get_widget("cbProfile", cbProfile);
	builder->get_widget("tbProfile", tbProfile);
	builder->get_widget("btnProfileBrowse", btnProfileBrowse);

	builder->get_widget("cbNoPause", cbNoPause);

	//Client
	builder->get_widget("cbConnect", cbConnect);
	builder->get_widget("tbConnect", tbConnect);

	builder->get_widget("cbPort", cbPort);
	builder->get_widget("tbPort", tbPort);

	builder->get_widget("cbPassword", cbPassword);
	builder->get_widget("tbPassword", tbPassword);

	builder->get_widget("cbHost", cbHost);

	//Settings tab
	builder->get_widget("tbArmaPath", tbArmaPath);
	builder->get_widget("tbWorkshopPath", tbWorkshopPath);

	builder->get_widget("btnArmaPathBrowse", btnArmaPathBrowse);
	builder->get_widget("btnWorkshopPathBrowse", btnWorkshopPathBrowse);

	builder->get_widget("cbArmaPathAutodetect", cbArmaPathAutodetect);
	builder->get_widget("cbWorkshopPathAutodetect", cbWorkshopPathAutodetect);

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
	cbFilePatching->set_active(Settings::FilePatching);
	cbNoLogs->set_active(Settings::NoLogs);

	cbWorld->set_active(Settings::World);
	tbWorld->set_text(Settings::WorldValue);

	cbProfile->set_active(Settings::Profile);
	tbProfile->set_text(Settings::ProfileValue);

	cbNoPause->set_active(Settings::NoPause);

	cbConnect->set_active(Settings::Connect);
	tbConnect->set_text(Settings::ConnectValue);

	cbPort->set_active(Settings::Port);
	tbPort->set_text(Settings::PortValue);

	cbPassword->set_active(Settings::Password);
	tbPassword->set_text(Settings::PasswordValue);

	cbHost->set_active(Settings::Host);

	cbArmaPathAutodetect->set_active(Settings::ArmaPathAutodetect);
	cbWorkshopPathAutodetect->set_active(Settings::WorkshopPathAutodetect);

	tbArmaPath->set_text(Settings::ArmaPath);
	tbWorkshopPath->set_text(Settings::WorkshopPath);

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
	cbExThreadsFileOperations->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbExThreadsFileOperations_Toggled));
	cbExThreadsTextureLoading->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbExThreadsTextureLoading_Toggled));
	cbExThreadsGeometryLoading->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbExThreadsGeometryLoading_Toggled));

	cbEnableHT->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbEnableHT_Toggled));
	cbFilePatching->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbFilePatching_Toggled));
	cbNoLogs->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbNoLogs_Toggled));

	cbWorld->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbWorld_Toggled));
	tbWorld->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbWorld_Changed));

	cbProfile->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbProfile_Toggled));
	tbProfile->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbProfile_Changed));
	btnProfileBrowse->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnProfileBrowse_Clicked));

	cbNoPause->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbNoPause_Toggled));

	cbConnect->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbConnect_Toggled));
	tbConnect->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbConnect_Changed));

	cbPort->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbPort_Toggled));
	tbPort->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbPort_Changed));

	cbPassword->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbPassword_Toggled));
	tbPassword->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbPassword_Changed));

	cbHost->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbHost_Toggled));

	tbArmaPath->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbArmaPath_Changed));
	tbWorkshopPath->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tbWorkshopPath_Changed));

	btnArmaPathBrowse->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnArmaPathBrowse_Clicked));
	btnWorkshopPathBrowse->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnWorkshopPathBrowse_Clicked));

	cbArmaPathAutodetect->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbArmaPathAutodetect_Toggled));
	cbWorkshopPathAutodetect->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::cbWorkshopPathAutodetect_Toggled));


	/////Executing every event - need to make sure UI represents actual Settings
	ignore = true;
	cbSkipIntro_Toggled(); cbNosplash_Toggled(); cbWindow_Toggled(); cbName_Toggled(); tbName_Changed();
	cbParameterFile_Toggled(); tbParameterFile_Changed(); btnParameterFileBrowse_Clicked();
	cbCheckSignatures_Toggled(); cbCpuCount_Toggled(); numCpuCount_Changed();

	cbExThreads_Toggled(); cbExThreadsFileOperations_Toggled(); cbExThreadsTextureLoading_Toggled();
	cbExThreadsGeometryLoading_Toggled(); cbEnableHT_Toggled(); cbFilePatching_Toggled();
	cbNoLogs_Toggled(); cbWorld_Toggled(); tbWorld_Changed(); cbProfile_Toggled();
	tbProfile_Changed(); btnProfileBrowse_Clicked(); cbNoPause_Toggled(); cbConnect_Toggled();
	tbConnect_Changed(); cbPort_Toggled(); tbPort_Changed(); cbPassword_Toggled();
	tbPassword_Changed(); cbHost_Toggled(); tbArmaPath_Changed(); tbWorkshopPath_Changed();
	btnArmaPathBrowse_Clicked(); btnWorkshopPathBrowse_Clicked(); cbArmaPathAutodetect_Toggled();
	cbWorkshopPathAutodetect_Toggled();
	ignore = false;
	/////

	set_title("ArmA 3 Linux Launcher");
	set_default_size(Settings::WindowSizeX, Settings::WindowSizeY);
	move(Settings::WindowPosX, Settings::WindowPosY);
	tvWorkshopMods->columns_autosize();
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnAdd_Click));

	for (Mod m: Mods)
	{
		Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->append());
		row[workshopColumns.enabled] = false;
		row[workshopColumns.name] = m.Name;
		row[workshopColumns.workshopid] = m.WorkshopId;
	}
	workshopToggleBox->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::hey));

	this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::onExit));
}

void MainWindow::btnAdd_Click()
{
	Gtk::MessageDialog dialog(*this, "This is 123", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_YES_NO, true);
	dialog.set_secondary_text("Hey there");

	int result = dialog.run();

}

bool MainWindow::onExit(GdkEventAny* event)
{
	this->get_position(Settings::WindowPosX, Settings::WindowPosY);
	this->get_size(Settings::WindowSizeX, Settings::WindowSizeY);
	Settings::Save(Filesystem::HomeDirectory
					+ Filesystem::LauncherSettingsDirectory
					+ Filesystem::LauncherSettingsFilename);
	return false;
}

void MainWindow::hey(Glib::ustring path) //path is index number
{
	std::cout << path << std::endl;
	Gtk::TreeModel::Row row = *(workshopModsStore.operator ->()->get_iter(path));
	row[workshopColumns.enabled] = !row[workshopColumns.enabled];
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
	if (!ignore) Settings::ExThreadsFileOperations = cbExThreadsFileOperations->get_active();
}

void MainWindow::cbExThreadsTextureLoading_Toggled()
{
	LOG(0, "cbExThreadsTextureLoading_Toggled: " + Utils::ToString(cbExThreadsTextureLoading->get_active()));
	if (!ignore) Settings::ExThreadsTextureLoading = cbExThreadsTextureLoading->get_active();
}

void MainWindow::cbExThreadsGeometryLoading_Toggled()
{
	LOG(0, "cbExThreadsGeometryLoading_Toggled: " + Utils::ToString(cbExThreadsGeometryLoading->get_active()));
	if (!ignore) Settings::ExThreadsGeometryLoading = cbExThreadsGeometryLoading->get_active();
}

void MainWindow::cbEnableHT_Toggled()
{
	LOG(0, "cbEnableHT_Toggled: " + Utils::ToString(cbEnableHT->get_active()));
	if (!ignore) Settings::EnableHT = cbEnableHT->get_active();
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

void MainWindow::cbProfile_Toggled()
{
	LOG(0, "cbProfile_Toggled: " + Utils::ToString(cbProfile->get_active()));
	if (!ignore) Settings::Profile = cbProfile->get_active();
	tbProfile->set_sensitive(Settings::Profile);
	btnProfileBrowse->set_sensitive(Settings::Profile);
}

void MainWindow::tbProfile_Changed()
{
	LOG(0, "tbProfile_Changed: " + tbProfile->get_text());
	if (!ignore) Settings::ProfileValue = tbProfile->get_text();
}

void MainWindow::btnProfileBrowse_Clicked()
{
	LOG(0, "btnProfileBrowse_Clicked");
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

void MainWindow::tbArmaPath_Changed()
{
	LOG(0, "tbArmaPath_Changed: " + tbArmaPath->get_text());
	if (!ignore) Settings::ArmaPath = tbArmaPath->get_text();
}

void MainWindow::tbWorkshopPath_Changed()
{
	LOG(0, "tbWorkshopPath_Changed: " + tbWorkshopPath->get_text());
	if (!ignore) Settings::WorkshopPath = tbWorkshopPath->get_text();
}

void MainWindow::btnArmaPathBrowse_Clicked()
{
	LOG(0, "btnArmaPathBrowse_Clicked");
}

void MainWindow::btnWorkshopPathBrowse_Clicked()
{
	LOG(0, "btnWorkshopPathBrowse_Clicked");
}

void MainWindow::cbArmaPathAutodetect_Toggled()
{
	LOG(0, "cbArmaPathAutodetect_Toggled: " + Utils::ToString(cbArmaPathAutodetect->get_active()));
	if (!ignore) Settings::ArmaPathAutodetect = cbArmaPathAutodetect->get_active();
	tbArmaPath->set_sensitive(!Settings::ArmaPathAutodetect);
	btnArmaPathBrowse->set_sensitive(!Settings::ArmaPathAutodetect);
}

void MainWindow::cbWorkshopPathAutodetect_Toggled()
{
	LOG(0, "cbWorkshopPathAutodetect_Toggled: " + Utils::ToString(cbWorkshopPathAutodetect->get_active()));
	if (!ignore) Settings::WorkshopPathAutodetect = cbWorkshopPathAutodetect->get_active();
	tbWorkshopPath->set_sensitive(!Settings::WorkshopPathAutodetect);
	btnWorkshopPathBrowse->set_sensitive(!Settings::WorkshopPathAutodetect);
}

void MainWindow::Init()
{
	if (Settings::ArmaPathAutodetect)
		Settings::ArmaPath = Filesystem::GetDirectory(DirectoryToFind::ArmaInstall);
	if (Settings::WorkshopPathAutodetect)
		Settings::WorkshopPath = Filesystem::GetDirectory(DirectoryToFind::WorkshopMods);

	LOG(1, "ArmA 3 Path: " + Settings::ArmaPath + "\nWorkshop mods path: " + Settings::WorkshopPath);

	Mods = Filesystem::FindMods(Settings::WorkshopPath);
	Filesystem::CheckFileStructure(Settings::ArmaPath, Settings::WorkshopPath, Mods);
}
