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

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogSettings
 * Handles LEDSpicerUI settings: binary detection, data directory.
 */
class DialogSettings : public GladeDialog<DialogSettings> {

	friend class Gtk::Builder;

public:

	virtual ~DialogSettings() = default;

	const string& getBinaryPath() const;
	const string& getDataDir() const;
	const string& getConfigPath() const;
	const StringVector& getColorFiles() const;

	/**
	 * Sets the binary path and updates the label.
	 * @param binaryPath the path to the ledspicerd binary.
	 * @param setFileBinarySelector if true, will update the file selector to match the new path.
	 */
	void setBinaryPath(const string& binaryPath, bool setFileBinarySelector);

	/**
	 * Sets the config file path and updates the label.
	 * @param configPath the path to the config file.
	 */
	void setConfigPath(const string& configPath);

	/**
	 * Loads settings from config file into dialog.
	 * @return true if settings were loaded successfully, false otherwise.
	 */
	bool loadSettings();

	/**
	 * Saves current dialog values to config file.
	 */
	void saveSettings();

	bool isValid() const;

protected:

	string
		binaryPath, /// The path to the ledspicerd binary (if available).
		dataDir,    /// Location of the data directory, should contain colors.ini, controls.ini, gameData.xml and color XML files.
		configPath; /// The path to the ledspicer.conf file.

	/// The status of the data directory.
	struct DataDirStatus {
		bool
			hasGameData = false, /// True if gameData.xml is found in the data directory.
			hasColors   = false, /// True if colors.ini is found in the data directory.
			hasControls = false; /// True if controls.ini is found in the data directory.
		StringVector colorFiles; /// List of color XML files found in the data directory.
	}
	dataDirStatus;

	Gtk::Label
		* labelBinaryPath   = nullptr,
		* labelBinaryStatus = nullptr,
		* labelConfigPath   = nullptr,
		* labelDataPath     = nullptr,
		* labelSystemFiles  = nullptr;

	Gtk::FileChooserButton
		* fileBinary        = nullptr,
		* fileDataDirSelect = nullptr;

	Gtk::Button* btnApply = nullptr;

	DialogSettings(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	/**
	 * Detects the ledspicerd version by running it and verifying that it is a valid ledspicerd binary.
	 * @return true if ledspicerd was detected and version was extracted successfully.
	 */
	bool detectLedspicerVersion();

	/**
	 * Updates the binary status label based on the detected version.
	 * @param version the detected version of ledspicerd.
	 */
	void updateBinaryStatusLabel(const string& version);

	/**
	 * Extracts dataDir and projectsDir from binary -h output.
	 */
	void processBinary();

	/**
	 * Sets the data directory and updates the status.
	 * @param dataDir the path to the data directory.
	 * @param setFileDataDirSelector if true, will update the file selector to match the new path.
	 */
	void setDataDir(const string& dataDir, bool setFileDataDirSelector); // FIX: was string&

	/**
	 * Updates the data directory status labels based on the current dataDirStatus.
	 */
	void updateDataDirLabels();

	/**
	 * Validates data directory contents.
	 * Populates dataDirStatus with findings.
	 */
	void processDataDir();

	void updateApplyButton();
};

} // namespace
