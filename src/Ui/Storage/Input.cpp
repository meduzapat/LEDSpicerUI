/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.cpp
 * @since     Sep 27, 2023
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

#include "Input.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Input::Input(unordered_map<string, string>& data) :
	Data(data),
	// maps can have multiple targets.
	maps(TARGET, {true, false}),
	// linked maps are not absolute and can have multiple entries.
	linkedMaps(ID, {false, false})
{
	// link maps to elements and groups using the name.
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDestination(&maps);
	CollectionHandler::getInstance(COLLECTION_GROUP)->registerDestination(&maps);
	// to avoid duplicated triggers.
	CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)->registerDestination(&maps);
	// link linked maps to maps using the trigger.
	CollectionHandler::getInstance(COLLECTION_INPUT_LINKED_MAPS)->registerDestination(&linkedMaps);
	// this value is not used as data, only for filename.
	ignored.push_back(FILENAME);
}

Input::~Input() {
	if (not getValue(FILENAME).empty()) {
		CollectionHandler::getInstance(COLLECTION_INPUT)->remove(createUniqueId());
	}
	// unlink.
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&maps);
	CollectionHandler::getInstance(COLLECTION_GROUP)->release(&maps);
	CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)->release(&maps);
	CollectionHandler::getInstance(COLLECTION_INPUT_LINKED_MAPS)->release(&linkedMaps);
}

string const Input::createPrettyName() const {
	return fieldsData.at(NAME) + " " + fieldsData.at(FILENAME);
}

const string Input::createTooltip() const {
	return "Plugin " + getValue(NAME) + " with " +
			std::to_string(maps.getSize()) + " maps" +
			(getValue(NAME) == "Actions" ? " and " + std::to_string(linkedMaps.getSize()) + " linked maps" : "");
}

const string Input::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(FILENAME)});
}

const string Input::getCssClass() const {
	return "InputBoxButton";
}

void Input::activate() {
	DataDialogs::DialogInputMap::getInstance()->setOwner(&maps, this);
	DataDialogs::DialogInputLinkMaps::getInstance()->setOwner(&linkedMaps, this);
}

const string Input::toXML() const {
	string r("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
	<LEDSpicer\
		version=\"1.0\"\
		type=\"Input\"\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	for (const auto& e : maps) {
		r += e->getData()->toXML();
	}
	Defaults::reduceTab();
	r += "</LEDSpicer>\n";
	return r;
}
