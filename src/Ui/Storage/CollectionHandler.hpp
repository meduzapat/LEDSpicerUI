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

#include "Message.hpp"
#include "Storage/BoxButtonCollection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

using BoxButtonCollections = vector<BoxButtonCollection*>;

/**
 * LEDSpicerUI::Ui::Storage::CollectionHandler
 */
class CollectionHandler {

public:

	virtual ~CollectionHandler() = default;

	/**
	 * Collection handler factory.
	 * @param collectionName
	 * @return an instance of that collection.
	 */
	static CollectionHandler* getInstance(const string& collectionName);

	/**
	 * Removes and deletes all stored collections.
	 */
	static void purgeAll();

	/**
	 * @return the number of registered items.
	 */
	size_t getSize() const;

	/**
	 * @param id
	 * @return an item by its ID.
	 */
	Data* get(const string& id) const;

	/**
	 * @param item
	 * @return true if the item is registered.
	 */
	bool isSet(const Data* item) const;

	/**
	 * @param id
	 * @return true if the ID is registered.
	 */
	bool isIdSet(const string& id) const;

	/**
	 * Search the collection for a value, and count the number of occurrences.
	 * @param value the value to search on every BoxButton inside the collection.
	 * @return the number of occurrences.
	 */
	size_t countByKey(const string& key, const string& value) const;

	/**
	 * Finds all data with matching property.
	 * @param property Property name.
	 * @param value Property value.
	 * @return Vector of matching Data pointers.
	 */
	vector<Data*> findByProperty(const string& property, const string& value);

	/**
	 * Adds an item to the collection.
	 * @param item
	 */
	void add(Data* item);

	/**
	 * Removes an item from the collection and removing consumers that uses that item.
	 * @param item
	 */
	void remove(Data* item);

	/**
	 * Replace an item in the collection.
	 * @param oldItem
	 * @param newItem
	 */
	void replace(Data* item, const string& oldId);

	/**
	 * Register a collection dependency to be tracked.
	 * @param destination
	 */
	void registerDependency(BoxButtonCollection* destination);

	/**
	 * Refresh a single combobox contents with the collection values.
	 * @param comboBox the combobox to refresh.
	 */
	void refreshComboBox(Gtk::ComboBoxText* comboBox);

	/**
	 * Register a collection consumer.
	 * @param destination
	 */
	void registerComboBox(Gtk::ComboBoxText* destination);

	/**
	 * Removes a collection consumer.
	 * @param destination
	 */
	void release(BoxButtonCollection* destination);

	/**
	 * Removes a tracked dependency or selection binding.
	 * @param destination
	 */
	void release(Gtk::ComboBoxText* destination);

	StringDataMap::iterator begin();
	StringDataMap::iterator end();
	StringDataMap::const_iterator begin() const;
	StringDataMap::const_iterator end() const;

protected:

	/// Stores the collection of items indexed by their unique identifier.
	StringDataMap collection;

	/// List of collections that keeps references to items in the collection.
	BoxButtonCollections dependencies;

	/// List of selectors.
	vector<Gtk::ComboBoxText*> comboBoxes;

	/// Keeps collections instances.
	static unordered_map<string, CollectionHandler*> collections;

	/**
	 * Avoids instantiation.
	 */
	CollectionHandler() = default;

	/**
	 * Populates comboboxes.
	 */
	void refreshComboBoxes();
};

} // namespace
