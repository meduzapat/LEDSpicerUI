/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogFormHost.hpp
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

#include "DialogForm.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogFormHost
 *
 * Intermediate base for dialogs whose Data objects own child collections and
 * whose UI is driven by a primary type-selector combo (device type, restrictor
 * type, input plugin type, etc.).
 *
 * Extracts three responsibilities shared by all such dialogs:
 *   - switchType()        : wipe + rebuild after a type change.
 *   - handleTypeSwitch()  : the full decision tree around a type-selector combo.
 *   - markUsed()          : generic liststore availability updater.
 */
class DialogFormHost : public DialogForm {

public:

	void clearForm() noexcept override;

protected:

	/// Last active type-selector value; prevents spurious signal re-fires.
	string previousName;

	Gtk::ComboBox* selectorCombo = nullptr;
	Gtk::ListStore* listStore    = nullptr;

	using DialogForm::DialogForm;

	/**
	 * Called when the type need to be empty.
	 */
	virtual void onEmpty() noexcept abstract;

	/**
	 * Called when the type was selected but before resetFrom.
	 */
	virtual void onSelected() noexcept abstract;

	/**
	 * Verifies and calculate the selection and reacts.
	 *
	 * If the selection changed to empty, onEmpty is called.
	 *
	 * @param box         The box of objects to check if not empty.
	 * @param confirmMsg  Shown to the user when switching away from saved data.
	 * @return false if no changes or empty, true if the selection changed.
	 */
	bool handleTypeSwitch(
		const OrdenableFlowBox* box,
		const string& confirmMsg
	) noexcept;

	/**
	 * Marks rows in a liststore as available or unavailable.
	 * Assumes column 0 holds the row id key and column 2 holds the availability bool.
	 *
	 * @param liststore   The liststore to update.
	 * @param isAvailable Predicate: receives the row id string, returns true if available.
	 */
	void markUsed(std::function<bool(const string&)> isAvailable) noexcept;

	template<typename TMap>
	void initializeSelector(
		string_view emptyMsg,
		const TMap& infoMap
	) noexcept {
		static_assert(
			std::is_base_of_v<Defaults::BaseInfo, typename TMap::mapped_type>,
			"TMap value type must derive from Defaults::BaseInfo"
		);
		if (not listStore)
			listStore   = static_cast<Gtk::ListStore*>(selectorCombo->get_model().get());
		auto row = *(listStore->append());
		row.set_value(0, string());
		row.set_value(1, string(emptyMsg));
		row.set_value(2, false);
		for (const auto& [id, info] : infoMap) {
			row = *(listStore->append());
			row.set_value(0, id);
			row.set_value(1, string(info.name));
			row.set_value(2, true);
		}
		selectorCombo->set_active(-1);
	}
};

} // namespace
