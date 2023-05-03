/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.cpp
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

#include "BoxButtonCollection.hpp"

using namespace LEDSpicerUI::Ui::Forms;

BoxButton* BoxButtonCollection::add(string type, Form* form) {
	// Create the button.
	auto b = new BoxButton(type, form);
	items.push_back(b);
	itemsOrder.push_back(b);
	b->show_all();
	return b;
}

void BoxButtonCollection::remove(BoxButton* item) {
	items.erase(std::remove(items.begin(), items.end(), item), items.end());
	itemsOrder.erase(std::remove(itemsOrder.begin(), itemsOrder.end(), item), itemsOrder.end());
	delete item;
}

void BoxButtonCollection::remove(const string& name) {

	for (auto b : items)
		if (b->getForm()->getValue(NAME) == name) {
			remove(b);
			break;
		}
}

void BoxButtonCollection::rename(const string& name, const string& newName) {
	for (auto b : items) {
		if (b->getForm()->getValue(NAME) == name) {
			b->getForm()->setValue(NAME, newName);
			b->updateLabel();
			break;
		}
	}
}

void BoxButtonCollection::populateBox(OrdenableFlowBox* box) {
	for (auto i : itemsOrder)
		box->add(*i);
	box->show_all();
}

void BoxButtonCollection::reindex(OrdenableFlowBox* box) {
	itemsOrder.clear();
	for (auto child : box->get_children()) {
		auto c = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto b = dynamic_cast<Forms::BoxButton*>(c->get_child());
		for (auto i : items) {
			if (i == b) {
				itemsOrder.push_back(b);
				break;
			}
		}
	}
}

void BoxButtonCollection::wipe() {
	for (auto i : items)
		delete i;
	items.clear();
	itemsOrder.clear();
}

vector<BoxButton*>::iterator BoxButtonCollection::begin() {
	return itemsOrder.begin();
}

vector<BoxButton*>::iterator BoxButtonCollection::end() {
	return itemsOrder.end();
}

vector<BoxButton*>::const_iterator BoxButtonCollection::begin() const {
	return itemsOrder.begin();
}

vector<BoxButton*>::const_iterator BoxButtonCollection::end() const {
	return itemsOrder.end();
}
