/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.cpp
 * @since     Sep 27, 2023
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

#include "Input.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Input::~Input() {
	if (CollectionHandler::getInstance(COLLECTION_INPUT)->isSet(this))
		CollectionHandler::getInstance(COLLECTION_INPUT)->remove(this);
}

const string Input::createTooltip() const {
	return "Input " + FileData::createTooltip();
}

const string Input::getCssClass() const {
	return "InputBoxButton";
}

void Input::activate() {
	DataDialogs::DialogInputSource::getInstance()->setOwner(&sources, this);
	// TODO wire linkedMaps once DialogInputLinkMaps is reworked.
	// DataDialogs::DialogInputLinkMaps::getInstance()->setOwner(&linkedMaps, this);
}

const string Input::toXML() const {
	string r(XMLHelper::xmlHeader("Input"));
	r += Data::toXML();
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	for (const auto& s : sources)
		r += s->getData()->toXML();
	Defaults::reduceTab();
	r += XMLHelper::xmlFooter();
	return r;
}

void Input::reset() {
	sources.wipe();
	linkedMaps.wipe();
	Data::reset();
}
