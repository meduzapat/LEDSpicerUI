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
	errors += processProcessLookup();
	if (not errors.empty())
		Message::displayError("Errors:\n" + errors);
}

unordered_map<string, string> ConfigFile::getSettings() {
	return nodeSettings;
}

vector<unordered_map<string, string>>& ConfigFile::getDevices() {
	return devices;
}

vector<unordered_map<string, string>>& ConfigFile::getRestrictors() {
	return restrictors;
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

vector<unordered_map<string, string>>& ConfigFile::getProcess() {
	return process;
}

string ConfigFile::getProcessLookupRunEvery() {
	return processLookupRunEvery;
}

string ConfigFile::processDevices() {

	tinyxml2::XMLElement* deviceNode = root->FirstChildElement("devices");
	if (not deviceNode)
		return "Missing Devices section\n";

	deviceNode = deviceNode->FirstChildElement("device");
	if (not deviceNode )
		return "Empty device section\n";

	string errors;
	for (; deviceNode; deviceNode = deviceNode->NextSiblingElement("device")) {
		unordered_map<string, string> deviceAttr = processNode(deviceNode);
		try {
			checkAttributes({NAME, ID}, deviceAttr, "device");
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}

		if (not Defaults::devicesInfo.count(deviceAttr[NAME])) {
			errors += "Ignored device, unknown type " + deviceAttr[NAME] + '\n';
			continue;
		}

		string name = deviceAttr[NAME] + "_" + deviceAttr[ID];

		devices.push_back(deviceAttr);
		string elementErrors(processElements(deviceNode, name));
		errors += (not elementErrors.empty() ? elementErrors  + '\n' : "");
	}

	string groupErrors(processGroups());
	string restrictorErrors(processRestrictors());
	errors += (not groupErrors.empty() ? groupErrors + '\n' : "") + (not restrictorErrors.empty() ? restrictorErrors + '\n' : "") ;
	return errors;
}

string ConfigFile::processRestrictors() {
	tinyxml2::XMLElement* restrictorNode = root->FirstChildElement("restrictors");
	// Restrictors are optional.
	if (not restrictorNode)
		return "";

	restrictorNode = restrictorNode->FirstChildElement("restrictor");
	if (not restrictorNode )
		return "";

	string errors;
	for (; restrictorNode; restrictorNode = restrictorNode->NextSiblingElement("restrictor")) {
		unordered_map<string, string> restrictorAttr = processNode(restrictorNode);
		try {
			checkAttributes({NAME, ID}, restrictorAttr, "device");
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		string name = restrictorAttr[NAME] + "_" + restrictorAttr[ID];

		if (not Defaults::restrictorsInfo.count(restrictorAttr[NAME])) {
			errors += "Ignored restrictor, unknown type " + restrictorAttr[NAME] + '\n';
			continue;
		}

		// check for single vs multiple restrinctors.
		if (not restrictorAttr.count(PLAYER)) {
			tinyxml2::XMLElement* restrictorMap = restrictorNode->FirstChildElement("map");
			if (not restrictorMap) {
				errors += "Ignored restrictor, invalid settings for " + name + '\n';
				continue;
			}
			for (; restrictorMap; restrictorMap = restrictorMap->NextSiblingElement("map")) {
				unordered_map<string, string> restrictorAttrTemp(restrictorAttr);
				for (auto& pair : processNode(restrictorMap))
					restrictorAttrTemp.insert(pair);
				if (not restrictorAttrTemp.count(PLAYER) or not restrictorAttrTemp.count(JOYSTICK) ) {
					errors += "Ignored restrictor, Missing or invalid mappings for " + name + '\n';
					continue;
				}
				restrictors.push_back(restrictorAttrTemp);
				continue;
			}
			continue;
		}
		restrictors.push_back(restrictorAttr);
	}
	return errors;
}

string ConfigFile::processProcessLookup() {
	tinyxml2::XMLElement* plNode = root->FirstChildElement("processLookup");
	if (not plNode)
		return "";

	unordered_map<string, string> plAttr = processNode(plNode);

	processLookupRunEvery = plAttr.count(PARAM_MILLISECONDS) ? plAttr.at(PARAM_MILLISECONDS) : "";

	plNode = plNode->FirstChildElement("map");
	if (not plNode )
		return "";

	string errors;
	for (; plNode; plNode = plNode->NextSiblingElement("map")) {
		plAttr = processNode(plNode);
		try {
			checkAttributes({PARAM_PROCESS_NAME, PARAM_SYSTEM}, plAttr, "processlookup");
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		process.push_back(plAttr);
	}
	return errors;
}

string ConfigFile::processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName) {

	tinyxml2::XMLElement* elementNote = deviceNode->FirstChildElement("element");
	if (not elementNote)
		return "Missing elements node for " + deviceName + '\n';
	vector<unordered_map<string, string>> elements;
	string errors;
	for (; elementNote; elementNote = elementNote->NextSiblingElement("element")) {
		unordered_map<string, string> elementAttr = processNode(elementNote);
		if (not elementAttr.count(NAME)) {
			errors += "Ignored element, Missing element name in " + deviceName + '\n';
			continue;
		}
		if (not elementAttr.count(PIN) and not elementAttr.count(SOLENOID))
			if (not elementAttr.count(RED_PIN) or not elementAttr.count(GREEN_PIN) or not elementAttr.count(BLUE_PIN)) {
				errors += "Ignored element, Missing pin data in element (" + elementAttr[NAME] + ") in " + deviceName + '\n';
				continue;
			}

		// Detect type
		elementAttr["type"] = Defaults::detectElementType(elementAttr[NAME]);
		elements.push_back(elementAttr);
	}
	devicesElements.emplace(deviceName, elements);
	return errors;
}

string ConfigFile::processGroups() {
	unordered_map<string, string> group;
	tinyxml2::XMLElement* layoutNode = root->FirstChildElement("layout");
	if (not layoutNode)
		throw Message("Missing layout section, no groups\n");

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
			errors += "Missing group name\n";
			continue;
		}

		groups.push_back(group);

		tinyxml2::XMLElement* elementNode = groupNode->FirstChildElement("element");

		if (not elementNode) {
			errors += "Group " + group[NAME] + " is empty\n";
			continue;
		}

		vector<unordered_map<string, string>> elements;
		for (; elementNode; elementNode = elementNode->NextSiblingElement("element")) {
			unordered_map<string, string> elementAttr = processNode(elementNode);
			if (not group.count(NAME)) {
				errors += "Missing element name in group " + group[NAME] + '\n';
				continue;
			}
			elements.push_back(elementAttr);
		}
		groupElements.emplace(group[NAME], elements);
	}
	return errors;
}


