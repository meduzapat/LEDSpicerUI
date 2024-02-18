/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.cpp
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

#include "Group.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Group::Group(unordered_map<string, string>& data) : Data(data) {
	// Any change on elements needs to be reflected here.
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDestination(&elements);
}

Group::~Group() {
	if (not getValue(NAME).empty()) {
		CollectionHandler::getInstance(COLLECTION_GROUP)->remove(createUniqueId());
	}
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&elements);
}

string const Group::createPrettyName() const {
	return fieldsData.at(NAME);
}

const string Group::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(NAME)});
}

const string Group::getCssClass() const {
	return "GroupBoxButton";
}

void Group::activate() {
	DataDialogs::DialogSelect::getInstance()->setOwner(&elements, this);
}

const string Group::toXML() const {
	string r(Defaults::tab() + "<group\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + ">\n";
	Defaults::increaseTab();
	for (const auto& e : elements) {
		r += e->getData()->toXML();
	}
	Defaults::reduceTab();
	r += Defaults::tab() + "</group>\n";
	return r;
}
