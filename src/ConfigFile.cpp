/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFile.cpp
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

#include "ConfigFile.hpp"

using namespace LEDSpicerUI;

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

const string ConfigFile::getDefaultProfile() const {
	return defaultProfile;
}

const string ConfigFile::getProcessLookupRunEvery() const {
	return processLookupRunEvery;
}

const string ConfigFile::processDevices() {

	tinyxml2::XMLElement* deviceNode = root->FirstChildElement("devices");
	if (not deviceNode)
		return "Missing Devices section\n";

	deviceNode = deviceNode->FirstChildElement("device");
	if (not deviceNode )
		return "Empty device section\n";

	string errors, name;
	vector<unordered_map<string, string>> devices;
	for (; deviceNode; deviceNode = deviceNode->NextSiblingElement("device")) {
		unordered_map<string, string> deviceAttr = processNode(deviceNode);
		try {
			checkAttributes({NAME}, deviceAttr, "device");
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		name = deviceAttr[NAME];
		if (not Defaults::devicesInfo.count(name)) {
			errors += "Ignored device, unknown type " + name + '\n';
			continue;
		}
		// Create Unique ID.
		unordered_map<string, string> data{
			{NAME, name},
			// Device ID is no mandatory, and if is missing on a ID device, is assumed 1
			{ID,   deviceAttr.count(ID)   ? deviceAttr[ID] : "1"},
			// Serial Devices allows empty device serial PORT, that defaults to /dev/ttyUSB0or auto-detects.
			{PORT, deviceAttr.count(PORT) ? deviceAttr[PORT] : ""}
		};

		devices.push_back(deviceAttr);
		string elementErrors(processElements(deviceNode, Defaults::createHardwareUniqueId(data)));
		errors += (not elementErrors.empty() ? elementErrors  + '\n' : "");
	}
	extractedData.emplace(COLLECTION_DEVICES, std::move(devices));

	string groupErrors(processGroups());
	string restrictorErrors(processRestrictors());
	errors += (not groupErrors.empty() ? groupErrors + '\n' : "") + (not restrictorErrors.empty() ? restrictorErrors + '\n' : "") ;
	return errors;
}

const string ConfigFile::processRestrictors() {
	tinyxml2::XMLElement* restrictorNode = root->FirstChildElement("restrictors");
	// Restrictors are optional.
	if (not restrictorNode)
		return "";

	restrictorNode = restrictorNode->FirstChildElement("restrictor");
	if (not restrictorNode )
		return "";

	string errors, name;
	vector<unordered_map<string, string>> restrictors;
	for (; restrictorNode; restrictorNode = restrictorNode->NextSiblingElement("restrictor")) {
		unordered_map<string, string> restrictorAttr = processNode(restrictorNode);
		try {
			checkAttributes({NAME}, restrictorAttr, "device");
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		name = restrictorAttr[NAME];
		if (not Defaults::restrictorsInfo.count(name)) {
			errors += "Ignored restrictor, unknown type " + name + '\n';
			continue;
		}
		// Create Unique ID.
		unordered_map<string, string> data{
			{NAME, name},
			// Restrictor ID is no mandatory, and if is missing on a ID restrictor, is assumed 1
			{ID,   not restrictorAttr.count(ID)   ? "1" : restrictorAttr[ID]},
			// Serial restrictors allows empty port, that defaults to /dev/ttyUSB0 or auto-detects.
			{PORT, not restrictorAttr.count(PORT) ? ""  : restrictorAttr[PORT]}
		};

		restrictors.push_back(restrictorAttr);
		string mapErrors(processRestrictorMaps(restrictorNode, Defaults::createHardwareUniqueId(data, false)));
		errors += (not mapErrors.empty() ? mapErrors  + '\n' : "");
	}
	extractedData.emplace(COLLECTION_RESTRICTORS, std::move(restrictors));
	return errors;
}

const string ConfigFile::processProcessLookup() {
	tinyxml2::XMLElement* plNode = root->FirstChildElement("processLookup");
	if (not plNode)
		return "";

	unordered_map<string, string> plAttr = processNode(plNode);
	processLookupRunEvery = plAttr.count(PARAM_MILLISECONDS) ? plAttr.at(PARAM_MILLISECONDS) : "";

	plNode = plNode->FirstChildElement("map");
	if (not plNode )
		return "";

	string errors;
	vector<unordered_map<string, string>> process;
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
	extractedData.emplace(COLLECTION_PROCESS, std::move(process));
	return errors;
}

const string ConfigFile::processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName) {

	tinyxml2::XMLElement* elementNode = deviceNode->FirstChildElement("element");
	if (not elementNode)
		return "Missing elements node for " + deviceName + '\n';
	vector<unordered_map<string, string>> elements;
	string errors;
	for (; elementNode; elementNode = elementNode->NextSiblingElement("element")) {
		unordered_map<string, string> elementAttr = processNode(elementNode);
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
	extractedData.emplace(Defaults::createCommonUniqueId({deviceName, COLLECTION_ELEMENT}), std::move(elements));
	return errors;
}

const string ConfigFile::processRestrictorMaps(tinyxml2::XMLElement* restrictorNode, const string& restrictorName) {
	tinyxml2::XMLElement* mapNode = restrictorNode->FirstChildElement("map");
	if (not mapNode)
		return "Missing player map node for " + restrictorName + '\n';
	vector<unordered_map<string, string>> maps;
	string errors;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {

		unordered_map<string, string> mapAttr = processNode(mapNode);
		try {
			checkAttributes({PLAYER, JOYSTICK, RESTRICTOR_INTERFACE}, mapAttr, "restrictor map");
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		maps.push_back(mapAttr);
	}
	extractedData.emplace(Defaults::createCommonUniqueId({restrictorName, COLLECTION_RESTRICTOR_MAP}), std::move(maps));
	return errors;
}

const string ConfigFile::processGroups() {
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
	vector<unordered_map<string, string>> groups;
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
		extractedData.emplace(Defaults::createCommonUniqueId({group[NAME], COLLECTION_GROUP}), std::move(elements));
	}
	extractedData.emplace(COLLECTION_GROUP, std::move(groups));
	return errors;
}
