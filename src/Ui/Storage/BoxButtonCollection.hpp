/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.hpp
 * @since     Apr 14, 2023
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

#include "OrdenableFlowBox.hpp"
#include "BoxButton.hpp"

#ifndef BOXBUTTONCOLLECTION_HPP_
#define BOXBUTTONCOLLECTION_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::BoxButtonCollection
 * Represents a collection of BoxButton items.
 *
 * This class manages a collection of BoxButton items, providing functionality
 * for adding, removing, renaming, and retrieving items. It also supports ordering
 * of items.
 */
class BoxButtonCollection {

public:

	/**
	 * SearchTypes
	 * Defines the types of searches for BoxButton items.
	 */
	struct SearchTypes {
		bool
			/// true to search for the whole name, false to search for one or more occurrences of the name.
			absolute = true,
			/// true when the list allows only one occurrence of each value (no duplicates), false otherwise.
			unique   = true;
	};

	/**
	 * Default constructor.
	 */
	BoxButtonCollection() = default;

	/**
	 * Parameterized constructor with a key.
	 * @param key The name of the field representing the unique key.
	 */
	BoxButtonCollection(const string& key) : key(key) {}

	/**
	 * Parameterized constructor with a search type.
	 * @param searchType The type of search to perform over the key.
	 */
	BoxButtonCollection(const SearchTypes searchType) : searchType(searchType) {}

	/**
	 * Parameterized constructor with a key and search type.
	 * @param key The name of the field representing the unique key.
	 * @param searchType The type of search to perform over the key.
	 */
	BoxButtonCollection(const string& key, const SearchTypes searchType) :
		key(key),
		searchType(searchType) {}

	/**
	 * Move constructor.
	 * @param other The BoxButtonCollection to be moved.
	 */
	BoxButtonCollection(BoxButtonCollection&& other) noexcept :
		key(std::move(other.key)),
		searchType(std::move(other.searchType)),
		items(std::move(other.items)),
		itemsOrder(std::move(other.itemsOrder)) {}

	/**
	 * Move assignment operator.
	 * @param other The BoxButtonCollection to be moved.
	 * @return Reference to the modified BoxButtonCollection.
	 */
	BoxButtonCollection& operator=(BoxButtonCollection&& other) noexcept {
		if (this != &other) {
			key        = std::move(other.key);
			searchType = std::move(other.searchType);
			items      = std::move(other.items);
			itemsOrder = std::move(other.itemsOrder);
		}
		return *this;
	}

	/**
	 * Destructor.
	 */
	~BoxButtonCollection();

	/**
	 * Get the size of the collection.
	 * @return The number of items in the collection.
	 */
	const size_t getSize() const;

	/**
	 * Check if an item with the specified name exists in the collection.
	 * @param name The name to check.
	 * @return True if an item with the given name exists, false otherwise.
	 */
	bool isset(const string& name) const;

	/**
	 * Add a BoxButton item to the collection.
	 * @param form The Data object to create a BoxButton.
	 * @return Pointer to the added BoxButton item.
	 */
	BoxButton* add(Data* form);

	/**
	 * Remove a BoxButton item from the collection.
	 * @param item Pointer to the BoxButton item to be removed.
	 */
	void remove(BoxButton* item);

	/**
	 * Remove a BoxButton item with the specified name from the collection.
	 * @param name The name of the item to be removed.
	 */
	void remove(const string& name);

	/**
	 * Rename a BoxButton item.
	 * @param name The current name of the item.
	 * @param newName The new name for the item.
	 */
	void rename(const string& name, const string& newName);

	/**
	 * Populate an OrdenableFlowBox with the BoxButton items.
	 * @param box The OrdenableFlowBox to be populated.
	 */
	void populateBox(OrdenableFlowBox* box);

	/**
	 * Reindex the items in the collection for ordering.
	 * @param box The OrdenableFlowBox containing the items.
	 */
	void reindex(OrdenableFlowBox* box);

	/**
	 * Clear all items from the collection.
	 * It doesn't delete the contets, it just wipe the box.
	 */
	void wipe();

	/**
	 * Get the BoxButton item at the specified position.
	 * @param position The position of the item.
	 * @return Pointer to the BoxButton item.
	 */
	BoxButton* at(uint position);

	/**
	 * Get an iterator pointing to the beginning of the collection.
	 * @return Iterator pointing to the beginning of the collection.
	 */
	vector<BoxButton*>::iterator begin();

	/**
	 * Get an iterator pointing to the end of the collection.
	 * @return Iterator pointing to the end of the collection.
	 */
	vector<BoxButton*>::iterator end();

	/**
	 * Get a const iterator pointing to the beginning of the collection.
	 * @return Const iterator pointing to the beginning of the collection.
	 */
	vector<BoxButton*>::const_iterator begin() const;

	/**
	 * Get a const iterator pointing to the end of the collection.
	 * @return Const iterator pointing to the end of the collection.
	 */
	vector<BoxButton*>::const_iterator end() const;

protected:

	/// The name of the field that represent the unique key.
	string key = NAME;

	/// The type of search to do over the key.
	SearchTypes searchType{true, true};

	vector<BoxButton*>
		/// Created items in the dialog.
		items,
		/// Array to be used when ordering items is needed.
		itemsOrder;
};

} /* namespace */

#endif /* BOXBUTTONCOLLECTION_HPP_ */
