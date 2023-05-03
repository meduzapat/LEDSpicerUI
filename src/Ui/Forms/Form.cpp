/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Fom.cpp
 * @since     Feb 28, 2023
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

#include "Form.hpp"

using namespace LEDSpicerUI::Ui::Forms;


Form::Form(unordered_map<string, string>& data) :
	VBox(false, 2),
	fieldsData(std::move(data))
{
	set_valign(Gtk::ALIGN_START);
}

const string Form::createName() const {
	return fieldsData.at(NAME);
}

string Form::toXML() {
	string r;
	for (const auto& v : fieldsData) {
		r += v.first + "=\"" + v.second + "\" ";
	}
	return r;
}

string Form::getValue(const string& key) const {
	return (fieldsData.count(key) ? fieldsData.at(key) : "");
}

void Form::setValue(const string& key, const string& value) {
	if (fieldsData.count(key))
		fieldsData.at(key) = value;
}

const unordered_map<string, string>* Form::getValues() const {
	return &fieldsData;
}

bool Form::canEdit() const {
	return true;
}

bool Form::canDelete() const {
	return true;
}
