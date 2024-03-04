/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.cpp
 * @since     Mar 16, 2023
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

#include "Element.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Element::~Element() {
	if (not getValue(NAME).empty()) {
		CollectionHandler::getInstance(COLLECTION_ELEMENT)->remove(createUniqueId());
	}
}

const string Element::createPrettyName() const {
	return fieldsData.at(NAME);
}

const string Element::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(NAME)});
}

const string Element::getCssClass() const {
	return "ElementBoxButton";
}

const string Element::toXML() const {
	string r(Defaults::tab() + "<element\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + "/>\n";
	return r;
}

string Element::getPinCssByData(uint8_t pin, Data* data) {
	string pinS(std::to_string(pin));
	if (not data->getValue(PIN).empty()       and data->getValue(PIN)       == pinS)
		return COLOR_PIN;
	if (not data->getValue(SOLENOID).empty()  and data->getValue(SOLENOID)  == pinS)
		return COLOR_SOLENOID;
	if (not data->getValue(RED_PIN).empty()   and data->getValue(RED_PIN)   == pinS)
		return COLOR_RED;
	if (not data->getValue(GREEN_PIN).empty() and data->getValue(GREEN_PIN) == pinS)
		return COLOR_GREEN;
	if (not data->getValue(BLUE_PIN).empty()  and data->getValue(BLUE_PIN)  == pinS)
		return COLOR_BLUE;
	return NO_COLOR;
}
