/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFile.cpp
 * @since     Nov 14, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

#include "InputFile.hpp"

using namespace LEDSpicerUI;

InputFile::InputFile(const string& inputFile, const string& projectRoot) : XMLHelper(inputFile, "Input") {

	string
		name(Defaults::extractName(inputFile, projectRoot)),
		errors;
	// Extract input settings.
	unordered_map<string, string> input(processNode(getRoot()));
	// Add filename (name of the plugin)
	input.emplace(FILENAME, name);
	string pluginType(input[NAME]);
	extractedData.emplace(COLLECTION_INPUT, std::move(vector<unordered_map<string, string>>{input}));
	vector<unordered_map<string, string>> inputMaps;
	// This plugin types can have multiple sources.
	if (pluginType == "Credits" or pluginType == "Actions" or pluginType == "Impulse" or pluginType == "Blinker") {

		// Extract listenEvents.
		unordered_set<string> listenEvents;
		errors += processInputSources(pluginType, getRoot(), listenEvents);

		// Extract maps.
		tinyxml2::XMLElement* mapsNode = getRoot()->FirstChildElement("maps");
		if (not mapsNode) {
			errors += "Missing maps section for input plugin " + name + '\n';
		}
		else {
			// Check for listenEvents, at this point everything is sanitized.
			vector<unordered_map<string, string>> maps;
			for (; mapsNode; mapsNode = mapsNode->NextSiblingElement("maps")) {
				unordered_map<string, string> mapsNodeAttr(processNode(mapsNode));
				if (not mapsNodeAttr.count("source")) {
					errors += "Missing source attribute in maps for input plugin " + name + '\n';
					continue;
				}
				const string mapName(mapsNodeAttr["source"]);
				if (not listenEvents.count(mapName)) {
					errors += "Maps does not match any listenEvent for input plugin " + name + '\n';
					continue;
				}
				maps.emplace_back(std::move(mapsNodeAttr));
				errors += processMaps(mapsNode, Defaults::createCommonUniqueId({name, mapName, COLLECTION_INPUT_MAPS}));
			}
			extractedData.emplace(Defaults::createCommonUniqueId({name, COLLECTION_INPUT_EVENTS}), std::move(maps));
		}
	}
	else {
		// Single source or malformed.
		errors += processMaps(getRoot(), Defaults::createCommonUniqueId({name, COLLECTION_INPUT_MAPS}));
	}

	if (not errors.empty())
		throw Message("Errors:\n" + errors);
}

const string InputFile::processMaps(tinyxml2::XMLElement* mapsNode, const string& inputName) {

	tinyxml2::XMLElement* mapNode = mapsNode->FirstChildElement("map");
	if (not mapNode) return "Missing input map section\n";

	string errors;
	vector<unordered_map<string, string>> maps;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {
		unordered_map<string, string> mapAttr = processNode(mapNode);
		try {
			checkAttributes({TYPE, TARGET, TRIGGER, COLOR, FILTER}, mapAttr, "input map for " + inputName);
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		maps.push_back(std::move(mapAttr));
	}
	extractedData.emplace(inputName, maps);
	return errors;
}

const string InputFile::processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode, unordered_set<string>& listenEvents) {
	// Check for listenEvents.
	tinyxml2::XMLElement* listenEventsNode(inputNode->FirstChildElement("listenEvents"));
	if (not listenEventsNode) return "Missing listenEvents for input plugin " + inputName + '\n';
	string errors;
	listenEventsNode = listenEventsNode->FirstChildElement("listenEvent");
	for (; listenEventsNode; listenEventsNode = listenEventsNode->NextSiblingElement("listenEvent")) {
		unordered_map<string, string> listenEventsAttr(processNode(listenEventsNode));
		if (not listenEventsAttr.count(NAME)) {
			errors += "Missing name attribute in listenEvents for input plugin " + inputName + '\n';
			continue;
		}
		listenEvents.insert(std::move(listenEventsAttr[NAME]));
	}
	if (listenEvents.empty()) return "Empty listenEvents section for input plugin " + inputName + '\n';
	return errors;
}
