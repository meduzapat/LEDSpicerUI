/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogLinkEdit.hpp
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
 * Describes one extra field on a Link.
 * Stored in SelectionRequest::linkFields and passed to DialogLinkEdit::open().
 * All members are const — descriptors are compile-time constants.
 */
struct LinkField {

	/// Widget type rendered for this field.
	enum class Widget : uint8_t {
		COLOR, /// Color picker via DialogColors.
		COMBO  /// ComboBox backed by a pre-existing glade liststore.
	};

	/// fieldsData key on the Link (e.g. COLOR, FILTER).
	const string key;

	/// Human label displayed next to the widget.
	const string label;

	const Widget widgetType;

	/// Value used when the Link has no stored value yet.
	const string defaultValue;

	/**
	 * Glade liststore ID. COMBO only.
	 * The dialog swaps the combo model to this liststore before showing.
	 * e.g. "liststoreFilter".
	 */
	const string modelId;

};

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogLinkEdit
 *
 * Generic dialog for editing extra fields on a Link.
 * Driven entirely by a vector<LinkField> — shows only the relevant slots,
 * hides the rest. Stateless: all context arrives in open().
 *
 * Slot capacity (current glade): 1 × COLOR, 1 × COMBO.
 * Add more slots to the glade file to expand capacity.
 */
class DialogLinkEdit : public GladeDialog<DialogLinkEdit> {

	friend class Gtk::Builder;

public:

	virtual ~DialogLinkEdit() = default;

	/**
	 * Opens the dialog to edit extra fields on a Link.
	 * Hides all slots first, then shows and pre-populates those described
	 * in fields. On Apply, writes widget values back into link's fieldsData.
	 * Closing the window or any non-Apply response leaves the Link unchanged.
	 *
	 * @param link   Link whose fieldsData will be read and written.
	 * @param fields Descriptors controlling which slots are shown.
	 */
	void open(Storage::Link* link, const vector<LinkField>& fields);

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

	DialogLinkEdit(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

};

} // namespace
