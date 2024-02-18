/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RestrictorMap.cpp
 * @since     Oct 14, 2023
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

#include "RestrictorMap.hpp"

using namespace LEDSpicerUI::Ui::Storage;

RestrictorMap::~RestrictorMap() {
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_RESTRICTOR_MAP)->remove(createUniqueId());
	}
}

string const RestrictorMap::createPrettyName() const {
	string name("Player " + fieldsData.at(PLAYER) + " Joystick " + fieldsData.at(JOYSTICK));
	// Split hardware name out of aditional info and check for multi.
	if (not getValue(RESTRICTOR_INTERFACE).empty())
		name += " [ interface " + fieldsData.at(RESTRICTOR_INTERFACE) + "]";
	return name;
}

string const RestrictorMap::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(PLAYER), getValue(JOYSTICK)});
}

string const RestrictorMap::getCssClass() const {
	return "RestrictorMapBoxButton";
}

const string RestrictorMap::toXML() const {
	string r(Defaults::tab() + "<map\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + ">\n";
	return r;
}
