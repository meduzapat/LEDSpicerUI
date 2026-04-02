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

#include "CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Data::~Data() {
	unregisterFromCollection();
}

const string Data::createPrettyName() const noexcept {
	return getPrimaryValue();
}

const string Data::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({getPrimaryValue()});
}

const string Data::getPrimaryValue() const noexcept {
	return getValue(getPrimaryKey());
}

const string& Data::getValue(const string& key) const noexcept {
	if (auto it = fieldsData.find(key); it != fieldsData.end())
		return it->second;
	return emptyString;
}

string Data::getValue(const string& key, const string& defaultValue) const noexcept {
	if (auto it = fieldsData.find(key); it != fieldsData.end())
		return it->second;
	return defaultValue;
}

void Data::unSet(const string& key) noexcept {
	fieldsData.erase(key);
}

void Data::setValue(const string& key, const string& value) noexcept {
	const string oldId{createUniqueId()};
	fieldsData[key] = value;
	handleRegistration(oldId);
}

StringUMap Data::copyValues() const noexcept {
	if (collectionId.empty()) return {};
	string baseId{createUniqueId()};
	string candidateId;
	uint8_t count{1};
	do {
		candidateId = baseId + "_" + std::to_string(count++);
		if (not count) {
			baseId += "X";
		}
	} while (getCollectionHandler()->isIdSet(candidateId));
	StringUMap copy{fieldsData};
	copy[getPrimaryKey()] = candidateId;
	return copy;
}

const StringUMap* Data::getValues() const noexcept {
	return &fieldsData;
}

void Data::setValues(const StringUMap& values) noexcept {
	const string oldId{createUniqueId()};
	fieldsData.insert(values.begin(), values.end());
	handleRegistration(oldId);
}

void Data::wipe() noexcept {
	if (not collectionId.empty()) getCollectionHandler()->remove(this);
	fieldsData.clear();
}

const string Data::toXML() const noexcept {
	StringUMap filtered;
	for (const auto& [k, v] : fieldsData)
		if (shouldSerialize(k, v))
			filtered.emplace(k, v);
	const string body(xmlBody());
	string r(createOpeningXML(string(getXmlTag()), filtered, {}, body.empty()));
	if (not body.empty())
		r += body + createClosingXML(string(getXmlTag()));
	return r;
}

void Data::setProperty(const string& key, const string& value) noexcept {
	properties[key] = value;
}

const string& Data::getProperty(const string& key) const noexcept {
	if (auto it = properties.find(key); it != properties.end())
		return it->second;
	return emptyString;
}

string Data::getProperty(
	const string& key,
	const string& defaultProperty
) const noexcept {
	if (auto it = properties.find(key); it != properties.end())
		return it->second;
	return defaultProperty;
}

bool Data::hasProperty(const string& key) const noexcept {
	return properties.find(key) != properties.end();
}

void Data::removeProperty(const string& key) noexcept {
	properties.erase(key);
}

const StringUMap& Data::getProperties() const noexcept {
	return properties;
}

StringUMap& Data::createEmptyData() noexcept {
	static StringUMap empty;
	return empty;
}

CollectionHandler* Data::getCollectionHandler() const noexcept {
	if (collectionId.empty()) return nullptr;
	return CollectionHandler::getInstance(collectionId);
}

void Data::registerToCollection() noexcept {
	if (not collectionId.empty() and not createUniqueId().empty())
		getCollectionHandler()->add(this);
}

void Data::unregisterFromCollection() noexcept {
	if (not collectionId.empty() and not createUniqueId().empty())
		getCollectionHandler()->remove(this);
}

void Data::handleRegistration(const string& oldId) noexcept {
	if (collectionId.empty()) return;
	const string newId{createUniqueId()};
//	 No registration needed.
//	if (newId == oldId) return;
//	 Remove if new ID is empty, if this a possible scenario?.
//	if (newId.empty())
//		getCollectionHandler()->remove(this);
//	Add if old ID is empty.
//	else if (oldId.empty())
//		getCollectionHandler()->add(this);
//	else
	// Replace if both IDs are non-empty and different.
	if (oldId != newId)
		getCollectionHandler()->replace(this, oldId);
}

string Data::valuesXML(
	const StringUSet& ignored,
	const StringUMap& data
) noexcept {
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

string Data::createOpeningXML(
	const string& node,
	const StringUMap& data,
	const StringUSet& ignored,
	bool empty
) noexcept {
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

string Data::createClosingXML(const string& node) noexcept {
	Defaults::reduceTab();
	string r(Defaults::tab() + "</" + node + ">\n");
	return r;
}
