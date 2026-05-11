/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Settings.hpp
 * @since     May 10, 2026
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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::Settings
 * Central singleton for all persistent application-level settings,
 * providing helper wrappers for easy access.
 */
class Settings : protected Values {

public:

	inline const string
		PATH_BINARY  {"binaryPath"},
		PATH_DATA    {"dataDir"},
		PATH_PROJECT {"projectsDir"},

		INTERACTIVE_MODE     {"interactiveMode"},
		CLEAN_PROJECT_DIR    {"cleanProjectDir"},
		PRESERVE_EMPTY_DIR   {"preserveEmptyDir"},
		REMOVE_INVALID_ITEMS {"removeInvalidItems"},
		SAVE_BACKUP          {"saveBackup"},
		DEBUG_FILES          {"debugFiles"},
		THEME_STYLE          {"themeStyle"};

	/// Application mode, derived from binary detection and interactiveMode.
	enum class Mode {
		Local,     /// ledspicerd detected, interactive mode OFF.
		Iterative, /// ledspicerd detected, interactive mode ON.
		Portable   /// ledspicerd not set or not detected.
	};

	/// UI theme style.
	enum class ThemeStyle { Auto, Light, Dark };

	Settings(const Settings&) = delete;
	Settings& operator=(const Settings&) = delete;

	/// Returns the single application-wide instance.
	static Settings& getInstance() noexcept { return instance; }

	const string& getBinaryPath()  const noexcept { return values.at(PATH_BINARY);  }
	const string& getDataDir()     const noexcept { return values.at(PATH_DATA);    }
	const string& getProjectsDir() const noexcept { return values.at(PATH_PROJECT); }

	void setBinaryPath(const string& path) noexcept;
	void setDataDir(const string& dir)     noexcept;
	void setProjectsDir(const string& dir) noexcept;

	bool isPortable()    const noexcept { return getMode() == Settings::Mode::Portable;  }
	bool isInteractive() const noexcept { return getMode() == Settings::Mode::Iterative; }
	Mode getMode()       const noexcept { return currentMode; }

	void setInteractiveMode(bool value) noexcept;

	ThemeStyle getThemeStyle() const noexcept;
	void setThemeStyle(ThemeStyle style) noexcept;

	bool shouldCleanProjectDir()    const noexcept { return values.at(CLEAN_PROJECT_DIR)    == HUMAN_TRUE; }
	bool shouldPreserveEmptyDir()   const noexcept { return values.at(PRESERVE_EMPTY_DIR)   == HUMAN_TRUE; }
	bool shouldRemoveInvalidItems() const noexcept { return values.at(REMOVE_INVALID_ITEMS) == HUMAN_TRUE; }
	bool shouldSaveBackup()         const noexcept { return values.at(SAVE_BACKUP)          == HUMAN_TRUE; }
	bool shouldDebugFiles()         const noexcept { return values.at(DEBUG_FILES)          == HUMAN_TRUE; }

	void setCleanProjectDir(bool value)    noexcept;
	void setPreserveEmptyDir(bool value)   noexcept;
	void setRemoveInvalidItems(bool value) noexcept;
	void setSaveBackup(bool value)         noexcept;
	void setDebugFiles(bool value)         noexcept;

protected:

	// LEDSpicer runtime
	Mode currentMode = Mode::Portable;

	Settings()  = default;
	~Settings() = default;

private:

	static Settings instance;

	/**
	 * Recomputes currentMode from binaryPath and interactiveMode.
	 * Portable  → no binary path set.
	 * Local     → binary detected, interactive mode OFF.
	 * Iterative → binary detected, interactive mode ON.
	 */
	void updateMode() noexcept;
};

} // namespace
