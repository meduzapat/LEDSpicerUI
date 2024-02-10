/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFile.hpp
 * @since     Apr 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "XMLHelper.hpp"

#ifndef CONFIGFILE_HPP_
#define CONFIGFILE_HPP_ 1

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::Ui::ConfigFile
 */
class ConfigFile : public XMLHelper {

public:

	ConfigFile() = delete;

	ConfigFile(const string& ledspicerconf);

	virtual ~ConfigFile() = default;

	unordered_map<string, string> getSettings();

	const string getDefaultProfile() const;

	const string getProcessLookupRunEvery() const;

protected:

	string defaultProfile;

	string processLookupRunEvery;

	unordered_map<string, string> nodeSettings;

	const string processDevices();

	const string processRestrictors();

	const string processProcessLookup();

	const string processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName);

	const string processRestrictorMaps(tinyxml2::XMLElement* restrictorNode, const string& restrictorName);

	const string processGroups();

};

} /* namespace LEDSpicerUI */

#endif /* CONFIGFILE_HPP_ */
