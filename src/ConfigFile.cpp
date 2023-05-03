/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFile.cpp
 * @since     Apr 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include "ConfigFile.hpp"

using namespace LEDSpicerUI::Ui;

ConfigFile::ConfigFile(const string& ledspicerconf) : XMLHelper(ledspicerconf, "Configuration") {
	// extract settings
	nodeSettings = processNode(getRoot());
	string errors(processDevices());
	if (not errors.empty())
		Message::displayError("Errors:\n" + errors);
}

unordered_map<string, string> ConfigFile::getSettings() {
	return nodeSettings;
}

vector<string>& ConfigFile::getDevicesList() {
	return allDevices;
}

vector<string>& ConfigFile::getElementsList() {
	return allElements;
}

vector<unordered_map<string, string>>& ConfigFile::getDevices() {
	return devices;
}

vector<unordered_map<string, string>>& ConfigFile::getDeviceElements(const string& deviceName) {
	return devicesElements.at(deviceName);
}

vector<unordered_map<string, string>>& ConfigFile::getGroups() {
	return groups;
}

vector<unordered_map<string, string>>& ConfigFile::getGroupElements(const string& groupName) {
	return groupElements.at(groupName);
}

string ConfigFile::detectElementType(const Glib::ustring& name) {
	for (int c = 1; c < elementTypes.size(); ++c)
		if (name.lowercase().find(elementTypes[c]) != name.npos)
			return std::to_string(c);
	return DEFAULT_ELEMENT_TYPE;
}

string ConfigFile::processDevices() {
	tinyxml2::XMLElement* deviceNode = root->FirstChildElement("devices");
	if (not deviceNode)
		return "Missing Devices section";

	deviceNode = deviceNode->FirstChildElement("device");
	if (not deviceNode )
		return "Empty device section";

	string errors;
	for (; deviceNode; deviceNode = deviceNode->NextSiblingElement("device")) {
		unordered_map<string, string> deviceAttr = processNode(deviceNode);
		try {
			checkAttributes({NAME, ID}, deviceAttr, "device");
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		string name = deviceAttr[NAME] + "_" + deviceAttr[ID];
		if (std::find(allDevices.begin(), allDevices.end(), name) != allDevices.end()) {
			errors += "Duplicated device " + name;
			continue;
		}
		devices.push_back(deviceAttr);
		allDevices.push_back(name);
		string elementErrors(processElements(deviceNode, name));
		errors += (not elementErrors.empty() ? '\n' + elementErrors : "");
	}

	string groupErrors(processGroups());
	errors += (not groupErrors.empty() ? '\n' + groupErrors : "");
	return errors;
}

string ConfigFile::processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName) {

	tinyxml2::XMLElement* elementNote = deviceNode->FirstChildElement("element");
	if (not elementNote)
		return "Missing elements node for " + deviceName;
	vector<unordered_map<string, string>> elements;
	string errors;
	for (; elementNote; elementNote = elementNote->NextSiblingElement("element")) {
		unordered_map<string, string> elementAttr = processNode(elementNote);
		if (not elementAttr.count(NAME)) {
			errors += "\nMissing name for element in " + deviceName;
			continue;
		}
		if (std::find(allElements.begin(), allElements.end(), elementAttr[NAME]) != allElements.end()) {
			errors += "\nDuplicated element (" + elementAttr[NAME] + ") in " + deviceName;
			continue;
		}
		if (not elementAttr.count(PIN) and not elementAttr.count(SOLENOID))
			if (not elementAttr.count(RED_PIN) or not elementAttr.count(GREEN_PIN) or not elementAttr.count(BLUE_PIN)) {
				errors += "\nMissing pin data in element (" + elementAttr[NAME] + ") in " + deviceName;
				continue;
			}

		allElements.push_back(elementAttr[NAME]);
		// detect type
		elementAttr["type"] = detectElementType(elementAttr[NAME]);
		elements.push_back(elementAttr);
	}
	devicesElements.emplace(deviceName, elements);
	return errors;
}

string ConfigFile::processGroups() {
	unordered_map<string, string> group;
	tinyxml2::XMLElement* layoutNode = root->FirstChildElement("layout");
	if (not layoutNode)
		throw Message("Missing layout section");

	string errors;
	// extract default profile.
	group = processNode(layoutNode);
	defaultProfile = group["defaultProfile"];
	group.clear();

	tinyxml2::XMLElement* groupNode = layoutNode->FirstChildElement("group");
	if (groupNode)
	for (; groupNode; groupNode = groupNode->NextSiblingElement("group")) {
		group = processNode(groupNode);
		if (not group.count(NAME)) {
			errors += "\nMissing group name";
			continue;
		}

		groups.push_back(group);

		tinyxml2::XMLElement* elementNode = groupNode->FirstChildElement("element");

		if (not elementNode) {
			errors += "\nGroup " + group[NAME] + " is empty";
			continue;
		}

		vector<unordered_map<string, string>> elements;
		for (; elementNode; elementNode = elementNode->NextSiblingElement("element")) {
			unordered_map<string, string> elementAttr = processNode(elementNode);
			if (not group.count(NAME)) {
				errors += "\nMissing element name in group " + group[NAME];
				continue;
			}
			if (std::find(allElements.begin(), allElements.end(), elementAttr[NAME]) == allElements.end()) {
				errors += ("\nInvalid element " + elementAttr[NAME] + " in group " +  group[NAME]);
				continue;
			}
			elements.push_back(elementAttr);
		}
		groupElements.emplace(group[NAME], elements);
	}
	return errors;
}


