/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.cpp
 * @since     Mar 16, 2023
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
	if (fieldsData.at(BRIGHTNESS) == "100")
		ignored.insert(BRIGHTNESS);
	return createOpeningXML("element", fieldsData, ignored, true);
}

void Element::splitRGB(Data* data) {
	if (not data->getValue(POSITIONS).empty()) {
		// Always assume good data.
		const string position(Defaults::explode(data->getValue(POSITIONS), ',')[0]);
		convertPositionToRGB(data, position, data->getValue(COLORFORMAT));
		return;
	}
	// LED strip.
	if (not data->getValue(STRIPSIZE).empty()) {
		convertPositionToRGB(data, data->getValue(POSITION), data->getValue(COLORFORMAT));
		return;
	}
	// RGB.
	if (not data->getValue(POSITION).empty()) {
		convertPositionToRGB(data, data->getValue(POSITION), data->getValue(COLORFORMAT));
		return;
	}
}

void Element::convertPositionToRGB(Data* data, const string& position, const string& colorFormat) {
	auto pin(findFirstConnectorIndexByPosition(position));
	for (const auto& c : colorFormat) {
		switch (c) {
		case 'R':
			data->setValue(RED_PIN, std::to_string(pin++));
			break;
		case 'G':
			data->setValue(GREEN_PIN, std::to_string(pin++));

			break;
		case 'B':
			data->setValue(GREEN_PIN, std::to_string(pin++));
			break;
		}
	}
}

const uint16_t Element::findFirstConnectorIndexByPosition(const string& position) {
	return (((std::stod(position) -1) * 3) + 1) - 1;
}

