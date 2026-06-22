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

#include "Values.hpp"

#pragma once

#define CONFIG_FILE "ledspicer.conf"

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::Settings
 *
 * Central singleton for all application settings.
 *
 * Persistent settings (saved to disk via SettingsFile) live in the inherited Values map.
 * Volatile settings (runtime state, never saved) live as typed members.
 * Computed properties are derived on demand from both.
 */
class Settings : protected Values {

public:

	/// Keys for persistent settings stored in the Values map.
	static inline const string
		PATH_BINARY          {"binaryPath"},
		PATH_DATA            {"dataDir"},
		PATH_THEME           {"themePath"},
		PATH_PROJECT         {"projectsDir"},
		DEFAULT_PROJECT      {"defaultProject"},
		INTERACTIVE_MODE     {"interactiveMode"},
		THEME_NAME           {"themeName"},
		THEME_STYLE          {"themeStyle"},
		PRESERVE_EMPTY_DIR   {"preserveEmptyDir"},
		REMOVE_INVALID_ITEMS {"removeInvalidItems"},
		SAVE_BACKUP          {"saveBackup"},
		DEBUG_FILES          {"debugFiles"},
		DEBUG_DAEMON         {"debugDaemon"},
		LAYOUT_GRID          {"layoutGrid"},
		LAYOUT_TEST_TIMEOUT  {"layoutTestTimeout"};

	/// Runtime mode — derived from binary detection result and interactiveMode preference.
	enum class Mode {
		Portable,  /// Binary absent or detection failed; config lives in the project dir.
		Local,     /// Binary detected, interactive mode OFF.
		Iterative  /// Binary detected, interactive mode ON.
	};

	/// UI theme preference.
	enum class ThemeStyle { Auto, Light, Dark };

	Settings(const Settings&)            = delete;
	Settings& operator=(const Settings&) = delete;

	static Settings& get() noexcept { return instance; }

	/**
	 * Replaces persistent settings with sanitized values from SettingsFile.
	 * Missing keys receive hard-coded defaults. Unknown keys are ignored.
	 * Recomputes currentMode after loading.
	 */
	void load(const Values& source) noexcept;

	/**
	 * @return Settings demoted to Values.
	 */
	const Values& asValues() const noexcept { return *this; }

	auto begin() const noexcept { return values.cbegin(); }
	auto end()   const noexcept { return values.cend();   }

	const string& getBinaryPath()     const noexcept { return getValue(PATH_BINARY);     }
	const string& getDataDir()        const noexcept { return getValue(PATH_DATA);       }
	const string& getProjectsDir()    const noexcept { return getValue(PATH_PROJECT);    }
	const string& getDefaultProject() const noexcept { return getValue(DEFAULT_PROJECT); }

	/**
	 * User-configurable themes directory. Falls back to the in-config-dir default when unset.
	 */
	string getThemePath() const noexcept;

	void setBinaryPath(const string& path)     noexcept; ///< Also recomputes currentMode.
	void setDataDir(const string& dir)         noexcept;
	void setThemePath(const string& dir)       noexcept;
	void setProjectsDir(const string& dir)     noexcept;
	void setDefaultProject(const string& name) noexcept;

	bool isInteractiveMode()        const noexcept { return is(INTERACTIVE_MODE);     }
	bool shouldPreserveEmptyDir()   const noexcept { return is(PRESERVE_EMPTY_DIR);   }
	bool shouldRemoveInvalidItems() const noexcept { return is(REMOVE_INVALID_ITEMS); }
	bool shouldSaveBackup()         const noexcept { return is(SAVE_BACKUP);          }
	bool shouldDebugFiles()         const noexcept { return is(DEBUG_FILES);          }
	bool shouldDebugDaemon()        const noexcept { return is(DEBUG_DAEMON);         }

	void setInteractiveMode(bool value)    noexcept; ///< Also recomputes currentMode.
	void setPreserveEmptyDir(bool value)   noexcept;
	void setRemoveInvalidItems(bool value) noexcept;
	void setSaveBackup(bool value)         noexcept;
	void setDebugFiles(bool value)         noexcept;
	void setDebugDaemon(bool value)        noexcept;

	/// Layout snap-to-grid step in pixels (same on both axes). 0 = snap disabled.
	int  getLayoutGrid() const noexcept { return getInt(LAYOUT_GRID); }
	void setLayoutGrid(int value) noexcept {
		setValue(LAYOUT_GRID, value);
		if (layoutGridChanged) layoutGridChanged();
	}

	/// Register a callback fired whenever the layout grid step changes.
	void onLayoutGridChanged(std::function<void()> cb) noexcept { layoutGridChanged = std::move(cb); }

	/// Layout test light duration, in milliseconds. Stored as seconds (1 decimal); read on demand.
	unsigned getLayoutTestTimeout() const noexcept;
	/// @param seconds duration in seconds; persisted with a single decimal.
	void     setLayoutTestTimeout(double seconds) noexcept;

	const string& getThemeName()  const noexcept { return getValue(THEME_NAME); }
	void setThemeName(const string& id)  noexcept { setValue(THEME_NAME, id);  }

	ThemeStyle getThemeStyle()           const noexcept;
	void       setThemeStyle(ThemeStyle) noexcept;

	Mode getMode()      const noexcept { return currentMode;                    }
	bool isPortable()   const noexcept { return currentMode == Mode::Portable;  }
	bool isLocal()      const noexcept { return currentMode == Mode::Local;     }
	bool isIterative()  const noexcept { return currentMode == Mode::Iterative; }

	/// Called by DialogSettings after binary detection succeeds or fails.
	void setMode(Mode mode) noexcept { currentMode = mode; }

	const string&       getConfigPath()     const noexcept { return configPath;     }
	const string&       getCurrentProject() const noexcept { return currentProject; }
	const StringVector& getColorFiles()     const noexcept { return colorFiles;     }
	bool getHasGameData() const noexcept { return hasGameData; }
	bool getHasColors()   const noexcept { return hasColors;   }
	bool getHasControls() const noexcept { return hasControls; }

	void setConfigPath(const string& path)     noexcept;
	void setCurrentProject(const string& name) noexcept; ///< Also writes DEFAULT_PROJECT.
	void setColorFiles(StringVector files)     noexcept; ///< Also calls colorFilesChanged if set.

	/// Register a callback fired whenever the color file list is replaced.
	void onColorFilesChanged(std::function<void()> cb) noexcept { colorFilesChanged = std::move(cb); }

	void setDataDirStatus(bool gameData, bool colors, bool controls) noexcept;

	/// projectsDir + currentProject + "/", or empty if either is unset.
	string getProjectDir() const noexcept;

	/// Portable: getProjectDir() + CONFIG_FILE.  Local/Iterative: configPath.
	string getActiveConfigPath() const noexcept;

protected:

	Mode currentMode = Mode::Portable;

	string
		configPath,
		currentProject;

	StringVector colorFiles;
	std::function<void()> colorFilesChanged;
	std::function<void()> layoutGridChanged;

	bool
		hasGameData = false,
		hasColors   = false,
		hasControls = false;

	Settings()  = default;
	~Settings() = default;

private:

	static const StringUMap DEFAULTS;
	static Settings instance;

	/// Derives currentMode from binaryPath (empty → Portable) and interactiveMode.
	void updateMode() noexcept;
};

} // namespace
