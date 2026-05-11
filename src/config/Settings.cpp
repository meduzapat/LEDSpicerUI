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

using namespace LEDSpicerUI::Config;

Settings Settings::instance;

void Settings::setBinaryPath(const string& path) noexcept {
	values[PATH_BINARY] = path;
	updateMode();
}

void Settings::setDataDir(const string& dir) noexcept {
	values[PATH_BINARY] = dir;
}

void Settings::setProjectsDir(const string& dir) noexcept {
	values[PATH_PROJECT] = dir;
}

void Settings::setInteractiveMode(bool value) noexcept {
	values[INTERACTIVE_MODE] = value ? HUMAN_TRUE : HUMAN_FALSE;
	updateMode();
}

Settings::ThemeStyle Settings::getThemeStyle() const noexcept {
	const string ts {values.at(THEME_STYLE)};
	if (ts == "Light")
		return ThemeStyle::Light;
	else if (ts == "Dark")
		return ThemeStyle::Dark;
	else
		return ThemeStyle::Auto;
}

void Settings::setThemeStyle(ThemeStyle style) noexcept {
	switch (style) {
	case ThemeStyle::Light:
		values[THEME_STYLE] = "Light";
	break;
	case ThemeStyle::Dark:
		values[THEME_STYLE] = "Dark";
		break;
	default:
		values[THEME_STYLE] = "Auto";
		break;
	}
}

void Settings::setCleanProjectDir(bool value) noexcept {
	values[CLEAN_PROJECT_DIR] = value ? HUMAN_TRUE : HUMAN_FALSE;
}

void Settings::setPreserveEmptyDir(bool value) noexcept {
	values[PRESERVE_EMPTY_DIR] = value ? HUMAN_TRUE : HUMAN_FALSE;
}

void Settings::setRemoveInvalidItems(bool value) noexcept {
	values[REMOVE_INVALID_ITEMS] = value ? HUMAN_TRUE : HUMAN_FALSE;
}

void Settings::setSaveBackup(bool value) noexcept {
	values[SAVE_BACKUP] = value ? HUMAN_TRUE : HUMAN_FALSE;
}

void Settings::setDebugFiles(bool value) noexcept {
	values[DEBUG_FILES] = value ? HUMAN_TRUE : HUMAN_FALSE;
}

void Settings::load(const StringUMap& valuesmap) noexcept {



	binaryPath  = read("binaryPath");
	dataDir     = read("dataDir");
	projectsDir = read("projectsDir");

	interactiveMode    = read("interactiveMode",    "true")  != "false";
	cleanProjectDir    = read("cleanProjectDir",    "false") == "true";
	preserveEmptyDir   = read("preserveEmptyDir",   "false") == "true";
	removeInvalidItems = read("removeInvalidItems", "false") == "true";
	saveBackup         = read("saveBackup",         "true")  != "false";
	debugFiles         = read("debugFiles",         "false") == "true";

	const string& ts = read("themeStyle", "auto");
	if (ts == "light")
		themeStyle = ThemeStyle::Light;
	else if (ts == "dark")
		themeStyle = ThemeStyle::Dark;
	else
		themeStyle = ThemeStyle::Auto;

	updateMode();
}

StringUMap Settings::toMap() const noexcept {
	return {
		{"binaryPath",        binaryPath},
		{"dataDir",           dataDir},
		{"projectsDir",       projectsDir},
		{"interactiveMode",   interactiveMode    ? "true"  : "false"},
		{"cleanProjectDir",   cleanProjectDir    ? "true"  : "false"},
		{"preserveEmptyDir",  preserveEmptyDir   ? "true"  : "false"},
		{"removeInvalidItems",removeInvalidItems ? "true"  : "false"},
		{"saveBackup",        saveBackup         ? "true"  : "false"},
		{"debugFiles",        debugFiles         ? "true"  : "false"},
		{"themeStyle",        themeStyle == ThemeStyle::Light ? "light" :
		                      themeStyle == ThemeStyle::Dark  ? "dark"  : "auto"},
	};
}

void Settings::updateMode() noexcept {
	if (values[PATH_BINARY].empty())
		currentMode = Mode::Portable;
	else if (values.at(INTERACTIVE_MODE) == HUMAN_TRUE)
		currentMode = Mode::Iterative;
	else
		currentMode = Mode::Local;
}
