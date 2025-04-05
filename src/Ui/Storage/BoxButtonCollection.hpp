/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.hpp
 * @since     Apr 14, 2023
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

#include "OrdenableFlowBox.hpp"
#include "BoxButton.hpp"

#ifndef BOXBUTTONCOLLECTION_HPP_
#define BOXBUTTONCOLLECTION_HPP_ 1

using BoxButtonVector = vector<std::unique_ptr<LEDSpicerUI::Ui::Storage::BoxButton>>;

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::BoxButtonCollection
 * Represents a collection of BoxButton items.
 *
 * This class manages a collection of BoxButton items, providing functionality
 * for adding, removing, renaming, and retrieving items. It also keeps ordering
 * of items.
 */
class BoxButtonCollection {

public:

	/**
	 * Default constructor.
	 */
	BoxButtonCollection() = default;

	/**
	 * Move constructor.
	 * @param other The BoxButtonCollection to be moved.
	 */
	BoxButtonCollection(BoxButtonCollection&& other) noexcept :
		items(std::move(other.items)) {}

	/**
	 * Move assignment operator.
	 * @param other The BoxButtonCollection to be moved.
	 * @return Reference to the modified BoxButtonCollection.
	 */
	BoxButtonCollection& operator=(BoxButtonCollection&& other) noexcept {
		if (this != &other) {
			items = std::move(other.items);
		}
		return *this;
	}

	~BoxButtonCollection() = default;

	/**
	 * Get the size of the collection.
	 * @return The number of items in the collection.
	 */
	size_t getSize() const;

	/**
	 * Check if an item with the Data is set in the collection.
	 * @param form The Data object to check.
	 * @return True if the item is set, false otherwise.
	 */
	bool isSet(Data* form) const;

	/**
	 * Check if an item with the specified name exists in the collection.
	 * @param name The name to check.
	 * @return True if an item with the given name exists, false otherwise.
	 */
	bool isIdSet(const string& name) const;

	/**
	 * Creates a new BoxButton item and adds it to the collection.
	 * @param form The Data object to create a BoxButton.
	 * @return Reference to the added BoxButton item.
	 */
	BoxButton& create(Data* form);

	/**
	 * Remove a BoxButton item from the collection.
	 * @param item Pointer to the BoxButton item to be removed.
	 */
	void remove(BoxButton& item);

	/**
	 * Remove a BoxButton item from the collection by its data.
	 * @param form Pointer to the Data object to delete.
	 */
	void remove(Data* form);

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
	 * Destroys all items from the collection.
	 */
	void wipe();

	/**
	 * Get an iterator pointing to the beginning of the collection.
	 * @return Iterator pointing to the beginning of the collection.
	 */
	BoxButtonVector::iterator begin();

	/**
	 * Get an iterator pointing to the end of the collection.
	 * @return Iterator pointing to the end of the collection.
	 */
	BoxButtonVector::iterator end();

	/**
	 * Get a const iterator pointing to the beginning of the collection.
	 * @return Const iterator pointing to the beginning of the collection.
	 */
	BoxButtonVector::const_iterator begin() const;

	/**
	 * Get a const iterator pointing to the end of the collection.
	 * @return Const iterator pointing to the end of the collection.
	 */
	BoxButtonVector::const_iterator end() const;

protected:

	/// Created items in the dialog.
	BoxButtonVector items;

};

} /* namespace */

#endif /* BOXBUTTONCOLLECTION_HPP_ */
