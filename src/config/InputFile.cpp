/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFile.cpp
 * @since     Nov 14, 2023
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

#include "InputFile.hpp"

using namespace LEDSpicerUI;

InputFile::InputFile(const string& inputFile) : ProjectFile(inputFile, "Input", INPUT_PATH) {

	string errors;

	StringUMap input(rootInfo.attributes);

	// Inject FILENAME (basename of pathFilename) so FileData can store it as a property.
	auto pos = pathFilename.find_last_of('/');
	input[FILENAME] = pos == string::npos ? pathFilename : pathFilename.substr(pos + 1);

	tinyxml2::XMLElement* mapsNode = getRoot()->FirstChildElement("maps");
	if (not mapsNode) {
		errors += "Missing maps section for input " + pathFilename + '\n';
	}
	else {
		StringUMapVector mapsSources;

		for (size_t idx = 0; mapsNode; mapsNode = mapsNode->NextSiblingElement("maps"), ++idx) {
			mapsSources.push_back(processNode(mapsNode));
			errors += processMaps(
				mapsNode,
				Defaults::createCommonUniqueId({pathFilename, std::to_string(idx), COLLECTION_INPUT_MAPS})
			);
		}

		extractedData.emplace(Defaults::createCommonUniqueId({pathFilename, COLLECTION_INPUT_SOURCES}), std::move(mapsSources));
	}

	extractedData.emplace(COLLECTION_INPUT, StringUMapVector{input});

	if (not errors.empty())
		Message::displayError("Errors:\n" + errors);
}

const string InputFile::processMaps(tinyxml2::XMLElement* mapsNode, const string& inputName) {

	tinyxml2::XMLElement* mapNode = mapsNode->FirstChildElement("map");
	if (not mapNode) return "Missing input map section\n";

	string errors;
	StringUMapVector maps;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {
		StringUMap mapAttr = processNode(mapNode);
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
