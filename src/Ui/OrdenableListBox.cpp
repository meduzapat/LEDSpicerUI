/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      OrdenableListBox.cpp
 * @since     May 4, 2023
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

#include "OrdenableListBox.hpp"

using namespace LEDSpicerUI::Ui;

OrdenableListBox::OrdenableListBox(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder,
	const string& up,
	const string& dn
) : OrdenableListBox(obj, builder) {
	// Link buttons
	Gtk::Button
		* btnUp = nullptr,
		* btnDn = nullptr;
	builder->get_widget(up, btnUp);
	builder->get_widget(dn, btnDn);
	// Register checkbox
	for (auto child : get_children()) {
		auto boxChild = dynamic_cast<Gtk::ListBoxRow*>(child);
		Defaults::registerWidget(dynamic_cast<Gtk::CheckButton*>(boxChild->get_child()));
	}
	// Set Signals.
	signal_show().connect([=]() {
		btnUp->set_sensitive(false);
		btnDn->set_sensitive(false);
	});

	signal_row_selected().connect([=](Gtk::ListBoxRow* row) {
		if (!row)
			return;
		int index = row->get_index();
		btnUp->set_sensitive(index);
		btnDn->set_sensitive(index != getSize() - 1);
	});

	btnUp->signal_clicked().connect([=]() {
		auto selectedRow = get_selected_row();
		auto index = selectedRow->get_index();
		remove(*selectedRow);
		insert(*selectedRow, index - 1);
		unselect_all();
		select_row(*selectedRow);
		Defaults::markDirty();
	});

	btnDn->signal_clicked().connect([=]() {
		auto selectedRow = get_selected_row();
		auto index = selectedRow->get_index();
		remove(*selectedRow);
		insert(*selectedRow, index + 1);
		unselect_all();
		select_row(*selectedRow);
		Defaults::markDirty();
	});
}

size_t OrdenableListBox::getSize() {
	return get_children().size();
}

void OrdenableListBox::wipe() {
	for (auto child : get_children()) {
		auto boxChild = dynamic_cast<Gtk::ListBoxRow*>(child);
		boxChild->remove();
		remove(*boxChild);
	}
}

void OrdenableListBox::sortAndMark(vector<string> values) {
	unordered_map<string, Gtk::ListBoxRow*> rows;
	// move items into temp container
	for (auto child : get_children()) {
		auto boxChild = dynamic_cast<Gtk::ListBoxRow*>(child);
		rows.emplace(boxChild->get_name(), boxChild);
		remove(*boxChild);
	}
	// Add items selected.
	for (auto& name : values) {
		dynamic_cast<Gtk::CheckButton*>(rows[name]->get_child())->set_active(true);
		add(*rows[name]);
		rows.erase(name);
	}
	// Add items non selected.
	for (auto row : rows) {
		dynamic_cast<Gtk::CheckButton*>(row.second->get_child())->set_active(false);
		add(*row.second);
	}
}

vector<string> OrdenableListBox::getCheckedValues() {
	vector<string> values;
	for (auto child : get_children()) {
		auto boxChild(dynamic_cast<Gtk::ListBoxRow*>(child));
		if (dynamic_cast<Gtk::CheckButton*>(boxChild->get_child())->get_active())
			values.push_back(boxChild->get_name());
	}
	return values;
}
