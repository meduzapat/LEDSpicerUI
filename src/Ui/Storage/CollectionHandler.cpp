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

CollectionHandler* CollectionHandler::getInstance(string_view collectionName) noexcept {
	auto [it, inserted] = collections.try_emplace(string(collectionName), new CollectionHandler());
	return it->second;
}

void CollectionHandler::purgeAll() noexcept {
	for (auto& c : collections)
		delete c.second;
	collections.clear();
}

Data* CollectionHandler::get(const string& id) const noexcept {
	return (isIdSet(id) ? collection.at(id) : nullptr);
}

bool CollectionHandler::isSet(const Data* item) const noexcept {
	return isIdSet(item->createUniqueId());
}

bool CollectionHandler::isIdSet(const string& id) const noexcept {
	return collection.find(id) != collection.end();
}

size_t CollectionHandler::countByKey(const string& key, const string& value) const noexcept {
	size_t count = 0;
	for (const auto& item : collection)
		if (item.second->getValue(key) == value) ++count;
	return count;
}

vector<Data*> CollectionHandler::findByProperty(const string& property, const string& value) const noexcept {
	vector<Data*> results;
	for (const auto& item : collection) {
		if (item.second->getProperties().getValue(property) == value) {
			results.push_back(item.second);
		}
	}
	return results;
}

void CollectionHandler::add(Data* item) noexcept {
	if (isSet(item)) return;
	collection.emplace(item->createUniqueId(), item);
	refreshComboBoxes();
}

void CollectionHandler::remove(Data* item) noexcept {

	auto uid{item->createUniqueId()};
	if (uid.empty() or not isIdSet(uid)) return;

	collection.erase(uid);

	// Collect depleted callbacks first, fire after iteration.
	vector<std::function<void()>> pending;
	for (auto& dep : dependencies) {
		dep.collection->remove(item);
		if (dep.minSize and dep.onDepletion and dep.collection->getSize() < dep.minSize)
			pending.push_back(dep.onDepletion);
	}
	refreshComboBoxes();

	for (auto& callback : pending) callback();
}

void CollectionHandler::replace(Data* item, const string& oldId) noexcept {
	if (item->createUniqueId() == oldId) return;
	collection.erase(oldId);
	add(item);
}

void CollectionHandler::registerDependency(const Dependency& dependency) noexcept {
	dependencies.push_back(dependency);
}

void CollectionHandler::refreshComboBox(Gtk::ComboBoxText* comboBox) noexcept {
	comboBox->remove_all();
	for (const auto& item : collection)
		comboBox->append(item.second->createUniqueId(), item.second->createPrettyName());
}

void CollectionHandler::refreshComboBox(
	Gtk::ComboBoxText* comboBox,
	const std::function<bool(const Data*)>& filter
) noexcept {
	comboBox->remove_all();
	for (const auto& item : collection)
		if (filter(item.second))
			comboBox->append(item.second->createUniqueId(), item.second->createPrettyName());
}

void CollectionHandler::registerComboBox(Gtk::ComboBoxText* destination) noexcept {
	comboBoxes.push_back(destination);
}

void CollectionHandler::release(BoxButtonCollection* destination) noexcept {
	auto it = std::find_if(
		dependencies.begin(),
		dependencies.end(),
		[destination](const Dependency& dep) {
			return dep.collection == destination;
		}
	);
	if (it != dependencies.end()) dependencies.erase(it);
}

void CollectionHandler::release(Gtk::ComboBoxText* destination) noexcept {
	comboBoxes.erase(std::find(comboBoxes.begin(), comboBoxes.end(), destination));
}

void CollectionHandler::refreshComboBoxes() noexcept {
	for (auto comboBox : comboBoxes) {
		refreshComboBox(comboBox);
	}
}

