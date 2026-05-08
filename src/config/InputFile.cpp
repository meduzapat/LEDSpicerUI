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

using namespace LEDSpicerUI::Config;

InputFile::InputFile(const string& filePath, const string& relPath) :
	ProjectFile(filePath, "Input", nullptr)
{
	string errors;

	string baseId(Defaults::createCommonUniqueId({relPath, filename}));

	StringUMap input(rootInfo.attributes);
	input[FILENAME]  = filename;
	input[PATH_BASE] = baseId;

	tinyxml2::XMLElement* mapsNode = getRoot()->FirstChildElement("maps");
	if (not mapsNode) {
		errors += "Missing maps section for input " + filename + '\n';
	}
	else {
		StringUMapVector mapsSources;

		for (size_t idx = 0; mapsNode; mapsNode = mapsNode->NextSiblingElement("maps"), ++idx) {
			StringUMap source(processNode(mapsNode));
			string sourceBaseId(Defaults::createCommonUniqueId({baseId, std::to_string(idx)}));
			source[PATH_BASE] = sourceBaseId;
			mapsSources.push_back(std::move(source));
			errors += processMaps(
				mapsNode,
				Defaults::createCommonUniqueId({sourceBaseId, COLLECTION_INPUT_MAPS})
			);
		}

		extractedData.emplace(
			Defaults::createCommonUniqueId({baseId, COLLECTION_INPUT_SOURCES}),
			std::move(mapsSources)
		);
	}

	processLinkedTriggers(baseId);

	extractedData.emplace(
		Defaults::createCommonUniqueId({relPath, COLLECTION_INPUTS}),
		StringUMapVector{input}
	);

	if (not errors.empty())
		throw Message("Errors:\n" + errors);
}

void InputFile::processLinkedTriggers(const string& baseId) noexcept {
	if (not Defaults::hasLinkedMaps(XMLHelper::valueOf(rootInfo.attributes, NAME)))
		return;

	const string linked{XMLHelper::valueOf(rootInfo.attributes, LINKED_ITEMS)};
	if (linked.empty())
		return;

	StringUMapVector imlData;
	for (const string& chunk : Defaults::explode(linked, '|'))
		if (not chunk.empty())
			imlData.push_back({{LINKED_ITEMS, chunk}});

	if (imlData.empty())
		return;

	extractedData.emplace(
		Defaults::createCommonUniqueId({baseId, COLLECTION_INPUT_LINKMAPS}),
		std::move(imlData)
	);
}

string InputFile::processMaps(
	tinyxml2::XMLElement* mapsNode,
	const string& inputName
) noexcept {
	tinyxml2::XMLElement* mapNode{mapsNode->FirstChildElement("map")};
	if (not mapNode) return "Missing input map section\n";

	string errors;
	StringUMapVector maps;
	for (; mapNode; mapNode = mapNode->NextSiblingElement("map")) {
		StringUMap mapAttr{processNode(mapNode)};
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

void InputFile::save(const Ui::Storage::Input& input, const string& filePath) {
	Glib::file_set_contents(filePath, input.toXML());
}
