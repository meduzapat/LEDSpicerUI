/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFile.cpp
 * @since     Nov 14, 2023
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

#include "InputFile.hpp"

using namespace LEDSpicerUI;

InputFile::InputFile(const string& inputFile) : XMLHelper(inputFile, "Input") {
	// process filename.
	auto file(Defaults::explode(Glib::path_get_basename(inputFile), '.'));
	// This will work fine if the file have extension :P
	file.pop_back();
	string
		// Extract name.
		name(Defaults::implode(file, '.')),
		// Extract maps.
		errors(processMaps(name));
	// Extract input data.
	unordered_map<string, string> input(processNode(getRoot()));
	// Add filename (name)
	input.emplace(FILENAME, name);
	extractedData.emplace(COLLECTION_INPUT, std::move(vector<unordered_map<string, string>>{input}));

	// Extract linked maps, if any.
	if (input.count(LINKED_ITEMS) and not input.at(LINKED_ITEMS).empty()) {
		errors += processLinkedMaps(input.at(LINKED_ITEMS), name);
	}
	if (not errors.empty())
		Message::displayError("Errors:\n" + errors);
}

const string InputFile::processMaps(const string& inputName) {

	tinyxml2::XMLElement* mapNode = root->FirstChildElement("map");
	if (not mapNode)
		return "Missing input map section\n";

	string errors;
	vector<unordered_map<string, string>> maps;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {
		unordered_map<string, string> mapAttr = processNode(mapNode);
		try {
			checkAttributes({TYPE, TARGET, TRIGGER, COLOR, FILTER}, mapAttr, "input map");
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			continue;
		}
		maps.push_back(mapAttr);
	}
	extractedData.emplace(Defaults::createCommonUniqueId({inputName, COLLECTION_INPUT_MAPS}), maps);
	return errors;
}

const string InputFile::processLinkedMaps(const string& inputLinkedMaps, const string& inputName) {
	if (!extractedData.count(inputName + COLLECTION_INPUT_MAPS)) {
		return "";
	}
	// Chunk sections.
	auto chunks(Defaults::explode(inputLinkedMaps, ID_GROUP_SEPARATOR));
	string errors;
	vector<unordered_map<string, string>> linkedMaps;
	for (auto& chunk : chunks) {
		if (chunk.empty()) {
			errors += "Empty linked map for " + inputName;
			continue;
		}
		vector<string> linkedMapTrigger;
		// Find the details using the trigger in the extracted input maps.
		for (auto& trigger : Defaults::explode(chunk, ID_SEPARATOR)) {
			Defaults::trim(trigger);
			if (trigger.empty()) {
				errors += "Empty item name in linked map for " + inputName;
				continue;
			}
			// Loop the input maps array until the trigger is sought.
			for (const auto& im : extractedData.at(inputName + COLLECTION_INPUT_MAPS)) {
				if (im.count(TYPE) and im.count(TARGET) and im.count(TRIGGER) and im.at(TRIGGER) == trigger) {
					// this should be trigger30type30name
					linkedMapTrigger.emplace_back(Defaults::createCommonUniqueId({
						trigger,
						im.at(TYPE) + " " + im.at(TARGET)
					}));
					break;
				}
			}
		}
		if (linkedMapTrigger.size()) {
			// this should be trigger30type30name31trigger30type30name31trigger30type30name31etc
			linkedMaps.emplace_back(unordered_map<string, string>{{NAME, Defaults::implode(linkedMapTrigger, RECORD_SEPARATOR)}});
		}
	}
	extractedData.emplace(Defaults::createCommonUniqueId({inputName, COLLECTION_INPUT_LINKED_MAPS}), std::move(linkedMaps));
	return errors;
}
