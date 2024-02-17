/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.cpp
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

#include "BoxButtonCollection.hpp"

using namespace LEDSpicerUI::Ui::Storage;

BoxButtonCollection::~BoxButtonCollection() {
	wipe();
}

const size_t BoxButtonCollection::getSize() const {
	return items.size();
}

bool BoxButtonCollection::isset(const string& name) const {
	for (auto i : items) {
		if (searchType.absolute) {
			if (i->getData()->getValue(key) == name)
				return true;
		}
		else {
			if (i->getData()->getValue(key).find(Defaults::addUnitSeparator(name)) != string::npos)
				return true;
		}
	}
	return false;
}

BoxButton* BoxButtonCollection::add(Data* form) {
	// Create the button.
	BoxButton* b = new BoxButton(form);
	items.push_back(b);
	itemsOrder.push_back(b);
	return b;
}

void BoxButtonCollection::remove(BoxButton* item) {
	items.erase(std::remove(items.begin(), items.end(), item), items.end());
	itemsOrder.erase(std::remove(itemsOrder.begin(), itemsOrder.end(), item), itemsOrder.end());
	delete item;
}

void BoxButtonCollection::remove(const string& name) {
	vector<BoxButton*> itemsToRemove;
	for (auto b : items) {
		if (searchType.absolute) {
			if (b->getData()->getValue(key) == name) {
				itemsToRemove.push_back(b);
				// Only one occurrence.
				if (searchType.unique)
					break;
			}
		}
		else {
			if (b->getData()->getValue(key).find(Defaults::addUnitSeparator(name)) != string::npos) {
				itemsToRemove.push_back(b);
				// Only one occurrence.
				if (searchType.unique)
					break;
			}
		}
	}
	// Remove the items after.
	for (auto item : itemsToRemove) {
		remove(item);
	}
}

void BoxButtonCollection::rename(const string& name, const string& newName) {
	for (auto b : items) {
		auto data(b->getData());
		if (searchType.absolute) {
			if (data->getValue(key) == name) {
				data->setValue(key, newName);
				b->updateLabel();
				// Only one occurrence.
				if (searchType.unique)
					break;
			}
		}
		else {
			// Can be many occurrences.
			string
				enclosedName(Defaults::addUnitSeparator(name)),
				replaced(data->getValue(key));

			// find and replace all occurrences
			size_t pos = replaced.find(enclosedName);
			while (pos != string::npos) {
				replaced.replace(pos, enclosedName.length(), newName);
				// Update pos to the position immediately after the replaced substring
				pos = replaced.find(enclosedName, pos + newName.length());
			}
			data->setValue(key, replaced);
			if (searchType.unique)
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
		auto b = dynamic_cast<Storage::BoxButton*>(c->get_child());
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

BoxButton* BoxButtonCollection::at(uint position) {
	return itemsOrder.at(position);
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
