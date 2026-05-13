/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.hpp
 * @since     Feb 22, 2023
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

#include "DialogLinkEditor.hpp"
#include "Storage/CollectionHandler.hpp"
#include "Storage/Selection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

using LEDSpicerUI::Ui::Storage::CollectionHandler;
using LEDSpicerUI::Ui::Storage::BoxButtonCollection;

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogSelect
 * Generic link-selection dialog.
 * Configuration is split into two parts:
 * - setRequest()     — called from wireChildrenDialogs() by the owner dialog.
 * - setDestination() — called from wireChildrenDialogs() by the owner dialog.
 * Both must be set before any action method is called.
 */
class DialogSelect : public GladeDialog<DialogSelect> {

	friend class Gtk::Builder;

public:

	/**
	 * Static configuration for one link type.
	 * Owned by the owner dialog for its entire lifetime.
	 */
	struct SelectionRequest {

		/// Box in the parent dialog where selected items are displayed.
		OrdenableFlowBox* displayBox;

		const string&
			/// Field name written into each Link.
			linkKey,
			/// Semantic type label used in XML and Link identity.
			linkType,
			/// Collection ID used by this link type.
			collectionId,
			/// If set will use prop value to filter the collection.
			filterProp;

		/// Used when filter is set to get only a reduced set.
		string filterValue;

		/// Collection to populate the picker from.
		CollectionHandler* sourceCollection;

		/// Fields editable per Link via DialogLinkEdit; empty if none.
		const vector<Storage::Link::LinkField> linkFields;

		/// Minimum number of items that must be selected to enable the Apply button.
		const int minSelection = 1;

	};

	virtual ~DialogSelect();

	/**
	 * Wires the dialog for the next action.
	 * Must be called before open(), refresh(), reindex(), or load().
	 * @param dest Destination collection for this action.
	 * @param req  Static configuration for this link type.
	 */
	void setUp(BoxButtonCollection* dest, const SelectionRequest& req) noexcept {
		destination = dest;
		request     = &req;
	}

	/**
	 * Opens the picker. Pre-selects items already in destination, then
	 * applies a differential save on confirm.
	 */
	void open() noexcept;

	/**
	 * Repopulates displayBox from the current contents of destination.
	 */
	void refresh() noexcept;

	/**
	 * Reorders destination to match the visual order of displayBox.
	 */
	void reindex() noexcept;

	/**
	 * Loads Link items from XML into destination, then refreshes displayBox.
	 * @param values        DataMap carrying the raw data.
	 * @param ownerUniqueId Unique ID of the owning Data.
	 */
	void load(DataMap& values, const string& ownerUniqueId) noexcept;

	/**
	 * @return Vector with indexes of the found BoxButtons.
	 */
	StringVector getSelectedIndexes() const noexcept;

	/**
	 * Selects items by a list of indexes.
	 * @param indexes Vector with indexes of the items to select.
	 */
	void selectByIndexes(const StringVector& indexes) noexcept;

protected:

	/// Confirm button inside the picker dialog.
	Gtk::Button* btnApply = nullptr;

	/// Box inside the picker dialog where all source items are shown.
	Gtk::FlowBox* pickerBox = nullptr;

	DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

private:

	/// Static config set by the owner dialog.
	const SelectionRequest* request = nullptr;

	/// Dynamic destination set by the owner dialog when currentData changes.
	BoxButtonCollection* destination = nullptr;

	/**
	 * Fills pickerBox with Selection widgets for all items in sourceCollection.
	 * Pre-selects items already present in destination.
	 */
	void populatePicker() noexcept;

	/**
	 * Adds DELETE (always) and EDIT (when linkFields non-empty) buttons to a BoxButton.
	 * @param boxButton Target BoxButton in displayBox.
	 */
	void addDisplayButtons(Storage::BoxButton& boxButton) noexcept;
};

} // namespace
