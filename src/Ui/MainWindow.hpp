/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.hpp
 * @since     Feb 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "OrdenableListBox.hpp"
#include "DialogSettings.hpp"
#include "DialogProject.hpp"
#include "InputDirectoryNavigator.hpp"
#include "AnimationDirectoryNavigator.hpp"
#include "ProfileDirectoryNavigator.hpp"

#include "DataDialogs/DialogDevice.hpp"
#include "DataDialogs/DialogRestrictor.hpp"
#include "DataDialogs/DialogProcess.hpp"
#include "DataDialogs/DialogGroup.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

using LEDSpicerUI::Config::ConfigFile;

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

	Gtk::Entry
		* inputUserId     = nullptr,
		* inputGroupId    = nullptr,
		* inputPortNumber = nullptr,
		* inputRunEvery   = nullptr;

	Gtk::ToggleButton* toggleCraftProfiles = nullptr;

	Gtk::SpinButton* inputFPS = nullptr;

	Gtk::ComboBoxText
		* comboUseColors = nullptr, /// colors.ini usage selector.
		* comboColors    = nullptr,
		* comboLogLevel  = nullptr;

	/// The list of data sources for ROM names.
	OrdenableListBox* listBoxDataSource = nullptr;

	/// Contains the possible pickers for colors when selecting colors randomly.
	Gtk::FlowBox* boxRandomColors = nullptr;

	/// Import configuration dialog.
	DialogImport dialogImportConfig;

	/// @name Storage Collections
	Storage::BoxButtonCollection
		/// Created devices in the dialog devices.
		devices,
		/// Created restrictors in the dialog restrictors.
		restrictors,
		/// Created processes in the dialog processes.
		processes,
		/// Created groups in the dialog groups.
		groups;

	/// Navigator for input files.
	InputDirectoryNavigator inputNavigator;

	/// Navigator for animation files.
	AnimationDirectoryNavigator animationNavigator;

	/// Navigator for profile files.
	ProfileDirectoryNavigator profileNavigator;

	/**
	 * Populates the configuration.
	 * @param values if empty will use default values.
	 */
	void setConfiguration(const Values& values);

	/**
	 * @return the generated configuration.
	 */
	Values packLedspicerConfig() const noexcept;

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
	 * Fill the colors combo box with available color profiles.
	 */
	void populateColorsCombo();
};

} // namespace
