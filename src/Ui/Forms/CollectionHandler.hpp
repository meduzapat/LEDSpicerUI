/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandler.hpp
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

#include "Message.hpp"
#include "../DialogForm.hpp"

#ifndef ELEMENTHANDLER_HPP_
#define ELEMENTHANDLER_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::CollectionHandler
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
	size_t getSize();

	/**
	 * Returns true if the item is registered.
	 * @param item
	 * @return
	 */
	bool isUsed(const std::string& item);

	/**
	 * Search the collection and count the number of occurrences.
	 * @param search
	 * @return
	 */
	int count(const std::string& search);

	/**
	 * Returns the list of registered items.
	 * @return
	 */
	std::vector<std::string> const& get();

	/**
	 * Adds an item to the collection.
	 * @param item
	 */
	void add(const std::string& item);

	/**
	 * Removes an item from the collection.
	 * @param item
	 */
	void remove(const std::string& item);

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

	void registerDestination(Gtk::ComboBoxText* destination);

	/**
	 * Removes a collection consumer.
	 * @param destination
	 */
	void release(BoxButtonCollection* destination);

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
	vector<Gtk::ComboBoxText*> comboboxes;

	/// Keeps collections instances.
	static unordered_map<string, CollectionHandler*> collections;

};

} /* namespace */

#endif /* ELEMENTHANDLER_HPP_ */
