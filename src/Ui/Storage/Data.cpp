/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.cpp
 * @since     Feb 28, 2023
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

#include "Data.hpp"

using namespace LEDSpicerUI::Ui::Storage;


Data::Data(unordered_map<string, string>& data) :
	VBox(false, 2),
	fieldsData(std::move(data))
{
	set_valign(Gtk::ALIGN_START);
}

Data::~Data() {
	deActivate();
}

const string Data::createPrettyName() const {
	return fieldsData.at(NAME);
}

const string Data::createTooltip() const {
	return "";
}

string Data::getValue(const string& key, const string& defaultValue) const {
	return (fieldsData.count(key) ? fieldsData.at(key) : defaultValue);
}

void Data::setValue(const string& key, const string& value) {
	fieldsData[key] = value;
}

void Data::replaceValue(const string& key, const string& newValue) {
	fieldsData.at(key) = newValue;
}

const unordered_map<string, string>* Data::getValues() const {
	return &fieldsData;
}

void Data::wipe() {
	fieldsData.clear();
}

const string Data::toXML() const {
	return valuesXML(ignored, fieldsData);
}

string Data::valuesXML(const vector<string>& ignored, const unordered_map<string, string>& data) {
	string r, el, tab(" ");
	if (data.size() > 2) {
		el  = "\n";
		tab = Defaults::tab();
	}
	for (const auto& v : data) {
		if (std::find(ignored.begin(), ignored.end(), v.first) == ignored.end())
			r += tab + v.first + "=\"" + v.second + "\"" + el;
	}
	return r;
}

string Data::createOpeningXML(const string& node, const unordered_map<string, string>& data, const vector<string>& ignored, bool empty) {
	string r(Defaults::tab() + "<" + node);
	if (data.size() > 2) {
		r += "\n";
		Defaults::increaseTab();
		r += valuesXML(ignored, data);
		Defaults::reduceTab();
		r += Defaults::tab();
	}
	else {
		r += valuesXML(ignored, data);
	}
	if (empty) {
		r += "/>\n";
	}
	else {
		r += ">\n";
		Defaults::increaseTab();
	}
	return r;
}

string Data::createClosingXML(const string& node) {
	Defaults::reduceTab();
	string r(Defaults::tab() + "</" + node + ">\n");
	return r;
}
