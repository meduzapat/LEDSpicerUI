/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMap.cpp
 * @since     Sep 30, 2023
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

#include "InputMap.hpp"

using namespace LEDSpicerUI::Ui::Storage;

InputMap::~InputMap() {
	if (not getValue(TRIGGER).empty()) {
		CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)->remove(createUniqueId());
		// Redraw the box, because the collection maybe deleted elements from linked maps.
		auto i(DataDialogs::DialogInputLinkMaps::getInstance());
		// When shutting down this will be null.
		if (i)
			i->refreshBox();
	}
}

const string InputMap::createPrettyName() const {
	return "[" + fieldsData.at(TRIGGER) + "] " + fieldsData.at(TYPE) + " " + fieldsData.at(TARGET);
}

const string InputMap::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(TRIGGER)});
}

const string InputMap::getCssClass() const {
	return "InputMapBoxButton";
}

const string InputMap::toXML() const {
	string r(Defaults::tab() + "<map\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + ">\n";
	return r;
}
