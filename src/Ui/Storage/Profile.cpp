/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Sep 25, 2023
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

#include "Profile.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Profile::Profile(unordered_map<string, string>& data) : Data(data) {
	// Any change needs to be reflected here.
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDestination(&alwaysOnElements);
	CollectionHandler::getInstance(COLLECTION_GROUP)->registerDestination(&alwaysOnGroups);
	CollectionHandler::getInstance(COLLECTION_INPUT)->registerDestination(&inputs);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->registerDestination(&animationss);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->registerDestination(&startTransitions);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->registerDestination(&endTransitions);
}

Profile::~Profile() {
	if (not getValue(FILENAME).empty()) {
		CollectionHandler::getInstance(COLLECTION_PROFILES)->remove(createUniqueId());
	}
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&alwaysOnElements);
	CollectionHandler::getInstance(COLLECTION_GROUP)->release(&alwaysOnGroups);
	CollectionHandler::getInstance(COLLECTION_INPUT)->release(&inputs);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->release(&animationss);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->release(&startTransitions);
//	CollectionHandler::getInstance(COLLECTION_ANIMATION)->release(&endTransitions);
}

const string Profile::createPrettyName() const {
	return fieldsData.at(FILENAME);
}

const string Profile::createUniqueId() const {
	return getValue(FILENAME);
}

const string Profile::getCssClass() const {
	return "ProfileBoxButton";
}


const string Profile::toXML() const {
	string r("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
	<LEDSpicer\
		version=\"1.0\"\
		type=\"Profile\"\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	for (const auto& e : alwaysOnElements) {
		r += e->getData()->toXML();
	}
	Defaults::reduceTab();
	r += "</LEDSpicer>\n";
	return r;
}

void Profile::lateActivate(Selectors selector) {
	switch (selector) {
	case Selectors::AlwaysOnElements:
		DataDialogs::DialogSelect::getInstance()->setOwner(&alwaysOnElements, this);
	break;
	case Selectors::AlwaysOnGroups:
		DataDialogs::DialogSelect::getInstance()->setOwner(&alwaysOnGroups, this);
	break;
	case Selectors::Inputs:
		DataDialogs::DialogSelect::getInstance()->setOwner(&inputs, this);
	break;
	case Selectors::Animationss:
		DataDialogs::DialogSelect::getInstance()->setOwner(&animationss, this);
	break;
	case Selectors::StartTransitions:
		DataDialogs::DialogSelect::getInstance()->setOwner(&startTransitions, this);
	break;
	case Selectors::EndTransitions:
		DataDialogs::DialogSelect::getInstance()->setOwner(&endTransitions, this);
	break;
	}
}

