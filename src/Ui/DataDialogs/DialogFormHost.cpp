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

void DialogFormHost::switchType() {
	currentData->reset();
	clearForm();
	refreshBox();
}

DialogFormHost::TypeSwitchResult DialogFormHost::handleTypeSwitch(
	Gtk::ComboBox* combo,
	const string&  newName,
	const string&  confirmMsg
) {
	if (newName.empty()) {
		clearForm();
		previousName = "";
		return TypeSwitchResult::Empty;
	}

	if (previousName == newName) return TypeSwitchResult::Unchanged;

	const string currentName(currentData->getValue(NAME));

	if (currentName.empty()) {
		// No committed data yet — switch silently.
		switchType();
	}
	else if (newName != currentName) {
		if (Message::ask(confirmMsg) == Gtk::ResponseType::RESPONSE_YES) {
			switchType();
		}
		else {
			combo->set_active_id(previousName);
			return TypeSwitchResult::Unchanged;
		}
	}

	previousName = newName;
	return TypeSwitchResult::Proceed;
}

void DialogFormHost::markUsed(
	Gtk::ListStore* liststore,
	std::function<bool(const string&)> isAvailable
) {
	for (auto iter = liststore->children().begin(); iter != liststore->children().end(); ++iter) {
		Gtk::TreeModel::Row row = *iter;
		string id;
		row.get_value(0, id);
		if (not id.empty())
			row.set_value(2, isAvailable(id));
	}
}
