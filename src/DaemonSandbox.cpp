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

DaemonSandbox::DaemonSandbox() :
	baseDir(Glib::build_filename(Glib::get_tmp_dir(), TEST_SANDBOX_DIR))
{
	namespace fs = std::filesystem;

	const string profileDir {
		Glib::build_filename(Glib::build_filename(baseDir, TEST_SANDBOX_PROJECT), PATH_PROFILE)
	};
	const string profilePath {
		Glib::build_filename(profileDir, TEST_SANDBOX_DEFAULT_PROFILE + string(".xml"))
	};

	// Start from a clean tree (any leftover from a prior, ungraceful exit).
	std::error_code ec;
	fs::remove_all(baseDir, ec);
	fs::create_directories(profileDir, ec);
	if (ec)
		throw Message("Could not create the test sandbox: " + ec.message());

	// Empty profile so the daemon boots a blank board.
	Values background;
	background.setValue(BACKGROUND_COLOR, DEFAULT_PROFILE_BACKGROUND_COLOR);
	string profileXml {XMLHelper::xmlHeader(TYPE_PROFILE, background)};
	Defaults::reduceTab();
	profileXml += XMLHelper::xmlFooter();
	ProjectFile::saveFile(profilePath, profileXml);
}

DaemonSandbox::~DaemonSandbox() {
	std::error_code ec;
	std::filesystem::remove_all(baseDir, ec);
}

void DaemonSandbox::regenerate(
	Values settings,
	const BoxButtonCollection& devices,
	const BoxButtonCollection& restrictors,
	const BoxButtonCollection& groups
) {
	// Throwaway config: keep the hardware-defining data (devices, groups), force
	// debug logging and the empty test profile, and drop the process lookup.
	settings.setValue("logLevel", "Debug");
	const string configPath {getConfigPath()};
	const BoxButtonCollection noProcesses;
	ConfigFile::save(ConfigFile::ConfigData(
		configPath,
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
