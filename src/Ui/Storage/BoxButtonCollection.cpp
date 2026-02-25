/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.cpp
 * @since     Apr 14, 2023
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

#include "BoxButtonCollection.hpp"

using namespace LEDSpicerUI::Ui::Storage;

BoxButtonCollection::~BoxButtonCollection() {
	for (auto &item : items) delete item;
}

size_t BoxButtonCollection::getSize() const {
	return items.size();
}

bool BoxButtonCollection::isSet(Data *form) const {
	return isIdSet(form->createUniqueId());
}

bool BoxButtonCollection::isIdSet(const string& id) const {
	for (const auto item : items) {
		if (item->getData()->createUniqueId() == id) {
			return true;
		}
	}
	return false;
}

BoxButton& BoxButtonCollection::create(Data* form) {
	BoxButton* ptr = new BoxButton(form);
	items.push_back(ptr);
	return *ptr;
}

void BoxButtonCollection::remove(BoxButton& item) {
	auto it = std::find_if(
		items.begin(),
		items.end(),
		[&item](BoxButton* ptr) {
			return ptr == &item;
		}
	);
	delete *it;
	items.erase(it);
}

void BoxButtonCollection::remove(Data* form) {
	auto it = std::find_if(
		items.begin(),
		items.end(),
		[form](const BoxButton* button) {
			return *button->getData() == *form;
		}
	);
	if (it == items.end()) return;
	delete *it;
	items.erase(it);
}

void BoxButtonCollection::populateBox(OrdenableFlowBox* box) {
	for (auto& item : items) {
		box->add(*item);
	}
	box->show_all();
}

void BoxButtonCollection::reindex(OrdenableFlowBox* box) {
	if (items.empty()) return;
	BoxButtonVector reorderedItems;
	reorderedItems.reserve(items.size());

	for (auto* child : box->get_children()) {
		auto flowChild = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto boxButton = dynamic_cast<BoxButton*>(flowChild->get_child());
		auto it = std::find_if(items.begin(), items.end(), [boxButton](BoxButton* item) {
			return item->getData()->createUniqueId() == boxButton->getData()->createUniqueId();
		});
		reorderedItems.push_back(*it);
	}
	items.swap(reorderedItems);
}

void BoxButtonCollection::wipe() {
	items.clear();
}

BoxButtonVector::iterator BoxButtonCollection::begin() {
	return items.begin();
}

BoxButtonVector::iterator BoxButtonCollection::end() {
	return items.end();
}

BoxButtonVector::const_iterator BoxButtonCollection::begin() const {
	return items.begin();
}

BoxButtonVector::const_iterator BoxButtonCollection::end() const {
	return items.end();
}
