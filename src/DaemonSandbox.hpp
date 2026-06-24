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
 * Isolated, throwaway projects tree for live layout testing, handed to the
 * daemon via --projects-dir so testing never touches the user's real project.
 */
class DaemonSandbox {

public:

	/**
	 * Builds the isolated tree and writes the empty test profile.
	 * @throws Message on failure.
	 */
	DaemonSandbox();

	~DaemonSandbox();

	DaemonSandbox(const DaemonSandbox&)            = delete;
	DaemonSandbox& operator=(const DaemonSandbox&) = delete;

	/**
	 * (Re)writes the test configuration from the current live data.
	 * @param settings packed <settings> values (already carries the port).
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
	);

	/**
	 * @return the path to the configuration file in the sandbox (ledspicer.conf).
	 */
	string getConfigPath() const noexcept { return Glib::build_filename(baseDir, CONFIG_FILE); }

	/**
	 * @return the path to the isolated projects base directory (daemon -J).
	 */
	const string& getProjectsDir() const noexcept { return baseDir; }

private:

	/// <tmp>/ledspicerui-test — the --projects-dir base.
	const string baseDir;
};

} // namespace
