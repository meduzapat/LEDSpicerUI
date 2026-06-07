/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Settings.cpp
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

#include "Settings.hpp"

using namespace LEDSpicerUI;
using namespace Config;

Settings Settings::instance;

const StringUMap Settings::DEFAULTS = {
	{"binaryPath",          ""},
	{"dataDir",             ""},
	{"projectsDir",         ""},
	{"defaultProject",      ""},
	{"interactiveMode",     HUMAN_TRUE},
	{"themeName",           DEFAULT},
	{"themeStyle",          "Auto"},
	{"preserveEmptyDir",    HUMAN_TRUE},
	{"removeInvalidItems",  HUMAN_TRUE},
	{"saveBackup",          HUMAN_TRUE},
	{"debugFiles",          HUMAN_FALSE},
};

void Settings::load(const Values& raw) noexcept {
	for (const auto& [key, def] : DEFAULTS)
		setValue(key, raw.getValue(key, def));
	updateMode();
}

void Settings::setBinaryPath(const string& path) noexcept {
	setValue(PATH_BINARY, path);
	updateMode();
}

void Settings::setDataDir(const string& dir) noexcept {
	setValue(PATH_DATA, dir);
}

void Settings::setProjectsDir(const string& dir) noexcept {
	setValue(PATH_PROJECT, dir);
}

void Settings::setDefaultProject(const string& name) noexcept {
	setValue(DEFAULT_PROJECT, name);
}

void Settings::setInteractiveMode(bool value) noexcept {
	setValue(INTERACTIVE_MODE, value);
	updateMode();
}

Settings::ThemeStyle Settings::getThemeStyle() const noexcept {
	const string& ts = getValue(THEME_STYLE);
	if (ts == "Light") return ThemeStyle::Light;
	if (ts == "Dark")  return ThemeStyle::Dark;
	return ThemeStyle::Auto;
}

void Settings::setThemeStyle(ThemeStyle style) noexcept {
	switch (style) {
	case ThemeStyle::Light: setValue(THEME_STYLE, "Light"); break;
	case ThemeStyle::Dark:  setValue(THEME_STYLE, "Dark");  break;
	default:                setValue(THEME_STYLE, "Auto");  break;
	}
}

void Settings::setPreserveEmptyDir(bool value)   noexcept { setValue(PRESERVE_EMPTY_DIR,   value); }
void Settings::setRemoveInvalidItems(bool value) noexcept { setValue(REMOVE_INVALID_ITEMS, value); }
void Settings::setSaveBackup(bool value)         noexcept { setValue(SAVE_BACKUP,          value); }
void Settings::setDebugFiles(bool value)         noexcept { setValue(DEBUG_FILES,          value); }

void Settings::setConfigPath(const string& path) noexcept {
	configPath = path;
}

void Settings::setCurrentProject(const string& name) noexcept {
	currentProject = name;
	setValue(DEFAULT_PROJECT, name);
}

void Settings::setColorFiles(StringVector files) noexcept {
	colorFiles = std::move(files);
	if (not colorFiles.empty() and colorFilesChanged) colorFilesChanged();
}

void Settings::setDataDirStatus(bool gameData, bool colors, bool controls) noexcept {
	hasGameData = gameData;
	hasColors   = colors;
	hasControls = controls;
}

string Settings::getProjectDir() const noexcept {
	const string& proj = getValue(PATH_PROJECT);
	if (proj.empty() || currentProject.empty())
		return {};
	return proj + currentProject + "/";
}

string Settings::getActiveConfigPath() const noexcept {
	if (isPortable())
		return getProjectDir() + CONFIG_FILE;
	return configPath;
}

void Settings::updateMode() noexcept {
	if (getValue(PATH_BINARY).empty()) {
		currentMode = Mode::Portable;
		return;
	}
	currentMode = (getValue(INTERACTIVE_MODE) == HUMAN_TRUE) ? Mode::Iterative : Mode::Local;
}
