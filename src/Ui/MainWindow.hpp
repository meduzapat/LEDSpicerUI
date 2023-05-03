/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.hpp
 * @since     Feb 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicerUI is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicerUI is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_ 1

#include "DialogDevice.hpp"
#include "DialogRestrictor.hpp"
#include "DialogGroup.hpp"
#include "XMLHelper.hpp"

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::MainWindow
 */
class MainWindow: public Gtk::ApplicationWindow {

public:

	MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder);

	virtual ~MainWindow();

protected:

	string
		dataDirectory,
		workingDirectory;

	static bool dirty;

	Gtk::HeaderBar* header = nullptr;

	Gtk::FileChooserDialog* dds = nullptr;

	Gtk::Entry
		* InputUserId     = nullptr,
		* InputGroupId    = nullptr,
		* InputPortNumber = nullptr;

	Gtk::CheckButton
		* InputCraftProfiles = nullptr,
		* InputColorsFile    = nullptr;

	Gtk::SpinButton* InputFPS = nullptr;

	Gtk::ComboBoxText
		* InputColors   = nullptr,
		* InputLogLevel = nullptr;

	Gtk::Button * btnSaveProject = nullptr;

	Gtk::ListBox* ListBoxDataSource = nullptr;

	// keep this dialogs only to clean up.
	DialogGroup*      dg  = nullptr;
	DialogDevice*     dd  = nullptr;
	DialogRestrictor* dr  = nullptr;

	/**
	 * Process the selected data directory.
	 * This directory should contain any data file, like colors, controls, etc.
	 * @param the dir name selected in the dialog.
	 * @returns true if the directory satisfies the required data files.
	 */
	void openDataDirectory(const string& dataDirectory);

	/**
	 * Populates the configuration.
	 * @param values if empty will use default values.
	 */
	void setConfiguration(unordered_map<string, string>& values);

	/**
	 * Process the selected working directory.
	 */
	void prepareWorkingDirectory(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Reads a ledspicer.conf file.
	 * @param ledspicerconf
	 * @param wipe if true, will clean before importing.
	 * @return the conf data.
	 * @throws LEDSpicerUI::Message
	 */
	unordered_map<string, string> import(const string& ledspicerconf, bool wipe);

	/**
	 * Sets the color file.
	 * @param colorFile
	 * @throws Message
	 */
	void setColorFile(const string& colorFile);

	/**
	 * Returns true if changes are unsaved
	 * @return
	 */
	bool isDirty();

	/**
	 * Sets the header info.
	 */
	void setHeader();

};

} /* namespace LEDSpicerUI */

#endif /* MAINWINDOW_HPP_ */
