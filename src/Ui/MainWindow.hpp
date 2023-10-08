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

#include "DialogImport.hpp"
#include "DialogDevice.hpp"
#include "DialogRestrictor.hpp"
#include "DialogProcess.hpp"
#include "DialogGroup.hpp"
#include "DialogInput.hpp"
#include "XMLHelper.hpp"
#include "OrdenableListBox.hpp"

// for file and dir stats
#include <sys/stat.h>
#include <sys/types.h>

// for file accessing.
#include <iostream>
#include <fstream>

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

	DialogImport dic;
	Gtk::FileChooserDialog dds;

	Gtk::Entry
		* InputUserId     = nullptr,
		* InputGroupId    = nullptr,
		* InputPortNumber = nullptr,
		* InputRunEvery   = nullptr;

	Gtk::ToggleButton
		* InputCraftProfiles = nullptr,
		* InputColorsFile    = nullptr;

	Gtk::SpinButton* InputFPS = nullptr;

	Gtk::ComboBoxText
		* InputColors   = nullptr,
		* InputLogLevel = nullptr;

	OrdenableListBox* ListBoxDataSource = nullptr;


	// keep this dialogs only to clean up.
	DialogDevice*     dd = nullptr;
	DialogRestrictor* dr = nullptr;
	DialogProcess*    dp = nullptr;
	DialogGroup*      dg = nullptr;
	DialogInput*      di = nullptr;

	Gtk::FlowBox* boxRandomColors = nullptr;

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
	 * @return a XML string with the configuration.
	 */
	string readConfiguration();

	/**
	 * Process the selected working directory.
	 * @param builder
	 */
	void prepareWorkingDirectory(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Reads a ledspicer.conf file.
	 * @param ledspicerconf
	 * @param wipe if true, will clean before importing.
	 * @return the conf data.
	 * @throws LEDSpicerUI::Message
	 */
	void import(const string& ledspicerconf, bool wipe, uint8_t importFlags);

	/**
	 * Sets the color file.
	 * @param colorFile
	 * @throws Message
	 */
	void setColorFile(const string& colorFile);

};

} /* namespace LEDSpicerUI */

#endif /* MAINWINDOW_HPP_ */
