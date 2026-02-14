/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.hpp
 * @since     Feb 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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



// for file and dir stats
#include <sys/stat.h>
#include <sys/types.h>

// for file accessing.
#include <iostream>
#include <fstream>

#include "config/ConfigFile.hpp"
#include "config/InputFile.hpp"

#include "OrdenableListBox.hpp"
#include "DialogSettings.hpp"
#include "DialogProject.hpp"
#include "DialogImport.hpp"
#include "DataDialogs/DialogDevice.hpp"
#include "DataDialogs/DialogRestrictor.hpp"
#include "DataDialogs/DialogProcess.hpp"
#include "DataDialogs/DialogGroup.hpp"
#include "DataDialogs/DialogInput.hpp"
#include "DataDialogs/DialogProfile.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::MainWindow
 */
class MainWindow: public Gtk::ApplicationWindow {

public:

	// Configuration import flags.
	static constexpr int IMPORT_ALL = 15; // CONFIG | DEVICES | RESTRICTORS | MAPPINGS

	MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder);

	virtual ~MainWindow();

protected:

	/// The name of the current project.
	string currentProjectName;

	Gtk::Entry
		* inputUserId     = nullptr,
		* inputGroupId    = nullptr,
		* inputPortNumber = nullptr,
		* inputRunEvery   = nullptr;

	Gtk::ToggleButton * toggleCraftProfiles = nullptr;

	Gtk::SpinButton* inputFPS = nullptr;

	Gtk::ComboBoxText
		* comboUseColors      = nullptr, /// colors.ini usage selector.
		* comboColors         = nullptr,
		* comboLogLevel       = nullptr,
		* comboDefaultProfile = nullptr;

	/// The list of data sources for ROM names.
	OrdenableListBox* listBoxDataSource = nullptr;

	/// Contains the possible pickers for colors when selecting colors randomly.
	Gtk::FlowBox* boxRandomColors = nullptr;

	DialogImport
		/// Import configuration dialog.
		dialogImportConfig,
		/// Import input dialog.
		dialogImportInput;

	/// @name Storage Collections
	Storage::BoxButtonCollection
		/// Created devices in the dialog devices.
		devices,
		/// Created restrictors in the dialog restrictors.
		restrictors,
		/// Created processes in the dialog processes.
		processes,
		/// Created groups in the dialog groups.
		groups,
		/// Created inputs in the dialog inputs.
		inputs,
		/// Created profiles in the dialog profiles.
		profiles;

	/**
	 * Populates the configuration.
	 * @param values if empty will use default values.
	 */
	void setConfiguration(StringUMap& values);

	/**
	 * @return the generated configuration.
	 */
	StringUMap ledspicerConfigToXml() const;

	/**
	 * Connects Dialogs with buttons.
	 * @param builder
	 */
	void prepareDialogs(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Reads a ledspicer.conf file.
	 * @param ledspicerconf
	 * @param wipe if true, will clean before importing.
	 * @return the conf data.
	 * @throws LEDSpicerUI::Message
	 */
	void readConfigFile(const string& ledspicerconf, bool wipe, uint8_t importFlags);

	/**
	 * Sets the color file.
	 * @param colorFile if empty will remove all colors.
	 * @throws Message
	 */
	void setColorFile(const string& colorFile);

	/**
	 * Fill the colors combo box with available color profiles.
	 */
	void populateColorsCombo();
};

} // namespace
