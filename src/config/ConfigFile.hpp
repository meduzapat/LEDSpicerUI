/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFile.hpp
 * @since     Apr 15, 2023
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

#include "ProjectFile.hpp"
#include "Storage/Element.hpp"

#pragma once

namespace LEDSpicerUI::Config {

using Ui::Storage::BoxButtonCollection;
using Ui::Storage::CollectionHandler;

/**
 * LEDSpicerUI::ConfigFile
 * Handles ledspicer.conf reading.
 */
class ConfigFile : public XMLHelper {

public:

	struct ConfigData {
		const string
			& configPath,
			& defaultProject,
			& defaultProfile,
			& runEvery;
		const Values& settings;
		const BoxButtonCollection
			& devices,
			& restrictors,
			& groups,
			& processes;

		ConfigData(
			const string& configPath,
			const string& defaultProject,
			const string& defaultProfile,
			const string& runEvery,
			const Values& settings,
			const BoxButtonCollection& devices,
			const BoxButtonCollection& restrictors,
			const BoxButtonCollection& groups,
			const BoxButtonCollection& processes
		) :
			configPath(configPath),
			defaultProject(defaultProject),
			defaultProfile(defaultProfile),
			runEvery(runEvery),
			settings(settings),
			devices(devices),
			restrictors(restrictors),
			groups(groups),
			processes(processes)
		{}
	};

	ConfigFile() = delete;

	ConfigFile(const string& ledspicerconf);

	virtual ~ConfigFile() = default;

	static void save(const ConfigData& data);

	/**
	 * Walks devices in order, expanding each strip into its children (never the
	 * parent, which carries PROP_NO_SELECT and is never linkable itself) — the
	 * order the daemon uses when it auto-builds All from the file.
	 * @param devices Devices collection, in save/file order.
	 * @return Flat, ordered list of every linkable element.
	 */
	static vector<Ui::Storage::Data*> expandDeviceElements(
		const BoxButtonCollection& devices
	) noexcept;

	/**
	 * @param links A group's links collection, in its current order.
	 * @param deviceElementOrder Result of expandDeviceElements().
	 * @return True if links is identical to deviceElementOrder, position for position.
	 */
	static bool matchesDeviceOrder(
		const BoxButtonCollection& links,
		const vector<Ui::Storage::Data*>& deviceElementOrder
	) noexcept;

protected:

	string processDevices();

	string processRestrictors();

	string processProcessLookup();

	string processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName);

	string processRestrictorMaps(tinyxml2::XMLElement* restrictorNode, const string& restrictorName);

	string processLayoutAndGroups();
};

} // namespace
