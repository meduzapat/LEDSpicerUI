/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogFormHost.cpp
 * @since     Jun 2025
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

#include "DialogFormHost.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

void DialogFormHost::clearForm() noexcept {
	selectorCombo->set_active(0);
	previousName = "";
}

bool DialogFormHost::handleTypeSwitch(
	const SortableFlowBox* box,
	const string& confirmMsg
) noexcept {

	if (selectorCombo->get_active_row_number() == -1) return false;

	// Selected name.
	string newName {selectorCombo->get_active_id()};

	// Clean up.
	if (newName.empty()) {
		onEmpty();
		return false;
	}

	// Dialog setup.
	if (previousName.empty()) {
		previousName = newName;
		onSelected();
		// Only ADD needs reset!
		return action == Actions::ADD;
	}

	// Refresh.
	if (previousName == newName) return false;

	// Current existing name (if any).
	string currentName(currentData->getPrimaryValue());

	// Original data or replaced.
	if (not currentName.empty()) {
		// ask to avoid losses.
		if (box->getSize() and Message::ask(confirmMsg, this) != Gtk::ResponseType::RESPONSE_YES) {
			selectorCombo->set_active_id(previousName);
			return false;
		}
	}

	// Process conventions.
	onEmpty();
	onSelected();
	// New data or replaced.
	previousName = newName;
	return true;
}

void DialogFormHost::markUsed(
	std::function<bool(const string&)> isAvailable
) noexcept {
	for (auto iter = listStore->children().begin(); iter != listStore->children().end(); ++iter) {
		Gtk::TreeModel::Row row = *iter;
		string id;
		row.get_value(0, id);
		if (not id.empty())
			row.set_value(2, isAvailable(id));
	}
}
