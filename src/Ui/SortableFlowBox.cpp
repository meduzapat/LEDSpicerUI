/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SortableFlowBox.cpp
 * @since     Apr 10, 2023
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

#include "SortableFlowBox.hpp"

using namespace LEDSpicerUI::Ui;

SortableFlowBox::SortableFlowBox(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder,
	const string& up,
	const string& dn,
	const string& first,
	const string& last
) : SortableFlowBox(obj, builder) {

	Gtk::Button
		* btnUp    = nullptr,
		* btnDn    = nullptr,
		* btnFirst = nullptr,
		* btnLast  = nullptr;

	builder->get_widget(up, btnUp);
	builder->get_widget(dn, btnDn);

	if (not first.empty()) builder->get_widget(first, btnFirst);
	if (not last.empty())  builder->get_widget(last, btnLast);

	signal_selected_children_changed().connect([this, btnUp, btnDn, btnFirst, btnLast]() {
		if (not get_selected_children().size()) {
			btnUp->set_sensitive(false);
			btnDn->set_sensitive(false);
			if (btnFirst) btnFirst->set_sensitive(false);
			if (btnLast)  btnLast->set_sensitive(false);
			return;
		}

		size_t index = get_selected_children().at(0)->get_index();
		btnUp->set_sensitive(index);
		btnDn->set_sensitive(index != getSize() - 1);
		if (btnFirst) btnFirst->set_sensitive(index);
		if (btnLast)  btnLast->set_sensitive(index != getSize() - 1);
	});

	btnUp->signal_clicked().connect([this]() {
		auto selectedChild = get_selected_children().at(0);
		auto index = selectedChild->get_index();
		remove(*selectedChild);
		insert(*selectedChild, index - 1);
		unselect_all();
		select_child(*selectedChild);
		Defaults::markDirty();
	});

	btnDn->signal_clicked().connect([this]() {
		auto selectedChild = get_selected_children().at(0);
		auto index = selectedChild->get_index();
		remove(*selectedChild);
		insert(*selectedChild, index + 1);
		unselect_all();
		select_child(*selectedChild);
		Defaults::markDirty();
	});

	if (btnFirst) {
		btnFirst->signal_clicked().connect([this]() {
			auto selectedChild = get_selected_children().at(0);
			remove(*selectedChild);
			insert(*selectedChild, 0);
			unselect_all();
			select_child(*selectedChild);
			Defaults::markDirty();
		});
	}

	if (btnLast) {
		btnLast->signal_clicked().connect([this]() {
			auto selectedChild = get_selected_children().at(0);
			remove(*selectedChild);
			insert(*selectedChild, -1);
			unselect_all();
			select_child(*selectedChild);
			Defaults::markDirty();
		});
	}
}

size_t SortableFlowBox::getSize() const noexcept {
	return get_children().size();
}

void SortableFlowBox::wipe() {
	for (auto child : get_children())
		remove(*child);
}
