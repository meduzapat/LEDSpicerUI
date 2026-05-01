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

std::unordered_map<string, CollectionHandler*> CollectionHandler::collections;

CollectionHandler* CollectionHandler::getInstance(const string& collectionName) noexcept {
	auto [it, inserted] = collections.try_emplace(collectionName, new CollectionHandler());
	return it->second;
}

void CollectionHandler::removeInstance(const string& collectionName) noexcept {
	auto it = collections.find(collectionName);
	if (it == collections.end()) return;
	delete it->second;
	collections.erase(it);
}

void CollectionHandler::purgeAll() noexcept {
	for (auto& c : collections)
		delete c.second;
	collections.clear();
}

Data* CollectionHandler::get(const string& id) const noexcept {
	auto it{collection.find(id)};
	if (it == collection.end()) return nullptr;
	return it->second;
}

bool CollectionHandler::isSet(const Data* item) const noexcept {
	return isIdSet(item->createUniqueId());
}

bool CollectionHandler::isIdSet(const string& id) const noexcept {
	return collection.find(id) != collection.end();
}

size_t CollectionHandler::countByKey(const string& key, const string& value) const noexcept {
	size_t count{0};
	for (const auto& item : collection)
		if (item.second->getValue(key) == value) ++count;
	return count;
}

vector<Data*> CollectionHandler::findByProperty(const string& property, const string& value) const noexcept {
	vector<Data*> results;
	for (const auto& item : collection)
		if (item.second->getProperties().getValue(property) == value)
			results.push_back(item.second);
	return results;
}

bool CollectionHandler::hasAny(const string& property, const string& value) const noexcept {
	for (const auto& [id, item] : collection)
		if (item->getProperties().getValue(property) == value)
			return true;
	return false;
}

void CollectionHandler::add(Data* item) noexcept {
	auto uid{item->createUniqueId()};
	if (collection.count(uid)) return;
	collection.emplace(uid, item);
	refreshSensitiveWidgets();
}

void CollectionHandler::remove(Data* item) noexcept {
	auto uid{item->createUniqueId()};
	if (uid.empty()) return;

	auto it{collection.find(uid)};
	if (it == collection.end()) return;

	collection.erase(it);

	for (auto dep : dependencies) {
		if (dep->isSet(item)) dep->remove(item);
	}

	refreshSensitiveWidgets();
}

void CollectionHandler::replace(Data* item, const string& oldId) noexcept {
	if (item->createUniqueId() == oldId) return;
	collection.erase(oldId);
	add(item);
	for (auto dep : dependencies) {
		for (auto btn : *dep) {
			if (*btn->getData() == *item) btn->sync();
		}
	}
}

void CollectionHandler::registerDependency(BoxButtonCollection* dependency) noexcept {
	dependencies.push_back(dependency);
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox) noexcept {
	comboBox->remove_all();
	for (const auto& item : collection)
		comboBox->append(item.second->createUniqueId(), item.second->createPrettyName());
}

void CollectionHandler::refreshComboBox(
	Gtk::ComboBoxText*    comboBox,
	const vector<string>& excludeProperties
) noexcept {
	comboBox->remove_all();
	for (const auto& [id, data] : collection) {
		if (std::none_of(
			excludeProperties.begin(),
			excludeProperties.end(),
			[&data](const string& prop) { return data->getProperties().isSet(prop); }
		))
			comboBox->append(data->createUniqueId(), data->createPrettyName());
	}
}

void CollectionHandler::release(BoxButtonCollection* destination) noexcept {
	auto it = std::find_if(
		dependencies.begin(),
		dependencies.end(),
		[destination](const BoxButtonCollection* dep) {
			return dep == destination;
		}
	);
	if (it != dependencies.end()) dependencies.erase(it);
}
