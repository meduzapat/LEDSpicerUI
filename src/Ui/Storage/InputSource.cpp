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

InputSource::~InputSource() {
	CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)->release(&maps);
	if (not fieldsData.empty()) {
		CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES)->remove(this);
	}
}

const string InputSource::createUniqueId() const {
	return Defaults::createCommonUniqueId({
		getProperty(INPUT_PK),
		getProperty(INDEX)
	});
}

const string InputSource::createPrettyName() const {
	string source(getValue(SOURCE));
	return source.empty() ? "<single>" : source;
}

const string InputSource::createTooltip() const {
	return "Source " + createPrettyName() + " with " + std::to_string(maps.getSize()) + " maps";
}

const string InputSource::getCssClass() const {
	return "InputSourceBoxButton";
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
