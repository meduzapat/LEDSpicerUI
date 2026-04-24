/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.hpp
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

#include "Storage/BoxButtonCollection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::CollectionHandler
 */
class CollectionHandler : public SensitivityTracker {

public:

	/// Binds a widget's sensitivity to this collection's size.
	struct SensitivityBinding {
		Gtk::Widget* widget;
		size_t       minCount = 1;
	};

	virtual ~CollectionHandler() = default;

	/**
	 * Collection handler factory.
	 * @param collectionName
	 * @return an instance of that collection.
	 */
	static CollectionHandler* getInstance(const string& collectionName) noexcept;

	/**
	 * Removes and deletes a collection instance.
	 * @param collectionName
	 * @return the removed instance, or nullptr if not found.
	 */
	static void removeInstance(const string& collectionName) noexcept;

	/**
	 * Removes and deletes all stored collections.
	 */
	static void purgeAll() noexcept;

	/**
	 * @return the number of registered items.
	 */
	size_t getSize() const noexcept override { return collection.size(); }

	/**
	 * @param id
	 * @return an item by its ID.
	 */
	Data* get(const string& id) const noexcept;

	/**
	 * @param item
	 * @return true if the item is registered.
	 */
	bool isSet(const Data* item) const noexcept;

	/**
	 * @param id
	 * @return true if the ID is registered.
	 */
	bool isIdSet(const string& id) const noexcept;

	/**
	 * Search the collection for a value, and count the number of occurrences.
	 * @param value the value to search on every BoxButton inside the collection.
	 * @return the number of occurrences.
	 */
	size_t countByKey(const string& key, const string& value) const noexcept;

	/**
	 * Finds all data with matching property.
	 * @param property Property name.
	 * @param value Property value.
	 * @return Vector of matching Data pointers.
	 */
	vector<Data*> findByProperty(const string& property, const string& value) const noexcept;

	/**
	 * Adds an item to the collection.
	 * @param item
	 */
	void add(Data* item) noexcept;

	/**
	 * Removes an item from the collection and removing consumers that uses that item.
	 * @param item
	 */
	void remove(Data* item) noexcept;

	/**
	 * Replace an item in the collection.
	 * @param oldItem
	 * @param newItem
	 */
	void replace(Data* item, const string& oldId) noexcept;

	/**
	 * Register a collection dependency to be tracked.
	 * @param dependency Dependency struct with collection, optional min size and callback.
	 */
	void registerDependency(BoxButtonCollection* dependency) noexcept;

	/**
	 * Refresh a single combobox contents with the collection values.
	 * @param comboBox the combobox to refresh.
	 */
	void refreshComboBox(Gtk::ComboBoxText* comboBox) noexcept;

	/**
	 * Refresh a single combobox contents with the collection values,
	 * excluding items that have any of the given properties set.
	 * @param comboBox the combobox to refresh.
	 * @param excludeProperties properties — items with any of these set are skipped.
	 */
	void refreshComboBox(Gtk::ComboBoxText* comboBox, const vector<string>&  excludeProperties) noexcept;

	/**
	 * Removes a collection consumer.
	 * @param destination
	 */
	void release(BoxButtonCollection* destination) noexcept;

	auto begin()       noexcept { return collection.begin(); }
	auto end()         noexcept { return collection.end();   }
	auto begin() const noexcept { return collection.begin(); }
	auto end()   const noexcept { return collection.end();   }

protected:

	/// Stores the collection of items indexed by their unique identifier.
	StringDataPtrMap collection;

	/// List of collections that keeps references to items in the collection.
	vector<BoxButtonCollection*> dependencies;

	/// Keeps collections instances.
	static std::unordered_map<string, CollectionHandler*> collections;

	/**
	 * Avoids instantiation.
	 */
	CollectionHandler() = default;

};

} // namespace
