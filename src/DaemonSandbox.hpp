/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DaemonSandbox.hpp
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

#include "config/ConfigFile.hpp"
#include "config/ProjectFile.hpp"

#pragma once

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::DaemonSandbox
 *
 * Utility class to create a temporary sandbox for the daemon to run in,
 * with its own isolated, throwaway projects tree for live layout testing.
 */
class DaemonSandbox {

public:

	/**
	 * Builds the isolated tree and writes the empty test profile.
	 * @throws Message on failure.
	 */
	DaemonSandbox() : sandboxDir(Glib::build_filename(Glib::get_tmp_dir(), TEST_SANDBOX_DIR)) {}

	~DaemonSandbox();

	DaemonSandbox(const DaemonSandbox&)            = delete;
	DaemonSandbox& operator=(const DaemonSandbox&) = delete;

	/**
	 * Removes the sandbox directory and all its contents.
	 * @throws Message on failure.
	 */
	void resetDir() const;

	/**
	 * Creates an empty test profile in the sandbox for Device testing.
	 * @param profilePath The path to use.
	 * @throws Message on failure.
	 */
	void createEmptyProfile(const string& profileDir) const;

	/**
	 * (Re)writes the test configuration from the current live data.
	 * Based on the devices and or restrictors it will create the necessary
	 * config file for the daemon to run in the sandbox.
	 * @param settings packed root values.
	 * @param devices live device collection.
	 * @param restrictors live restrictor collection.
	 * @param groups live group collection.
	 * @throws Message on failure.
	 */
	void regenerate(
		Values settings,
		const Ui::Storage::BoxButtonCollection& devices,
		const Ui::Storage::BoxButtonCollection& restrictors,
		const Ui::Storage::BoxButtonCollection& groups
	) const;

	/**
	 * @return the path to the temp ledspicer.conf file in the sandbox.
	 */
	string getConfigPath() const noexcept { return Glib::build_filename(sandboxDir, CONFIG_FILE); }

	/**
	 * @return the path to the isolated projects base directory.
	 */
	const string& getProjectsDir() const noexcept { return sandboxDir; }

protected:

	/// The base directory for the sandbox, which is created in a temporary location.
	const string sandboxDir;

};

} // namespace
