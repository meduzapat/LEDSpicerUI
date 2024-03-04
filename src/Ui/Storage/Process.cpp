/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Process.cpp
 * @since     May 2, 2023
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

#include "Process.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Process::~Process() {
	if (not getValue(PARAM_PROCESS_NAME).empty()) {
		CollectionHandler::getInstance(COLLECTION_PROCESS)->remove(createUniqueId());
	}
}

const string Process::createPrettyName() const {
	return string("Process: " + fieldsData.at(PARAM_PROCESS_NAME) + " System: " + fieldsData.at(PARAM_SYSTEM));
}

const string Process::createUniqueId() const {
	return getValue(PARAM_PROCESS_NAME);
}

const string Process::getCssClass() const {
	return "ProcessBoxButton";
}

const string Process::toXML() const {
	string r(Defaults::tab() + "<map\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += Defaults::tab() + "/>\n";
	return r;
}
