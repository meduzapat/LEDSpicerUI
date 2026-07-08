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
	{"themePath",           ""},
	{"projectsDir",         ""},
	{"defaultProject",      ""},
	{"interactiveMode",     HUMAN_TRUE},
	{"themeName",           ""},
	{"themeStyle",          "Auto"},
	{"preserveEmptyDir",    HUMAN_TRUE},
	{"removeInvalidItems",  HUMAN_TRUE},
	{"saveBackup",          HUMAN_TRUE},
	{"debugFiles",          HUMAN_FALSE},
	{"layoutGrid",          "0"},
	{"layoutTestTimeout",   "1.5"},
};

void Settings::load(const Values& raw) noexcept {
	for (const auto& [key, def] : DEFAULTS)
		setValue(key, raw.getValue(key, def));
}

void Settings::setBinaryPath(const string& path) noexcept {
	setValue(PATH_BINARY, path);
}

void Settings::setDataDir(const string& dir) noexcept {
	setValue(PATH_DATA, dir);
}

string Settings::getThemePath() const noexcept {
	const string& dir = getValue(PATH_THEME);
	const string base {dir.empty() ? Glib::get_user_config_dir() + "/" PACKAGE_NAME "/themes" : dir};
	return base.back() == '/' ? base : base + "/";
}

void Settings::setThemePath(const string& dir) noexcept {
	setValue(PATH_THEME, dir);
}

void Settings::setProjectsDir(const string& dir) noexcept {
	setValue(PATH_PROJECT, dir);
}

void Settings::setDefaultProject(const string& name) noexcept {
	setValue(DEFAULT_PROJECT, name);
}

void Settings::setInteractiveMode(bool value) noexcept {
	setValue(INTERACTIVE_MODE, value);
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
void Settings::setDebugHardwareTest(bool value)  noexcept { setValue(DEBUG_HARDWARE_TEST,  value); }

unsigned Settings::getLayoutTestTimeout() const noexcept {
	return static_cast<unsigned>(std::lround(getDouble(LAYOUT_TEST_TIMEOUT) * 1000));
}

void Settings::setLayoutTestTimeout(double seconds) noexcept {
	stringstream ss;
	ss << std::fixed << std::setprecision(1) << seconds;
	setValue(LAYOUT_TEST_TIMEOUT, ss.str());
}

void Settings::setConfigPath(const string& path) noexcept {
	configPath = path;
}

void Settings::setCurrentProject(const string& name) noexcept {
	currentProject = name;
	setValue(DEFAULT_PROJECT, name);
	resolveConfigSource();
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

string Settings::getProjectDir(const string& name) const noexcept {
	const string& proj = getValue(PATH_PROJECT);
	if (proj.empty() or name.empty())
		return {};
	return proj + name + "/";
}

string Settings::getActiveConfigPath() const noexcept {
	if (configSource == ConfigSource::Project)
		return getProjectDir() + CONFIG_FILE;
	return configPath;
}

void Settings::resolveConfigSource() noexcept {

	if (currentProject.empty()) {
		configSource = ConfigSource::System;
		return;
	}
	// An embedded config makes the project portable.
	if (Glib::file_test(getProjectDir() + CONFIG_FILE, Glib::FILE_TEST_EXISTS)) {
		configSource = ConfigSource::Project;
		return;
	}
	// No embedded config: use the system config when it exists or can be created there.
	configSource = isSystemConfigAvailable() ? ConfigSource::System : ConfigSource::Project;
}

bool Settings::isSystemConfigAvailable() const noexcept {
	return Glib::file_test(configPath, Glib::FILE_TEST_EXISTS) or isPathWritable(configPath);
}

bool Settings::isRootConfigWritable() const noexcept {
	if (configSource == ConfigSource::Project)
		return isProjectDirWritable();
	return isPathWritable(configPath);
}

bool Settings::isProjectDirWritable() const noexcept {
	return isPathWritable(getProjectDir());
}

bool Settings::isPathWritable(const string& path) noexcept {
	if (path.empty())
		return false;
	string target {path};
	// Missing target: probe the closest existing ancestor (can the target be created?).
	while (not Glib::file_test(target, Glib::FILE_TEST_EXISTS))
		target = Glib::path_get_dirname(target);
	return access(target.c_str(), W_OK) == 0;
}

