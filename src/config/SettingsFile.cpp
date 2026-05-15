/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SettingsFile.cpp
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

#include "SettingsFile.hpp"

using namespace LEDSpicerUI::Config;

string SettingsFile::settingFilePath;

SettingsFile::SettingsFile() : XMLHelper(getSettingsPath(), UI_CONFIG_TYPE) {
	Settings::get().load(getRootInfo());
}

bool SettingsFile::initialize() {
	if (settingFilePath.empty())
		settingFilePath = Glib::get_user_config_dir() + "/" PACKAGE_NAME "/" UI_CONFIG_FILE;
	if (not configExists()) {
		Settings::get().load(Values{});
		return false;
	}
	SettingsFile sf;
	return true;
}

void SettingsFile::save() {
	ensureConfigDir();
	string xml {xmlHeader(UI_CONFIG_TYPE, Settings::get().asValues())};
	Defaults::reduceTab();
	xml += xmlFooter();
	try {
		Glib::file_set_contents(getSettingsPath(), xml);
	}
	catch (const Glib::FileError& e) {
		throw Message("Failed to save settings: " + string(e.what()));
	}
}

void SettingsFile::setSettingsPath(const string& path) noexcept {
	settingFilePath = path;
}

const string& SettingsFile::getSettingsPath() noexcept {
	return settingFilePath;
}

bool SettingsFile::configExists() noexcept {
	return Glib::file_test(getSettingsPath(), Glib::FILE_TEST_EXISTS);
}

void SettingsFile::ensureConfigDir() noexcept {
	const string dir {Glib::path_get_dirname(getSettingsPath())};
	if (not Glib::file_test(dir, Glib::FILE_TEST_IS_DIR)) {
		Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(dir);
		file->make_directory_with_parents();
	}
}
