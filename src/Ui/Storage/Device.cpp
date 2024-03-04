/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	  Device.cpp
 * @since	 Feb 26, 2023
 * @author	Patricio A. Rossi (MeduZa)
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

#include "Device.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Device::~Device() {
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_DEVICES)->remove(createUniqueId());
	}
}

const string Device::createPrettyName() const {
	string
		name (fieldsData.at(NAME)),
		r(Defaults::devicesInfo.at(name).name);
	if (Defaults::isIdUser(name))
		r += " Id: " + fieldsData.at(ID);
	if (Defaults::isSerial(name))
		r += " Port: " + (fieldsData.at(PORT).empty() ? "<autodetect>" : fieldsData.at(PORT));
	return r;
}

const string Device::getCssClass() const {
	return "DeviceBoxButton";
}

const string Device::createUniqueId() const {
	return Defaults::createHardwareUniqueId(*getValues());
}

void Device::destroy() {
	elements.wipe();
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_DEVICES)->remove(createUniqueId());
	}
}

void Device::activate() {
	DataDialogs::DialogElement::getInstance()->setOwner(&elements, this);
}

const string Device::toXML() const {
	string r(Defaults::tab() + "<device\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + ">\n";
	Defaults::increaseTab();
	for (const auto& e : elements) {
		r += e->getData()->toXML();
	}
	Defaults::reduceTab();
	return r + Defaults::tab() + "</device>\n";
}


