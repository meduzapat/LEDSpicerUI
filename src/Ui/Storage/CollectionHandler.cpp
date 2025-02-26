/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.cpp
 * @since     Mar 30, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox) {
	comboBox->remove_all();
	for (const auto& item : collection)
		comboBox->append(item);
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox, const vector<string>& ignoreList) {
	comboBox->remove_all();
	for (const auto& item : collection)
		if (std::find(ignoreList.begin(), ignoreList.end(), item) == ignoreList.end())
			comboBox->append(item);
}

bool CollectionHandler::isUsed(const string& item) const {
	return collection.count(item);
}

size_t CollectionHandler::count(const string& search) const {
	return std::count_if(
		collection.begin(),
		collection.end(),
		[search](const auto& item) {
			return item.find(search) != string::npos;
		}
	);
}

std::set<string> const& CollectionHandler::get() const {
	return collection;
}

void CollectionHandler::add(const string& item) {
	if (item.empty()) return;
	collection.emplace(item);
	populateComboboxesSorted();
}

void CollectionHandler::remove(const string& item) {
	if (item.empty()) return;
	collection.erase(item);
	for (auto destination : destinationGroups)
		destination->remove(item);
	populateComboboxesSorted();
}

void CollectionHandler::replace(const string& oldItem, const string& newItem) {
	if (oldItem.empty() or not isUsed(oldItem) or oldItem == newItem) return;
	collection.erase(oldItem);
	collection.emplace(newItem);
	for (auto destination : destinationGroups)
		destination->rename(oldItem, newItem);
	populateComboboxesSorted();
}

void CollectionHandler::registerDestination(BoxButtonCollection* destination) {
	destinationGroups.emplace(destination);
}

void CollectionHandler::registerDestination(Gtk::ComboBoxText* destination) {
	destinationComboBoxes.emplace(destination);
}

void CollectionHandler::release(BoxButtonCollection* destination) {
	destinationGroups.erase(destination);
}

void CollectionHandler::release(Gtk::ComboBoxText* destination) {
	destinationComboBoxes.erase(destination);
}

std::set<string>::iterator CollectionHandler::begin() {
	return collection.begin();
}

std::set<string>::iterator CollectionHandler::end() {
	return collection.end();
}

std::set<string>::const_iterator CollectionHandler::begin() const {
	return collection.begin();
}

std::set<string>::const_iterator CollectionHandler::end() const {
	return collection.end();
}

void CollectionHandler::populateComboboxesSorted() {
	for (auto comboBox : destinationComboBoxes) {
		comboBox->remove_all();
		for (const auto& item : collection) {
			comboBox->append(item);
		}
	}
}
