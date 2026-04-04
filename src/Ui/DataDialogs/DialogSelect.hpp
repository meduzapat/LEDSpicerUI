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

#include "DialogLinkEdit.hpp"
#include "Storage/CollectionHandler.hpp"
#include "Storage/Selection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

using LEDSpicerUI::Ui::Storage::CollectionHandler;
using LEDSpicerUI::Ui::Storage::BoxButtonCollection;

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogSelect
 *
 * Generic stateless link-selection dialog.
 * Each call is fully self-contained — all context is supplied via SelectionRequest.
 * No state is retained between calls.
 *
 * Usage:
 *   - open()    — lets the user pick from a collection; applies differentially on confirm.
 *   - refresh() — repopulates the parent displayBox from the current destination state.
 *   - reindex() — reorders destination to match the visual order of displayBox.
 *   - load()    — populates destination from XML without user interaction.
 *
 * Buttons per item in displayBox:
 *   - DELETE is always present.
 *   - EDIT is added when linkFields is non-empty; opens DialogLinkEdit.
 */
class DialogSelect : public GladeDialog<DialogSelect> {

	friend class Gtk::Builder;

public:

	/**
	 * All context needed for one selection operation.
	 * Construct at the call site — the dialog holds no reference after
	 * open() / refresh() / reindex() / load() returns.
	 */
	struct SelectionRequest {

		/// Box in the parent dialog where selected items are displayed.
		OrdenableFlowBox*    displayBox;

		/// Destination collection that owns the resulting Link objects.
		BoxButtonCollection* destination;

		/// Field name written into each Link (e.g. NAME).
		const string         linkKey;

		/// Semantic type label used in XML and Link identity (e.g. "element").
		const string         linkType;

		/// CollectionHandler key to source items from (e.g. COLLECTION_ELEMENT).
		const string         sourceCollectionId;

		/**
		 * Extra fields editable per Link via DialogLinkEdit.
		 * Empty  → DELETE button only.
		 * Non-empty → EDIT + DELETE; EDIT opens DialogLinkEdit with these descriptors.
		 */
		const vector<LinkField> linkFields;

		/**
		 * Optional expander: given a Data*, returns zero or more Data* to show
		 * in the picker instead of the item itself.
		 * Use for strip elements that expand into individual pins.
		 * nullptr = no expansion.
		 */
		std::function<vector<Storage::Data*>(Storage::Data*)> expander;

	};

	virtual ~DialogSelect() = default;

	/**
	 * Opens the picker for one selection set.
	 * Pre-selects items already in destination, then applies a differential
	 * save on confirm: removes deselected Links, adds newly selected ones.
	 * Closing the window leaves destination unchanged.
	 *
	 * @param request   Full context for this selection.
	 * @param ownerData Data that owns the destination collection.
	 */
	void open(const SelectionRequest& request, const Storage::Data* ownerData);

	/**
	 * Repopulates displayBox from the current contents of destination.
	 * Call from clearForm() and retrieveData() in the parent dialog.
	 *
	 * @param request Must supply displayBox and destination.
	 */
	void refresh(const SelectionRequest& request);

	/**
	 * Reorders destination to match the visual order of displayBox.
	 *
	 * @param request Must supply displayBox and destination.
	 */
	void reindex(const SelectionRequest& request);

	/**
	 * Loads Link items from XML into destination, then refreshes displayBox.
	 * Errors are accumulated and displayed as a batch.
	 *
	 * @param values        XMLHelper carrying the raw data.
	 * @param ownerUniqueId Unique ID of the owning Data (keys into XMLHelper).
	 * @param request       Full context for this load.
	 */
	void load(
		XMLHelper*              values,
		const string&           ownerUniqueId,
		const SelectionRequest& request
	);

protected:

	/// Confirm button inside the picker dialog.
	Gtk::Button* btnApply = nullptr;

	/// Box inside the picker dialog where all source items are shown.
	Gtk::FlowBox* pickerBox = nullptr;

	DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

private:

	/**
	 * Fills pickerBox with Selection widgets for all items in sourceCollectionId.
	 * Pre-selects items already present in destination.
	 */
	void populatePicker(const SelectionRequest& request);

	/**
	 * Creates one Selection widget inside pickerBox for data.
	 *
	 * @param data        Item to make selectable.
	 * @param destination Used to determine initial selection state.
	 */
	void createPickerItem(Storage::Data* data, BoxButtonCollection* destination);

	/**
	 * Adds DELETE (always) and EDIT (when linkFields non-empty) to a BoxButton.
	 *
	 * @param boxButton Target BoxButton in displayBox.
	 * @param request   Supplies linkFields, displayBox, and destination.
	 */
	void addDisplayButtons(Storage::BoxButton& boxButton, const SelectionRequest& request);

	/**
	 * Creates a Link for target, adds it to destination, adds its BoxButton
	 * to displayBox with the appropriate buttons.
	 */
	void addLink(Storage::Data* target, const SelectionRequest& request);

};

} // namespace
