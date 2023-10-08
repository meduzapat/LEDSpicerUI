/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.hpp
 * @since     Apr 14, 2023
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

#include "BoxButton.hpp"
#include "../OrdenableFlowBox.hpp"

#ifndef BOXBUTTONCOLLECTION_HPP_
#define BOXBUTTONCOLLECTION_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::BoxButtonCollection
 */
class BoxButtonCollection {

public:

	BoxButtonCollection() = default;

	BoxButtonCollection(const string& key) : key(key) {}

	BoxButtonCollection(BoxButtonCollection&& other) noexcept :
		key(std::move(other.key)),
		items(std::move(other.items)),
		itemsOrder(std::move(other.itemsOrder)) {}

	BoxButtonCollection& operator=(BoxButtonCollection&& other) noexcept {
		if (this != &other) {
			key        = std::move(other.key);
			items      = std::move(other.items);
			itemsOrder = std::move(other.itemsOrder);
		}
		return *this;
	}

	~BoxButtonCollection() = default;

	size_t size();

	BoxButton* add(string type, Form* form);

	void remove(BoxButton* item);

	void remove(const string& name);

	void rename(const string& name, const string& newName);

	void populateBox(OrdenableFlowBox* box);

	void reindex(OrdenableFlowBox* box);

	void wipe();

	vector<BoxButton*>::iterator begin();

	vector<BoxButton*>::iterator end();

	vector<BoxButton*>::const_iterator begin() const;

	vector<BoxButton*>::const_iterator end() const;

protected:

	string key = NAME;

	/// Created items in the dialog.
	vector<BoxButton*> items;

	/// Array to be used when ordering items is needed.
	vector<BoxButton*> itemsOrder;

};

} /* namespace */

#endif /* BOXBUTTONCOLLECTION_HPP_ */
