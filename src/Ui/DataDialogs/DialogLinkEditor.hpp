/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogLinkEditor.hpp
 * @since     Apr 2026
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
#include "Storage/Link.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogLinkEditor
 *
 * Generic dialog for editing extra fields on a Link.
 */
class DialogLinkEditor : public GladeDialog<DialogLinkEditor> {

	friend class Gtk::Builder;

public:

	virtual ~DialogLinkEditor() = default;

	/**
	 * Opens the dialog to edit extra fields on a Link.
	 * @param link   Link whose fieldsData will be read and written.
	 */
	void open(Storage::Link* link) noexcept;

protected:

	Gtk::Button* btnApply = nullptr;

	Gtk::Box
		* boxColor = nullptr,
		* boxCombo = nullptr;

	Gtk::Label
		* labelColor = nullptr,
		* labelCombo = nullptr;

	Gtk::Button*   btnColor    = nullptr;
	Gtk::ComboBox* comboFilter = nullptr;

	DialogLinkEditor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

};

} // namespace
