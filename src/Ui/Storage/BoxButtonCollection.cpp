/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollection.cpp
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

#include "BoxButtonCollection.hpp"

using namespace LEDSpicerUI::Ui::Storage;

BoxButtonCollection::~BoxButtonCollection() {
	wipe();
}

size_t BoxButtonCollection::getSize() const {
	return items.size();
}

bool BoxButtonCollection::isset(const string& name) const {
	for (const auto& i : items) {
		if (searchType.absolute) {
			if (i.getData()->getValue(key) == name)
				return true;
		}
		else {
			if (i.getData()->getValue(key).find(Defaults::addUnitSeparator(name)) != string::npos)
				return true;
		}
	}
	return false;
}

BoxButton& BoxButtonCollection::add(Data* form) {
	items.emplace_back(BoxButton(form));
	return items.back();
}

void BoxButtonCollection::remove(BoxButton& item) {
	items.remove_if([&item](const BoxButton& b) {
		return &b == &item;
	});
}

void BoxButtonCollection::remove(const string& name) {
	items.remove_if([this, &name](const auto& b) {
		if (searchType.absolute) {
			return b.getData()->getValue(key) == name;
		}
		return b.getData()->getValue(key).find(Defaults::addUnitSeparator(name)) != string::npos;
	});
}

void BoxButtonCollection::rename(const string& name, const string& newName) {
	if (name == newName) return;
	for (auto& b : items) {
		auto data = b.getData();
		if (searchType.absolute) {
			const string currentValue(data->getValue(key));
			if (currentValue == name) {
				data->setValue(key, newName);
				b.updateLabel();
				if (searchType.unique) break;
			}
		}
		else {
			string currentValue = data->getValue(key);
			size_t pos = 0;
			string search = Defaults::addUnitSeparator(name);
			while ((pos = currentValue.find(search, pos)) != string::npos) {
				currentValue.replace(pos, search.length(), newName);
				pos += newName.length();
				if (searchType.unique) break;
			}
			data->setValue(key, currentValue);
		}
	}
}

void BoxButtonCollection::populateBox(OrdenableFlowBox* box) {
	for (auto& i : items)
		box->add(i);
	box->show_all();
}

void BoxButtonCollection::reindex(OrdenableFlowBox* box) {
	std::list<BoxButton> sortedItems;
	for (auto* child : box->get_children()) {
		auto flowChild = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto boxButton = dynamic_cast<BoxButton*>(flowChild->get_child());
		auto it = std::find_if(
			items.begin(),
			items.end(),
			[boxButton](BoxButton& b) {
				return &b == boxButton;
			}
		);
		if (it != items.end()) {
			sortedItems.splice(sortedItems.end(), items, it);
		}
	}
	items.swap(sortedItems);
}

void BoxButtonCollection::wipe() {
	items.clear();
}

BoxButton& BoxButtonCollection::at(uint position) {
	if (position >= items.size()) {
		throw std::out_of_range("BoxButtonCollection::at: index out of range");
	}
	auto it = items.begin();
	std::advance(it, position);
	return *it;
}

std::list<BoxButton>::iterator BoxButtonCollection::begin() {
	return items.begin();
}

std::list<BoxButton>::iterator BoxButtonCollection::end() {
	return items.end();
}

std::list<BoxButton>::const_iterator BoxButtonCollection::begin() const {
	return items.begin();
}

std::list<BoxButton>::const_iterator BoxButtonCollection::end() const {
	return items.end();
}
