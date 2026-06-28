/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSettings.hpp
 * @since     Feb 6, 2026
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

#include "config/SettingsFile.hpp"
#include "GladeDialog.hpp"
#include "StatusBar.hpp"
#include "ThemeManager.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogSettings
 * Handles LEDSpicerUI settings: binary detection, data directory.
 * All persistent state lives in Settings; this class owns only UI widgets.
 */
class DialogSettings : public GladeDialog<DialogSettings> {

	friend class Gtk::Builder;

public:

	virtual ~DialogSettings() = default;

	/**
	 * Full startup flow: load from disk, auto-detect, or show first-run welcome dialog.
	 * @param parent Window to parent any dialogs shown during startup.
	 * @return false if the application should exit, true to continue.
	 */
	bool startup(Gtk::Window* parent);

	/**
	 * Searches PATH for ledspicerd and auto-configures if found.
	 * Saves settings if the result is valid.
	 * @return true if binary was found and data directory is valid.
	 */
	bool autoDetect();

	/**
	 * Calls SettingsFile::initialize() then syncs widgets from Settings.
	 * @return true if a config file was found and loaded.
	 */
	bool loadSettings();

	/**
	 * Pushes current Settings to disk via SettingsFile::save().
	 */
	void saveSettings();

	bool isValid() const;

	/**
	 * Registers a callback fired after the settings dialog closes, so the owner
	 * can re-evaluate state that depends on settings (e.g. the daemon controls).
	 * @param cb the callback.
	 */
	void onClose(std::function<void()> cb) noexcept { onClosed = std::move(cb); }

	/**
	 * Sets the binary path, runs detection, and updates UI.
	 * @param binaryPath
	 * @param setFileBinarySelector if true, syncs the file chooser widget.
	 */
	void setBinaryPath(const string& binaryPath, bool setFileBinarySelector);

	/**
	 * Sets the config file path and updates the label.
	 * @param configPath
	 */
	void setConfigPath(const string& configPath);

protected:

	Gtk::Label
		* labelBinaryPath   = nullptr,
		* labelBinaryStatus = nullptr,
		* labelConfigPath   = nullptr,
		* labelDataPath     = nullptr,
		* labelSystemFiles  = nullptr;

	Gtk::FileChooserButton
		* fileBinary         = nullptr,
		* fileDataDirSelect  = nullptr,
		* fileThemeDirSelect = nullptr;

	Gtk::Button* btnThemeRefresh = nullptr;

	Gtk::Switch
		* switchInteractiveMode    = nullptr,
		* switchPreserveEmptyDir   = nullptr,
		* switchRemoveInvalidItems = nullptr,
		* switchSaveBackup         = nullptr,
		* switchDebugFiles         = nullptr,
		* switchDebugHardwareTest  = nullptr;

	Gtk::ToggleButton
		* btnStyleAuto  = nullptr,
		* btnStyleLight = nullptr,
		* btnStyleDark  = nullptr;

	Gtk::Scale* scaleLayoutGrid = nullptr;

	Gtk::SpinButton* spinLayoutTestTimeout = nullptr;

	Gtk::FlowBox* flowBoxThemes = nullptr;

	/// Fired after the dialog closes, when set.
	std::function<void()> onClosed;

	bool
		/// Guards against recursive signal firing when syncing style buttons.
		settingStyle    = false,
		/// Guards against recursive signal firing when syncing theme selection.
		selectingTheme  = false,
		/// True while signal_show() is mirroring Settings into widgets; suppresses auto-save.
		syncing         = false;

	DialogSettings(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	bool detectLedspicerVersion();
	void updateBinaryStatusLabel(const string& version);
	void processBinary();
	void setDataDir(const string& dataDir, bool setFileDataDirSelector);
	void updateDataDirLabels();
	void processDataDir();

	/**
	 * Persists Settings and pushes a transient status-bar confirmation.
	 * No-op while syncing widgets from Settings during signal_show().
	 */
	void commit(const string& message, StatusBar::Severity severity = StatusBar::Severity::Success);

	/**
	 * Clears BoxSelectTheme and rebuilds the None tile plus one tile per discovered theme.
	 */
	void rebuildThemeTiles();
	/**
	 * Appends a single selectable tile to BoxSelectTheme. Empty previewPath renders a placeholder.
	 */
	void addThemeTile(const string& id, const string& name, const string& previewPath, const string& tooltip);
	/**
	 * Selects the tile matching the current theme name, if present.
	 */
	void selectCurrentThemeTile();
	/**
	 * Rescans the themes directory, applies the None-fallback, and refreshes the tiles.
	 */
	void refreshThemes();
	/**
	 * Syncs style toggle buttons to current Settings without firing apply.
	 */
	void syncStyleButtons();
	/**
	 * Applies the current theme name + style from Settings via ThemeManager.
	 */
	void applyCurrentTheme();
};

} // namespace
