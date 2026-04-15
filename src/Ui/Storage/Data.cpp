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

string Data::createPrettyName() const noexcept {
	return getPrimaryValue();
}

string Data::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({getPrimaryValue()});
}

const string& Data::getPrimaryValue() const noexcept {
	return getValue(getPrimaryKey());
}

StringUMap Data::copyValues() const noexcept {
	auto handler = getCollectionHandler();
	if (not handler) return {};
	string baseId{createUniqueId()};
	string candidateId;
	uint8_t count{1};
	do {
		candidateId = baseId + "_" + std::to_string(count++);
		if (not count) {
			baseId += "X";
		}
	} while (handler->isIdSet(candidateId));
	StringUMap copy{Values::copyValues()};
	copy[getPrimaryKey()] = candidateId;
	return copy;
}

void Data::wipe() noexcept {
	auto handler = getCollectionHandler();
	if (handler) handler->remove(this);
	Values::wipe();
}

void Data::unSet(const string& key) noexcept {
	if (key == getPrimaryKey()) {
		auto handler = getCollectionHandler();
		if (handler) handler->remove(this);
	}
	Values::unSet(key);
}

string Data::toXML() const noexcept {
	StringUMap filtered;
	for (const auto& [k, v] : values)
		if (shouldSerialize(k, v))
			filtered.emplace(k, v);
	string body(xmlBody());
	string r(createOpeningXML(string(getXmlTag()), filtered, body.empty()));
	if (not body.empty())
		r += body + createClosingXML(string(getXmlTag()));
	return r;
}

void Data::registerToCollection() noexcept {
	auto handler = getCollectionHandler();
	if (handler and not createUniqueId().empty())
		handler->add(this);
}

void Data::unregisterFromCollection() noexcept {
	auto handler = getCollectionHandler();
	if (handler and not createUniqueId().empty())
		handler->remove(this);
}

void Data::syncRegistration(const string& oldId) noexcept {
	auto handler = getCollectionHandler();
	if (not handler) return;
	// ID will never be empty.
	string newId{createUniqueId()};
	// Replace if both IDs are non-empty and different.
	if (oldId != newId)
		handler->replace(this, oldId);
}

string Data::valuesXML(const StringUMap& data) noexcept {
	string r, el, tab(" ");
	if (data.size() > 2) {
		el  = "\n";
		tab = Defaults::tab();
	}
	for (const auto& v : data)
		r += tab + v.first + "=\"" + v.second + "\"" + el;
	return r;
}

string Data::createOpeningXML(
	const string&     node,
	const StringUMap& data,
	bool              empty
) noexcept {
	string r(Defaults::tab() + "<" + node);
	if (data.size() > 2) {
		r += "\n";
		Defaults::increaseTab();
		r += valuesXML(data);
		Defaults::reduceTab();
		r += Defaults::tab();
	}
	else {
		r += valuesXML(data);
	}
	r += empty ? "/>\n" : ">\n";
	if (not empty)
		Defaults::increaseTab();
	return r;
}

string Data::createClosingXML(const string& node) noexcept {
	Defaults::reduceTab();
	string r(Defaults::tab() + "</" + node + ">\n");
	return r;
}
