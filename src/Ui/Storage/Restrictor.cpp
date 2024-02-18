/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	Restrictor.cpp
 * @since	Apr 30, 2023
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

#include "Restrictor.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Restrictor::~Restrictor() {
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_RESTRICTORS)->remove(createUniqueId());
	}
}

const string Restrictor::createPrettyName() const {
	string
		name (fieldsData.at(NAME)),
		r(Defaults::restrictorsInfo.at(name).name);
	if (Defaults::isIdUser(name, false))
		r += " Id: " + fieldsData.at(ID);
	if (Defaults::isSerial(name, false))
		r += " Port: " + (fieldsData.at(PORT).empty() ? "<autodetect>" : fieldsData.at(PORT));
	return r;
}

const string Restrictor::createUniqueId() const {
	return Defaults::createHardwareUniqueId(*getValues(), false);
}

const string Restrictor::getCssClass() const {
	return "RestrictorBoxButton";
}

void Restrictor::destroy() {
	playerMapping.wipe();
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_RESTRICTORS)->remove(createUniqueId());
	}
}

void Restrictor::activate() {
	DataDialogs::DialogRestrictorMap::getInstance()->setOwner(&playerMapping, this);
}

const string Restrictor::toXML() const {
	string r(Defaults::tab() + "<restrictor\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	for (const auto& e : playerMapping) {
		r += e->getData()->toXML();
	}
	Defaults::reduceTab();
	return r + Defaults::tab() + "</restrictor>\n";
}

