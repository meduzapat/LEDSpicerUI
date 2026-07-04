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

	bool
		haveDevices     = root->FirstChildElement("devices")     != nullptr,
		haveRestrictors = root->FirstChildElement("restrictors") != nullptr;

	// No devices or restrictors, nothing to do.
	if (not haveDevices and not haveRestrictors)
		throw Message("Missing devices and restrictors sections in " + ledspicerconf);

	string errors;
	// Optional only if restrictors are present.
	if (haveDevices) {
		errors =  processDevices();
		if (not errors.empty()) errors += '\n';
		errors += processLayoutAndGroups();
		if (not errors.empty()) errors += '\n';
		errors += processProcessLookup();
		if (not errors.empty()) errors += '\n';
	}
	// Optional only if devices are present.
	if (haveRestrictors) {
		errors += processRestrictors();
	}

	if (not errors.empty()) {
		const string report {"Errors:\n" + errors};
		if (Message::isBatching())
			Message::collect(report);
		else
			Message::displayError(report);
	}
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
			errors += e.takeMessage() + '\n';
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
			errors += '\n' + e.takeMessage();
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

	// Optional.
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
			errors += e.takeMessage() + '\n';
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
			errors += '\n' + e.takeMessage();
			continue;
		}
		maps.push_back(std::move(mapAttr));
	}
	extractedData.emplace(Defaults::createCommonUniqueId({restrictorName, COLLECTION_RESTRICTOR_MAPS}), std::move(maps));
	return errors;
}

string ConfigFile::processLayoutAndGroups() {

	tinyxml2::XMLElement* layoutNode {root->FirstChildElement("layout")};
	if (not layoutNode)
		return "Missing layout section\n";

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

vector<LEDSpicerUI::Ui::Storage::Data*> ConfigFile::expandDeviceElements(const BoxButtonCollection& devices) noexcept {
	vector<Ui::Storage::Data*> order;
	for (const auto deviceBtn : devices) {
		for (const auto elementBtn : *static_cast<Ui::Storage::Parent*>(deviceBtn->getData())->getPrimaryChild()) {
			auto element  {static_cast<Ui::Storage::Element*>(elementBtn->getData())};
			auto children {element->copyStripChildren()};
			if (children.empty())
				order.push_back(element);
			else
				for (auto child : children)
					order.push_back(child);
		}
	}
	return order;
}

bool ConfigFile::matchesDeviceOrder(
	const BoxButtonCollection& links,
	const vector<Ui::Storage::Data*>& deviceElementOrder
) noexcept {
	return std::equal(
		links.begin(), links.end(),
		deviceElementOrder.begin(), deviceElementOrder.end(),
		[](Ui::Storage::BoxButton* linkBtn, Ui::Storage::Data* element) {
			return *linkBtn->getData() == *element;
		}
	);
}

void ConfigFile::save(const ConfigData& data) {

	// Both Collections are app wide.
	bool
		haveDevices     {CollectionHandler::getInstance(COLLECTION_ELEMENTS)->getSize()        > 0},
		haveRestrictors {CollectionHandler::getInstance(COLLECTION_RESTRICTOR_MAPS)->getSize() > 0};

	if (not haveDevices and not haveRestrictors)
		throw Message("Missing devices or restrictors sections");

	// Build main values, not all values are necessary for Device/Restrictor-only configurations, but not harm.
	string xmlData {xmlHeader(TYPE_CONFIGURATION, data.settings)};

	// Helper: collect toXML() from a parent BoxButtonCollection into one string.
	const auto collectParents {[](const BoxButtonCollection& col) {
		string r;
		for (const auto btn : col) {
			auto parent {static_cast<Ui::Storage::Parent*>(btn->getData())};
			if (not parent->getPrimaryChild()->getSize()) continue;
			r += btn->getData()->toXML();
		}
		return r;
	}};

	// Process lookup (optional, only needed if devices are present, but can be used for other purposes).
	if (data.processes.getSize()) {
		Values plAttrs;
		if (not data.runEvery.empty())
			plAttrs.setValue(PARAM_MILLISECONDS, data.runEvery);
		string pData;
		for (const auto btn : data.processes)
			pData += btn->getData()->toXML();

		xmlData += xmlSection("processLookup", pData, plAttrs);
	}

	if (haveRestrictors) {
		// Restrictors (optional if devices are present)
		xmlData += xmlSection("restrictors", collectParents(data.restrictors));
	}

	// Devices needs elements and valid layout.
	if (haveDevices) {
		if (data.defaultProfile.empty())
			throw Message("Select a default profile in the profile section");

		// Devices (optional if restrictors are present)
		xmlData += xmlSection("devices", collectParents(data.devices));

		auto deviceElementOrder {expandDeviceElements(data.devices)};

		// Same as collectParents, but omits All when it's just the default order.
		const auto collectGroups {[&deviceElementOrder](const BoxButtonCollection& col) {
			string r;
			for (const auto btn : col) {
				auto group {static_cast<Ui::Storage::Parent*>(btn->getData())};
				auto links {group->getPrimaryChild()};
				if (not links->getSize()) continue;

				if (btn->getData()->getValue(NAME) == GROUP_ALL_NAME and matchesDeviceOrder(*links, deviceElementOrder))
					continue;

				r += btn->getData()->toXML();
			}
			return r;
		}};

		/*
		Layout with groups. defaultProfile is required whenever devices exist, so the
		tag must survive even when every group (including All) is empty or the default.
		*/
		Values layoutAttrs;
		if (not data.defaultProject.empty())
			layoutAttrs.setValue("defaultProject", data.defaultProject);
		layoutAttrs.setValue("defaultProfile", data.defaultProfile);
		xmlData += xmlSection("layout", collectGroups(data.groups), layoutAttrs, true);
	}

	Defaults::reduceTab();
	xmlData += xmlFooter();

	ProjectFile::saveFile(data.configPath, xmlData);
}
