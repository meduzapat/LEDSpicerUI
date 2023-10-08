/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Sep 25, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

using namespace LEDSpicerUI::Ui::Forms;

Profile::Profile(unordered_map<string, string>& data) : Form(data) {
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
}

string const Profile::createPrettyName() const {
	return fieldsData.at(NAME);
}

void Profile::resetForm(Modes mode) {}

void Profile::isValid(Modes mode) {}

void Profile::storeData(Modes mode) {}

void Profile::retrieveData(Modes mode) {}

void Profile::cancelData(Modes mode) {}

const string Profile::getCssClass() const {
	return "ProfileBoxButton";
}

