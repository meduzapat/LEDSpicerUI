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

using namespace LEDSpicerUI;

SettingsFile::SettingsFile(const string& settingsFile) : XMLHelper(settingsFile, XML_FILE_PLAIN) {
	rootInfo.attributes = processNode(root);
}

string SettingsFile::getConfigFilePath() {
	return Glib::get_user_config_dir() + "/" PACKAGE_NAME "/" UI_CONFIG_FILE;
}

bool SettingsFile::configExists() {
	return Glib::file_test(getConfigFilePath(), Glib::FILE_TEST_EXISTS);
}

void SettingsFile::ensureConfigDir() {
	string configDir = Glib::get_user_config_dir() + "/" PACKAGE_NAME;
	if (not Glib::file_test(configDir, Glib::FILE_TEST_IS_DIR)) {
		Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(configDir);
		file->make_directory_with_parents();
	}
}

void SettingsFile::save(const string& settingsFile, const StringUMap& values) {
	ensureConfigDir();

	// Settings file is self-closing, no version/type (UI config, not data file)
	string xmlData;
	xmlData  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	xmlData += "<!-- " DEFAULT_MESSAGE " -->\n";
	xmlData += "<" PACKAGE_NAME "\n";

	Defaults::increaseTab();
	for (const auto& [key, value] : values) {
		if (not value.empty()) {
			xmlData += Defaults::tab() + key + "=\"" + value + "\"\n";
		}
	}
	Defaults::reduceTab();

	xmlData += "/>\n";

	try {
		Glib::file_set_contents(settingsFile, xmlData);
	}
	catch (const Glib::FileError& e) {
		throw Message("Failed to save settings: " + string(e.what()));
	}
}
