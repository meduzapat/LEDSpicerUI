/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DaemonSandbox.cpp
 * @since     Jun 24, 2026
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

#include "DaemonSandbox.hpp"

#include <filesystem>

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Ui;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Config::ConfigFile;
using LEDSpicerUI::Config::ProjectFile;
using LEDSpicerUI::Ui::Storage::BoxButtonCollection;

DaemonSandbox::~DaemonSandbox() {
	try {
		resetDir();
	}
	catch (...) {}
}

void DaemonSandbox::resetDir() const {
	std::error_code ec;
	std::filesystem::remove_all(sandboxDir, ec);
	if (ec)
		throw Message("Could not remove the test sandbox: " + ec.message());
}

void DaemonSandbox::createEmptyProfile(const string& profileDir) const {
	const string profilePath {Glib::build_filename(profileDir, TEST_SANDBOX_DEFAULT_PROFILE + string(".xml"))};
	Values background;
	background.setValue(BACKGROUND_COLOR, DEFAULT_PROFILE_BACKGROUND_COLOR);
	string profileXml {XMLHelper::xmlHeader(TYPE_PROFILE, background)};
	Defaults::reduceTab();
	profileXml += XMLHelper::xmlFooter();
	ProjectFile::saveFile(profilePath, profileXml);
}

void DaemonSandbox::regenerate(
	Values settings,
	const BoxButtonCollection& devices,
	const BoxButtonCollection& restrictors,
	const BoxButtonCollection& groups
) const {

	const string profileDir {Glib::build_filename(Glib::build_filename(sandboxDir, TEST_SANDBOX_PROJECT), PATH_PROFILE)};

	// Start from a clean tree.
	std::error_code ec;
	resetDir();

	// Create test ground.
	std::filesystem::create_directories(sandboxDir, ec);
	if (ec)
		throw Message("Could not create the test sandbox: " + ec.message());

	// Devices Test only.
	if (devices.getSize() > 0) {
		std::filesystem::create_directories(profileDir, ec);
		if (ec)
			throw Message("Could not create the test profile: " + ec.message());
		createEmptyProfile(profileDir);
	}

	settings.setValue("logLevel", "Debug");

	// Neither need processes.
	const BoxButtonCollection noProcesses;

	// save config will only save whatis necessary.
	ConfigFile::save(ConfigFile::ConfigData(
		getConfigPath(),
		TEST_SANDBOX_PROJECT,
		TEST_SANDBOX_DEFAULT_PROFILE,
		DEFAULT_RUNEVERY,
		settings,
		devices,
		restrictors,
		groups,
		noProcesses
	));
}
