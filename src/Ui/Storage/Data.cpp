/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.cpp
 * @since     Feb 28, 2023
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

#include "Data.hpp"

using namespace LEDSpicerUI::Ui::Storage;

bool Data::operator==(const Data& other) const {
	return this == &other;
}

Data::~Data() {
	deActivate();
}

const string Data::createPrettyName() const {
	return fieldsData.at(getPrimaryKey());
}

const string Data::createUniqueId() const {
	return Defaults::createCommonUniqueId({getValue(getPrimaryKey())});
}

const string Data::createTooltip() const {
	return "";
}

string Data::getValue(const string& key, const string& defaultValue) const {
	return (fieldsData.find(key) != fieldsData.end() ? fieldsData.at(key) : defaultValue);
}

void Data::unSet(const string& key) {
	fieldsData.erase(key);
}

void Data::setValue(const string& key, const string& value) {
	fieldsData[key] = value;
}

StringUMap Data::copyValues(uint8_t number) const {
	StringUMap r;
	for (const auto &v : fieldsData) {
		if (v.first == getPrimaryKey()) {
			r[v.first] = v.second  + " copy" + std::to_string(number);
			continue;
		}
		r[v.first] = v.second;
	}
	return r;
}

const StringUMap* Data::getValues() const {
	return &fieldsData;
}

void Data::setValues(const StringUMap& values) {
	for (const auto& valPair : values) {
		fieldsData[valPair.first] = valPair.second;
	}
}

void Data::wipe() {
	fieldsData.clear();
}

void Data::reset() {
	wipe();
}

const string Data::toXML() const {
	return valuesXML(ignored, fieldsData);
}

void Data::setProperty(const string& key, const string& value) {
	properties[key] = value;
}

string Data::getProperty(const string& key, const string& def) const {
	return (properties.find(key) != properties.end()) ? properties.at(key) : def;
}

bool Data::hasProperty(const string& key) const {
	return properties.find(key) != properties.end();
}

void Data::removeProperty(const string& key) {
	properties.erase(key);
}

const StringUMap& Data::getProperties() const {
	return properties;
}

StringUMap& Data::createEmptyData() {
	static StringUMap empty;
	return empty;
}

string Data::valuesXML(
	const StringUSet& ignored,
	const StringUMap& data
) {
	string r, el, tab(" ");
	if (data.size() > 2) {
		el  = "\n";
		tab = Defaults::tab();
	}
	for (const auto& v : data) {
		if (ignored.find(v.first) == ignored.end())
			r += tab + v.first + "=\"" + v.second + "\"" + el;
	}
	return r;
}

const string Data::getPrimaryKey() const {
	return NAME;
}

string Data::createOpeningXML(
	const string& node,
	const StringUMap& data,
	const StringUSet& ignored,
	bool empty
) {
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
