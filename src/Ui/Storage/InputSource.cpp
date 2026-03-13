/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSource.cpp
 * @since     Feb 20, 2026
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

#include "InputSource.hpp"

using namespace LEDSpicerUI::Ui::Storage;

InputSource::InputSource(StringUMap& data) :
	Data(data),
	sId("src_" + std::to_string(++sourceCounter))
	{
	setProperty(UID, sId);
	// register maps against the global collections so cascade deletes propagate.
	CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({&maps});
	CollectionHandler::getInstance(COLLECTION_GROUP)->registerDependency({&maps});
}

InputSource::~InputSource() {

	CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&maps);
	CollectionHandler::getInstance(COLLECTION_GROUP)->release(&maps);

	const string collectionId(COLLECTION_INPUT_SOURCES + getProperty(PID));
	if (CollectionHandler::getInstance(collectionId)->isSet(this))
		CollectionHandler::getInstance(collectionId)->remove(this);
}

const string InputSource::createUniqueId() const {
	return Defaults::createCommonUniqueId({getProperty(PID), getValue(SOURCE)});
}

const string InputSource::createPrettyName() const {
	const string label(getProperty(NAME));
	if (not label.empty()) return label;
	string source{getValue(SOURCE)};
	return source.empty() ? "<single>" : source;
}

const string InputSource::createTooltip() const {
	return "Source " + createPrettyName() + " with " + std::to_string(maps.getSize()) + " maps";
}

const string InputSource::getCssClass() const {
	return "InputSourceBoxButton";
}

void InputSource::reset() {
	maps.wipe();
	if (not fieldsData.empty())
		CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES + getProperty(PID))->remove(this);
	Data::reset();
}

void InputSource::activate() {
	DataDialogs::DialogInputMap::getInstance()->setOwner(&maps, this);
}

const string InputSource::toXML() const {
	string source(getValue(SOURCE));
	string r(Defaults::tab());

	if (source.empty()) {
		r += "<maps>\n";
	}
	else {
		r += "<maps source=\"" + source + "\">\n";
	}

	Defaults::increaseTab();
	for (const auto& m : maps) {
		r += m->getData()->toXML();
	}
	Defaults::reduceTab();

	r += Defaults::tab() + "</maps>\n";
	return r;
}
