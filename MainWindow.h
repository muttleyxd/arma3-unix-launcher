/*
 * MainWindow.h
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
	protected:
		Glib::RefPtr<Gtk::Builder> builder;

		Glib::RefPtr<Gtk::ListStore> workshopModsStore;
		Glib::RefPtr<Gtk::ListStore> customModsStore;

		Gtk::TreeView* tvWorkshopMods;
		Gtk::TreeView* tvCustomMods;

		Gtk::Button* btnRefresh;

		Gtk::Button* btnAdd;
		Gtk::Button* btnRemove;

		Gtk::Button* btnPlay;

		Gtk::Label* lblSelectedMods;
		Gtk::Label* lblStatus;

	public:
		MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);

	protected:
		void btnAdd_Click();
};

#endif /* MAINWINDOW_H_ */
