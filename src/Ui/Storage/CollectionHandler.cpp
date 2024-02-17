/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.cpp
 * @since     Mar 30, 2023
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

#include "CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;

unordered_map<string, CollectionHandler*> CollectionHandler::collections;

CollectionHandler::CollectionHandler(const string& collectionName) {
	collections.emplace(collectionName, this);
}

void CollectionHandler::wipe() {
	for (auto& c : collections)
		delete c.second;
}

CollectionHandler* CollectionHandler::getInstance(const string& collectionName) {
	if (not collections.count(collectionName))
		new CollectionHandler(collectionName);
	return collections.at(collectionName);
}

const size_t CollectionHandler::getSize() const {
	return collection.size();
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox) {
	Defaults::populateComboBoxText(comboBox, collection);
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox, const vector<string>& ignoreList) {
	comboBox->remove_all();
	for (const auto& item : collection)
		if (std::find(ignoreList.begin(), ignoreList.end(), item) == ignoreList.end())
			comboBox->append(item);
}

const bool CollectionHandler::isUsed(const string& item) const {
	return std::find(collection.begin(), collection.end(), item) != collection.end();
}

const size_t CollectionHandler::count(const string& search) const {
	int count = 0;
	for (auto& i : collection)
		if (i.find(search) != i.npos)
			++count;
	return count;
}

std::vector<string> const& CollectionHandler::get() const {
	return collection;
}

void CollectionHandler::add(const string& item) {
	if (not item.empty() and not isUsed(item)) {
		collection.push_back(item);
		std::sort(collection.begin(), collection.end());
		populateComboboxesSorted();
	}
}

void CollectionHandler::remove(const string& item) {
	if (item.empty() or not isUsed(item))
		return;
	collection.erase(std::remove(collection.begin(), collection.end(), item), collection.end());
	for (auto destination : destinationGroups)
		destination->remove(item);
	populateComboboxesSorted();
}

void CollectionHandler::replace(const string& oldItem, const string& newItem) {
	if (oldItem.empty() or not isUsed(oldItem))
		return;
	if (oldItem != newItem) {
		std::replace(collection.begin(), collection.end(), oldItem, newItem);
		for (auto destination : destinationGroups)
			destination->rename(oldItem, newItem);
		std::sort(collection.begin(), collection.end());
		populateComboboxesSorted();
	}
}

void CollectionHandler::registerDestination(BoxButtonCollection* destination) {
	destinationGroups.push_back(destination);
}

void CollectionHandler::registerDestination(Gtk::ComboBoxText* destination) {
	destinationComboBoxes.push_back(destination);
}

void CollectionHandler::release(BoxButtonCollection* destination) {
	destinationGroups.erase(
		std::remove(destinationGroups.begin(), destinationGroups.end(), destination),
		destinationGroups.end()
	);
}

void CollectionHandler::release(Gtk::ComboBoxText* destination) {
	destinationComboBoxes.erase(
		std::remove(destinationComboBoxes.begin(), destinationComboBoxes.end(), destination),
		destinationComboBoxes.end()
	);
}

vector<string>::iterator CollectionHandler::begin() {
	return collection.begin();
}

vector<string>::iterator CollectionHandler::end() {
	return collection.end();
}

vector<string>::const_iterator CollectionHandler::begin() const {
	return collection.begin();
}

vector<string>::const_iterator CollectionHandler::end() const {
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
