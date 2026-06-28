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

#include "config/ConfigFile.hpp"

#include "SortableListBox.hpp"
#include "DialogSettings.hpp"
#include "DialogProject.hpp"
#include "DialogPrompt.hpp"
#include "InputDirectoryNavigator.hpp"
#include "AnimationDirectoryNavigator.hpp"
#include "ProfileDirectoryNavigator.hpp"

#include "DataDialogs/DialogDevice.hpp"
#include "DataDialogs/DialogRestrictor.hpp"
#include "DataDialogs/DialogProcess.hpp"
#include "DataDialogs/DialogGroup.hpp"
#include "Layout/Layout.hpp"
#include "DaemonHandler.hpp"
#include "DaemonSandbox.hpp"
#include "StatusBar.hpp"

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
	SortableListBox* listBoxDataSource = nullptr;

	/// Contains the possible pickers for colors when selecting colors randomly.
	Gtk::FlowBox* boxRandomColors = nullptr;

	/// Import configuration dialog.
	DialogImport dialogImportConfig;

	Gtk::Stack*  mainTabs       = nullptr;
	Gtk::Box*    mainTabsBox    = nullptr;
	Gtk::Button* btnImportConfig = nullptr;

	/// Daemon connection toggle (top bar).
	Gtk::ToggleButton* toggleConnect = nullptr;
	/// Guards reentrancy while reverting toggleConnect in code.
	bool ignoreConnectToggle = false;

	/// Modal busy indicator (from glade); its label text is swapped per operation.
	Gtk::Window* busyWindow = nullptr;
	Gtk::Label*  busyLabel  = nullptr;

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

	/// Visual board controller. Subscribed to Storage::Element lifecycle events.
	Layout::Layout layout;

	/// Isolated test environment for the daemon; exists only in interactive mode.
	std::unique_ptr<DaemonSandbox> sandbox;

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
	 * Handles the daemon connect/disconnect toggle, reverting on failure.
	 */
	void onConnectToggled();

	/**
	 * Creates or drops the test sandbox to match interactive mode.
	 */
	void syncSandbox() noexcept;

	/**
	 * Regenerates the test config and launches the daemon.
	 * @return true once our daemon is up.
	 * @throws Message if the config cannot be written.
	 */
	bool launchDaemon();

	/**
	 * Connects for a new test session (busy spinner, status, enables testing).
	 * @return true on success.
	 */
	bool connectDaemon() noexcept;

	/**
	 * Daemon readiness gate installed on DaemonHandler: command() asks before
	 * each send. Reports whether the daemon is online and, refreshing it in
	 * place when the base configuration drifted, fresh.
	 * @return true if a test may fire now.
	 */
	bool ensureDaemonReady() noexcept;

	/**
	 * Refreshes a stale daemon in place before a test; drops the link on failure.
	 * @return true if testing is usable afterwards.
	 */
	bool reconnectDaemon() noexcept;

	/**
	 * Stages the config and runs the rotator with positional player/joystick/ways args.
	 * @param positional flat token list passed after `rotator -c <conf>`.
	 * @param output     captured rotator output (stdout with stderr appended).
	 * @return true if the rotator ran and exited cleanly.
	 */
	bool runRotatorTest(const StringVector& positional, string& output) noexcept;

	/**
	 * Shows/enables the connect toggle per mode (hidden/disabled/enabled).
	 */
	void updateDaemonControls() noexcept;

	/**
	 * Stales the test daemon when daemon-relevant data (devices, elements,
	 * groups, port) changes, so it refreshes on the next test.
	 */
	void onDaemonConfigChanged() noexcept;

	/**
	 * Shows the modal busy spinner with text and paints it before blocking.
	 */
	void showBusy(const Glib::ustring& text) noexcept;

	/**
	 * Hides the modal busy spinner.
	 */
	void hideBusy() noexcept;

	/**
	 * Connects Dialogs with buttons.
	 * @param builder
	 */
	void prepareDialogs(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Opens a project by name: sets current project, loads its config, and activates the UI.
	 * Persists the project to UI settings immediately if the config file already exists.
	 * @param name project directory name
	 */
	void openProject(const string& name);

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
