/*
 * MainWindow.cpp
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */

#include "MainWindow.h"
#include <iostream>
#include "Settings.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) :
			Gtk::Window(cobject), builder(refGlade)
{
	workshopModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("workshopModsStore"));
	customModsStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("customModsStore"));

	builder->get_widget("tvCustomMods", tvCustomMods);
	builder->get_widget("tvWorkshopMods", tvWorkshopMods);

	builder->get_widget("btnRefresh", btnRefresh);

	builder->get_widget("btnAdd", btnAdd);
	builder->get_widget("btnRemove", btnRemove);
	builder->get_widget("btnPlay", btnPlay);

	builder->get_widget("lblSelectedMods", lblSelectedMods);
	builder->get_widget("lblStatus", lblStatus);

	set_title("ArmA 3 Linux Launcher");
	set_default_size(Settings::WindowSizeX, Settings::WindowSizeY);
	move(Settings::WindowPosX, Settings::WindowPosY);

	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::btnAdd_Click));
	//this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::on_exit));
	//signal_delete_event().connect(sigc::mem_fun(*this, &OnExit));
}

void MainWindow::btnAdd_Click()
{
	Gtk::MessageDialog dialog(*this, "This is 123", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_YES_NO, true);
	dialog.set_secondary_text("Hey there");

	int result = dialog.run();

}
