/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFile.cpp
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

#include "ConfigFile.hpp"

using namespace LEDSpicerUI::Config;

ConfigFile::ConfigFile(const string& ledspicerconf) : XMLHelper(ledspicerconf, "Configuration") {
	// rootInfo.attributes already populated by XMLHelper constructor
	string errors(processDevices());
	errors += processProcessLookup();
	if (not errors.empty()) Message::displayError("Errors:\n" + errors);
}

string ConfigFile::processDevices() {

	tinyxml2::XMLElement* deviceNode = root->FirstChildElement("devices");
	if (not deviceNode)
		return "Missing Devices section\n";

	deviceNode = deviceNode->FirstChildElement(TYPE_DEVICE.c_str());
	if (not deviceNode )
		return "Empty device section\n";

	string errors, name;
	ValueVector devices;
	for (; deviceNode; deviceNode = deviceNode->NextSiblingElement(TYPE_DEVICE.c_str())) {
		Values deviceAttr {processNode(deviceNode)};
		try {
			checkAttributes({NAME}, deviceAttr, TYPE_DEVICE);
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		name = deviceAttr.getValue(NAME);
		if (Defaults::devicesInfo.find(name) == Defaults::devicesInfo.end()) {
			errors += "Ignored device, unknown type " + name + '\n';
			continue;
		}
		// Create Unique ID.
		const string id {Defaults::createHardwareUniqueId(deviceAttr)};
		devices.push_back(std::move(deviceAttr));
		string elementErrors(processElements(deviceNode, id));
		errors += (not elementErrors.empty() ? elementErrors  + '\n' : "");
	}
	extractedData.emplace(COLLECTION_DEVICES, std::move(devices));

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

	string errors, name;
	ValueVector restrictors;
	for (; restrictorNode; restrictorNode = restrictorNode->NextSiblingElement("restrictor")) {
		Values restrictorAttr {processNode(restrictorNode)};
		try {
			checkAttributes({NAME}, restrictorAttr, TYPE_DEVICE);
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		name = restrictorAttr.getValue(NAME);
		if (Defaults::restrictorsInfo.find(name) == Defaults::restrictorsInfo.end()) {
			errors += "Ignored restrictor, unknown type " + name + '\n';
			continue;
		}
		// Create Unique ID.
		const string id {Defaults::createHardwareUniqueId(restrictorAttr, false)};

		restrictors.push_back(std::move(restrictorAttr));
		string mapErrors(processRestrictorMaps(restrictorNode, id));
		errors += (not mapErrors.empty() ? mapErrors  + '\n' : "");
	}
	extractedData.emplace(COLLECTION_RESTRICTORS, std::move(restrictors));
	return errors;
}

string ConfigFile::processProcessLookup() {
	tinyxml2::XMLElement* plNode = root->FirstChildElement("processLookup");
	if (not plNode)
		return "";

	Values plAttr {processNode(plNode)};
	rootInfo.setValue(PARAM_MILLISECONDS, plAttr.getValue(PARAM_MILLISECONDS));

	plNode = plNode->FirstChildElement("map");
	if (not plNode )
		return "";

	string errors;
	ValueVector process;
	for (; plNode; plNode = plNode->NextSiblingElement("map")) {
		plAttr = processNode(plNode);
		try {
			checkAttributes({PARAM_PROCESS_NAME, PARAM_SYSTEM}, plAttr, "processlookup");
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		process.push_back(std::move(plAttr));
	}
	extractedData.emplace(COLLECTION_PROCESSES, std::move(process));
	return errors;
}

string ConfigFile::processElements(tinyxml2::XMLElement* deviceNode, const string& deviceName) {

	tinyxml2::XMLElement* elementNode {deviceNode->FirstChildElement(TYPE_ELEMENT.c_str())};
	if (not elementNode)
		return "Missing elements node for " + deviceName + '\n';
	ValueVector elements;
	string errors;
	for (; elementNode; elementNode = elementNode->NextSiblingElement(TYPE_ELEMENT.c_str())) {
		Values elementAttr {processNode(elementNode)};
		if (not elementAttr.isSet(NAME)) {
			errors += "Ignored element, Missing element name in " + deviceName + '\n';
			continue;
		}
		// Detect type
		elementAttr.setValue("type", Defaults::detectElementType(elementAttr.getValue(NAME)));
		elements.push_back(std::move(elementAttr));
	}
	extractedData.emplace(Defaults::createCommonUniqueId({deviceName, COLLECTION_ELEMENTS}), std::move(elements));
	return errors;
}

string ConfigFile::processRestrictorMaps(tinyxml2::XMLElement* restrictorNode, const string& restrictorName) {
	tinyxml2::XMLElement* mapNode {restrictorNode->FirstChildElement("map")};
	if (not mapNode)
		return "Missing player map node for " + restrictorName + '\n';
	ValueVector maps;
	string errors;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {

		Values mapAttr {processNode(mapNode)};
		try {
			checkAttributes({PLAYER, JOYSTICK, RESTRICTOR_INTERFACE}, mapAttr, "restrictor map");
		}
		catch (Message& e) {
			errors += '\n' + e.getMessage();
			continue;
		}
		maps.push_back(std::move(mapAttr));
	}
	extractedData.emplace(Defaults::createCommonUniqueId({restrictorName, COLLECTION_RESTRICTOR_MAPS}), std::move(maps));
	return errors;
}

string ConfigFile::processGroups() {
	tinyxml2::XMLElement* layoutNode {root->FirstChildElement("layout")};
	if (not layoutNode)
		throw Message("Missing layout section, no groups\n");

	string errors;
	// extract default profile.
	Values group {processNode(layoutNode)};
	rootInfo.setValue("defaultProfile", group.getValue("defaultProfile"));

	tinyxml2::XMLElement* groupNode {layoutNode->FirstChildElement("group")};
	ValueVector groups;
	if (groupNode)
	for (; groupNode; groupNode = groupNode->NextSiblingElement("group")) {
		group = processNode(groupNode);
		if (not group.isSet(NAME)) {
			errors += "Missing group name\n";
			continue;
		}

		const string groupName {group.getValue(NAME)};
		groups.push_back(std::move(group));

		tinyxml2::XMLElement* elementNode = groupNode->FirstChildElement(TYPE_ELEMENT.c_str());

		if (not elementNode) {
			errors += "Group " + groupName + " is empty\n";
			continue;
		}

		ValueVector elements;
		for (; elementNode; elementNode = elementNode->NextSiblingElement(TYPE_ELEMENT.c_str())) {
			Values elementAttr {processNode(elementNode)};
			if (not elementAttr.isSet(NAME)) {
				errors += "Missing element name in group " + groupName + '\n';
				continue;
			}

			elements.push_back(std::move(elementAttr));
		}
		extractedData.emplace(Defaults::createCommonUniqueId({groupName, COLLECTION_GROUP_LINKS}), std::move(elements));
	}
	extractedData.emplace(COLLECTION_GROUPS, std::move(groups));
	return errors;
}

void ConfigFile::save(const ConfigData& data) {

	// Validation
	if (data.defaultProfile.empty())
		throw Message("Select a default profile in the profile section");

	if (data.devices.getSize() == 0)
		throw Message("At least one device is required");

	// Helper: collect toXML() from a BoxButtonCollection into one string.
	const auto collect {[](const BoxButtonCollection& col) {
		string r;
		for (const auto btn : col)
			r += btn->getData()->toXML();
		return r;
	}};

	// Build XML
	string xmlData(xmlHeader("Configuration"));
	xmlData += toXML(data.settings);
	Defaults::reduceTab();
	xmlData += ">\n";
	Defaults::increaseTab();

	// Process lookup (optional)
	if (data.processes.getSize()) {
		Values plAttrs;
		if (not data.runEvery.empty())
			plAttrs.setValue(PARAM_MILLISECONDS, data.runEvery);

		xmlData += xmlSection("processLookup", collect(data.processes), plAttrs);
		Defaults::reduceTab();
	}

	// Devices (required)
	xmlData += xmlSection("devices", collect(data.devices));

	// Restrictors (optional)
	xmlData += xmlSection("restrictors", collect(data.restrictors));

	// Layout with groups
	xmlData += xmlSection(
		"layout",
		collect(data.groups),
		{{string("defaultProfile"), data.defaultProfile}}
	);

	Defaults::reduceTab();
	xmlData += xmlFooter();

	// DEBUG: display instead of writing — remove when real saving is wired up.
	debugSave(data.configPath, xmlData);
//	Glib::file_set_contents(filePath, xmlData);

}
