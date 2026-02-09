/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SettingsFile.hpp
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

#include "XMLHelper.hpp"

#pragma once

#define UI_CONFIG_FILE "ledspicerui.conf"

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::SettingsFile
 * Handles UI configuration file.
 */
class SettingsFile : public XMLHelper {

public:

	SettingsFile() = delete;

	/**
	 * Loads settings from config file.
	 * @param settingsFile Path to config file.
	 */
	SettingsFile(const string& settingsFile);

	virtual ~SettingsFile() = default;

	// getSettings() inherited from XMLHelper

	/**
	 * Saves settings to file.
	 * @param settingsFile Path to config file.
	 * @param values Settings to save.
	 * @throws Message on write error.
	 */
	static void save(const string& settingsFile, const StringUMap& values);

	/**
	 * @return Full path to config file.
	 */
	static string getConfigFilePath();

	/**
	 * @return True if config file exists.
	 */
	static bool configExists();

protected:

	/**
	 * Ensures config directory exists.
	 * @throws Glib::Error if directory creation fails.
	 */
	static void ensureConfigDir();
};

} // namespace
