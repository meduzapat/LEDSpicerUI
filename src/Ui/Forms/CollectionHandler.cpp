/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.cpp
 * @since     Mar 30, 2023
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

#include "CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Forms;

unordered_map<string, CollectionHandler*> CollectionHandler::collections;

CollectionHandler::CollectionHandler(const string& collectionName) {
	if (collections.count(collectionName))
		throw Message("collection already exists.");
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

size_t CollectionHandler::getSize() {
	return collection.size();
}

bool CollectionHandler::isUsed(const std::string& item) {
	auto iter = std::find(collection.begin(), collection.end(), item);
	return iter != collection.end();
}

int CollectionHandler::count(const std::string& search) {
	int count = 0;
	for (auto& i : collection)
		if (i.find(search) != i.npos)
			++count;
	return count;
}

void CollectionHandler::add(const std::string& item) {
	if (std::find(collection.begin(), collection.end(), item) == collection.end())
		collection.push_back(item);
}

void CollectionHandler::remove(const std::string& item) {
	collection.erase(std::remove(collection.begin(), collection.end(), item), collection.end());
	for (auto destination : destinationGroups)
		destination->remove(item);
	for (auto c : comboboxes)
		if (c and c->get_active_text() == item)
			c->set_active_text("");
}

void CollectionHandler::replace(const string& oldItem, const string& newItem) {
	if (oldItem != newItem) {
		std::replace(collection.begin(), collection.end(), oldItem, newItem);
		for (auto destination : destinationGroups)
			destination->rename(oldItem, newItem);

		for (auto c : comboboxes)
			if (c and c->get_active_text() == oldItem)
				c->set_active_text(newItem);
	}
}

void CollectionHandler::registerDestination(BoxButtonCollection* destination) {
	destinationGroups.push_back(destination);
}

void CollectionHandler::registerDestination(Gtk::ComboBoxText* destination) {
	comboboxes.push_back(destination);
}

void CollectionHandler::release(BoxButtonCollection* destination) {
	destinationGroups.erase(std::remove(destinationGroups.begin(), destinationGroups.end(), destination), destinationGroups.end());
}

void CollectionHandler::release(Gtk::ComboBoxText* destination) {
	//comboboxes
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
