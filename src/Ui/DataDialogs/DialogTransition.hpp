/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogTransition.hpp
 * @since     Jun 2026
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

#include "DialogColors.hpp"
#include "Storage/Transition.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogTransition
 *
 * Edits the single per-profile Transition record in place.
 */
class DialogTransition : public GladeDialog<DialogTransition> {

	friend class Gtk::Builder;

public:

	virtual ~DialogTransition() = default;

	/**
	 * Opens the dialog to edit the given Transition.
	 * @param transition Transition to edit. Must outlive the call.
	 */
	void edit(Storage::Transition* transition) noexcept;

protected:

	Gtk::ComboBox*  comboName  = nullptr;
	Gtk::ComboBox*  comboSpeed = nullptr;
	Gtk::Button*    btnColor   = nullptr;
	Gtk::Box*       boxSpeed   = nullptr;
	Gtk::Box*       boxColor   = nullptr;
	Gtk::Label*     brief      = nullptr;
	Gtk::Button*    btnApply   = nullptr;
	Gtk::ListStore* listStore  = nullptr;

	/// Live Transition pointer for the duration of edit(). null otherwise.
	Storage::Transition* currentData = nullptr;

	DialogTransition(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/// Fills the type selector liststore from Defaults::transitionsInfo.
	void populateTypes() noexcept;

	/// Widgets <- currentData.
	void retrieveData() noexcept;

	/// Widgets -> currentData. May clear all fields when "None" is selected.
	void storeData() noexcept;

	/// Show/hide speed and color rows based on the active type's flags.
	void updateVisibility() noexcept;

	/// Throws Message if the active selection is incomplete.
	void isValid() const;

};

} // namespace
