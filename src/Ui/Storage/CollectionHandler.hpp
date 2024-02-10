/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.hpp
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

#include "Message.hpp"
#include "DataDialogs/DialogForm.hpp"

#ifndef ELEMENTHANDLER_HPP_
#define ELEMENTHANDLER_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::CollectionHandler
 */
class CollectionHandler {

public:

	CollectionHandler() = delete;

	virtual ~CollectionHandler() = default;

	/**
	 * Collection handler factory.
	 * @param collectionName
	 * @return an instance of that collection.
	 */
	static CollectionHandler* getInstance(const string& collectionName);

	/**
	 * Removes all stored collections.
	 */
	static void wipe();

	/**
	 * Get the number of registered items.
	 * @return
	 */
	const size_t getSize() const;

	/**
	 * Refresh a single combobox contents with the collection values.
	 * @param comboBox the combobox to refresh.
	 */
	void refreshComboBox(Gtk::ComboBoxText* comboBox);

	/**
	 * Refresh a single combobox contents with the collection values.
	 * @param comboBox the combobox to refresh.
	 * @param ignoreList if set will ignore any elements in this list.
	 */
	void refreshComboBox(Gtk::ComboBoxText* comboBox, const vector<string>& ignoreList);

	/**
	 * Returns true if the item is registered.
	 * @param item
	 * @return
	 */
	const bool isUsed(const string& item) const;

	/**
	 * Search the collection and count the number of occurrences.
	 * @param search a text to search on every value inside the collection.
	 * @return
	 */
	const size_t count(const string& search) const;

	/**
	 * Returns the list of registered items.
	 * @return
	 */
	std::vector<string> const& get() const;

	/**
	 * Adds an item to the collection.
	 * @param item
	 */
	void add(const string& item);

	/**
	 * Removes an item from the collection.
	 * @param item
	 */
	void remove(const string& item);

	/**
	 * Replace an item in the collection.
	 * @param oldItem
	 * @param newItem
	 */
	void replace(const string& oldItem, const string& newItem);

	/**
	 * Register a collection consumer.
	 * @param destination
	 */
	void registerDestination(BoxButtonCollection* destination);

	/**
	 * Register a collection consumer.
	 * @param destination
	 */
	void registerDestination(Gtk::ComboBoxText* destination);

	/**
	 * Removes a collection consumer.
	 * @param destination
	 */
	void release(BoxButtonCollection* destination);

	/**
	 * Removes a collection consumer.
	 * @param destination
	 */
	void release(Gtk::ComboBoxText* destination);

	vector<string>::iterator begin();

	vector<string>::iterator end();

	vector<string>::const_iterator begin() const;

	vector<string>::const_iterator end() const;

protected:

	CollectionHandler(const string& collectionName);

	/// List of collection names.
	vector<string> collection;

	/// List of containers.
	vector<BoxButtonCollection*> destinationGroups;

	/// List of selectors.
	vector<Gtk::ComboBoxText*> destinationComboBoxes;

	/// Keeps collections instances.
	static unordered_map<string, CollectionHandler*> collections;

	/**
	 * Sort and populates comboboxes.
	 */
	void populateComboboxesSorted();

};

} /* namespace */

#endif /* ELEMENTHANDLER_HPP_ */
