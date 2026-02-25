/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMap.cpp
 * @since     Sep 30, 2023
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

#include "InputMap.hpp"

using namespace LEDSpicerUI::Ui::Storage;

InputMap::~InputMap() {
	auto collectionName(COLLECTION_INPUT_MAPS + getProperty(SOURCE));
	if (CollectionHandler::getInstance(collectionName)->isSet(this))
		CollectionHandler::getInstance(collectionName)->remove(this);
}

const string InputMap::createUniqueId() const {
	string trigger(getValue(TRIGGER));
	if (trigger.empty()) return "";
	// Source ID stored as a property by DialogInputMap::createData().
	return getProperty(SOURCE) + "_" + trigger + "_" + getValue(NAME);
}

const string InputMap::createPrettyName() const {
	return "[" + getValue(TRIGGER) + "] " + getValue(TYPE) + " " + getValue(NAME);
}

const string InputMap::getCssClass() const {
	return "InputMapBoxButton";
}
