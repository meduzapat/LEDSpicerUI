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

#include "Settings.hpp"
#include "XMLHelper.hpp"

#pragma once

#define UI_CONFIG_FILE "ledspicerui.conf"
#define UI_CONFIG_TYPE "Settings"

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::SettingsFile
 *
 * Handles UI configuration file I/O.
 * Always call initialize or set the paths manually.
 * Construction loads from file into Settings.
 */
class SettingsFile : public XMLHelper {

public:

	SettingsFile();

	/**
	 * One-call startup entry point.
	 * Loads from file or applies defaults.
	 *
	 * @return true if a config file was found and loaded, false on first run.
	 */
	static bool initialize();

	/**
	 * Dumps to storage.
	 */
	static void save();

	/**
	 * Overrides the resolved path (used by tests). Pass empty string to reset.
	 *
	 * @param path
	 */
	static void setSettingsPath(const string& path) noexcept;

	/**
	 * @return the override path if set, otherwise the user config path.
	 */
	static const string& getSettingsPath() noexcept;

	/**
	 * @return True if getSettingsPath() exists on disk.
	 */
	static bool configExists() noexcept;

private:

	static string settingFilePath;

	static void ensureConfigDir() noexcept;
};

} // namespace
