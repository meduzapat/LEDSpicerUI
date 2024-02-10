/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMapLink.cpp
 * @since     Feb 6, 2024
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2024 Patricio A. Rossi (MeduZa)
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

#include "InputMapLink.hpp"

using namespace LEDSpicerUI::Ui::Storage;

const string InputMapLink::createTooltip() const {
	string tooltip("This action will start with the ");
	vector<string> txts;
	// data is trigger(30)type target(31)trigger(30)type target(31)trigger(30)type target
	for (const auto& group : Defaults::explode(fieldsData.at(NAME), RECORD_SEPARATOR)) {
		// group is trigger(30)type target
		const auto parts(Defaults::explode(group, FIELD_SEPARATOR));
		const auto item(Defaults::explode(parts.at(1), ' '));
		txts.push_back(item[0] + " " + item[1] + ", when the trigger \"" + parts.at(0) + "\"");
	}
	return tooltip + Defaults::implode(txts, " is detected, will move to ") + " is detected, will start over";
}

const string InputMapLink::createPrettyName() const {
	vector<string> prettyName;
	// data is trigger(30)type target(31)trigger(30)type target(31)trigger(30)type target
	for (const auto& group : Defaults::explode(fieldsData.at(NAME), RECORD_SEPARATOR)) {
		// group is trigger(30)type target
		const auto parts(Defaults::explode(group, FIELD_SEPARATOR));
		prettyName.push_back(parts.at(1));
	}
	return Defaults::implode(prettyName, " ➡️ ") + " 🔙";
}

const string InputMapLink::createUniqueId() const {
	return getValue(NAME);
}

const string InputMapLink::getCssClass() const {
	return "LinkBoxButton";
}

