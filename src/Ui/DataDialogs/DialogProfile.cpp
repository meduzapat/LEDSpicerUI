/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.cpp
 * @since     Feb 14, 2023
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

#include "DialogProfile.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogProfile* DialogProfile::instance = nullptr;

void DialogProfile::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogProfile", instance);
	}
}

DialogProfile* DialogProfile::getInstance() {
	return instance;
}

void DialogProfile::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_PROFILES), values);
}

void DialogProfile::clearForm() {

}

void DialogProfile::isValid() const {

}

void DialogProfile::storeData() {

}

void DialogProfile::retrieveData() {

}

string const DialogProfile::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(NAME)});
}


const string DialogProfile::getType() const {
	return "profile";
}

Forms::Form* DialogProfile::getData(unordered_map<string, string>& rawData) {
	return new Storage::Profile(rawData);
}
