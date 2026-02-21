/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.cpp
 * @since     Mar 30, 2023
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

unordered_map<string, CollectionHandler*> CollectionHandler::collections;

void CollectionHandler::purgeAll() {
	for (auto& c : collections)
		delete c.second;
	collections.clear();
}

CollectionHandler* CollectionHandler::getInstance(const string& collectionName) {
	auto [it, inserted] = collections.try_emplace(collectionName, new CollectionHandler());
	return it->second;
}

size_t CollectionHandler::getSize() const {
	return collection.size();
}

Data* CollectionHandler::get(const string &id) const {
	return (isIdSet(id) ? collection.at(id) : nullptr);
}

bool CollectionHandler::isSet(const Data* item) const {
	return isIdSet(item->createUniqueId());
}

bool CollectionHandler::isIdSet(const string& id) const {
	return collection.find(id) != collection.end();
}

size_t CollectionHandler::countByKey(const string& key, const string& value) const {
	size_t count = 0;
	for (const auto& item : collection)
		if (item.second->getValue(key) == value) ++count;
	return count;
}

vector<Data*> CollectionHandler::findByProperty(const string& property, const string& value) {
	vector<Data*> results;
	for (auto& [id, data] : collection) {
		if (data->getProperty(property) == value) {
			results.push_back(data);
		}
	}
	return results;
}

void CollectionHandler::add(Data* item) {
	collection.emplace(item->createUniqueId(), item);
	refreshComboBoxes();
}

void CollectionHandler::remove(Data* item) {
	collection.erase(item->createUniqueId());
	for (auto destination : dependencies)
		destination->remove(item);
	refreshComboBoxes();
}

void CollectionHandler::replace(Data* item, const string& oldId) {
	if (item->createUniqueId() == oldId) return;
	collection.erase(oldId);
	add(item);
}

void CollectionHandler::registerDependency(BoxButtonCollection* destination) {
	dependencies.push_back(destination);
}

void CollectionHandler::registerComboBox(Gtk::ComboBoxText* destination) {
	comboBoxes.push_back(destination);
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox) {
	comboBox->remove_all();
	for (const auto& item : collection)
		comboBox->append(item.second->createPrettyName());
}

void CollectionHandler::release(BoxButtonCollection* destination) {
	auto it = std::find(dependencies.begin(), dependencies.end(), destination);
	if (it != dependencies.end()) {
		dependencies.erase(it);
		return;
	}
}

void CollectionHandler::release(Gtk::ComboBoxText* destination) {
	comboBoxes.erase(std::find(comboBoxes.begin(), comboBoxes.end(), destination));
}

StringDataMap::iterator CollectionHandler::begin() {
	return collection.begin();
}

StringDataMap::iterator CollectionHandler::end() {
	return collection.end();
}

StringDataMap::const_iterator CollectionHandler::begin() const {
	return collection.begin();
}

StringDataMap::const_iterator CollectionHandler::end() const {
	return collection.end();
}
void CollectionHandler::refreshComboBoxes() {
	for (auto comboBox : comboBoxes) {
		refreshComboBox(comboBox);
	}
}

