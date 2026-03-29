/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Sep 25, 2023
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

#include "Profile.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Profile::Profile(StringUMap& data) :
	Parent(data, COLLECTION_PROFILES, {
		{COLLECTION_ELEMENT,    BoxButtonCollection{}},
		{COLLECTION_GROUP,      BoxButtonCollection{}},
		{COLLECTION_ANIMATIONS, BoxButtonCollection{}},
		{COLLECTION_INPUT,      BoxButtonCollection{}}
	})
{
	registerDependency(COLLECTION_ELEMENT);
	registerDependency(COLLECTION_GROUP);
	registerDependency(COLLECTION_ANIMATIONS);
	registerDependency(COLLECTION_INPUT);
}

void Profile::setUp() {
	DataDialogs::DialogSelect::getInstance()->setDestinations(itemCollections, this);
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

const string Profile::getPrimaryKey() const {
	return FILENAME;
}
