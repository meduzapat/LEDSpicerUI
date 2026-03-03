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

#include "DialogColors.hpp"
#include "Storage/Element.hpp"
#include "Storage/CollectionHandler.hpp"
#include "Storage/Selection.hpp"
#include "Storage/Link.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogSelect
 * Handles the selection of already created items by other dialogs.
 * Will allow to select and populate a box with selections from other collections.
 * This dialog differs from the rest because it does not store any data itself,
 * but only allows to move data from one place to another using links.
 * It also allows owners with multiple sources to be used.
 */
class DialogSelect: public GladeDialog<DialogSelect> {

	friend class Gtk::Builder;

public:

	using StringBoxButtonCollectionUMap = std::unordered_map<string, Storage::BoxButtonCollection*>;
	using CollectionHandler = LEDSpicerUI::Ui::Storage::CollectionHandler;

	/// Flag indicating a delete button.
	static constexpr const uint8_t BUTTON_DELETER = 0b00000001;
	/// Flag indicating a default color picker button.
	static constexpr const uint8_t BUTTON_COLORER = 0b00000010;
	/// Flag indicating an edit button.
	static constexpr const uint8_t BUTTON_EDITER  = 0b00000100;

	/**
	 * Holds the rules for the selector.
	 */
	struct SettingRequest {
		/// The box where the selectables are handled.
		OrdenableFlowBox*& workingBox;
		const string
			/// The key that will contain the selection inside link (ex: name).
			parameter,
			/// The type of selection, used later to create the node that will be used in the XML (ex: element).
			type,
			/// The Collection to do lookups for already existing items (ex: elements).
			sourceCollection;
		/// The buttons to add to the selectable items.
		const uint8_t buttons;
	};

	virtual ~DialogSelect() = default;

	/**
	 * Sets the item destinations to be used by the running setting.
	 * @param itemCollections
	 * @param caller
	 */
	void setDestinations(const StringBoxButtonCollectionUMap& itemCollections, const Storage::Data* caller);

	/**
	 * Prepares the dialog to be used.
	 * @param setting
	 */
	void setSettings(const SettingRequest& setting);

	/**
	 * Refresh the destination box.
	 */
	void refresh();

	/**
	 * Sort the items by the box.
	 */
	void reindex();

	/**
	 * Executes the dialog with a set of rules.
	 * @param setting
	 */
	void runSelection();

	void load(XMLHelper* values, const string& identifier);

	/**
	 * @return The number of selected items.
	 */
	size_t getNumberOfSelections() const;

	/**
	 * @return The number of available items to select from.
	 */
	size_t getNumberOfSelectables() const;

	/**
	 * @return The current setting's collection.
	 */
	Storage::CollectionHandler* getCollection() const;

	/**
	 * @return The current item collection based on the settings.
	 */
	Storage::BoxButtonCollection* getItemCollection() const;

protected:

	Gtk::Button
		/// Open this form to create new Data (item), its located in the calling dialog.
		* btnAdd = nullptr,
		/// Store changes.
		* btnApply = nullptr;

	/// Box where the selectables are displayed.
	Gtk::FlowBox* boxAll = nullptr;

	/// Pointer to the current settings.
	const SettingRequest* setting = nullptr;

	/// The data record that called this Dialog.
	const Storage::Data* caller = nullptr;

	/// For storage with multiple items.
	StringBoxButtonCollectionUMap itemCollections;

	DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const;

	Storage::Data* createData(StringUMap& rawData) ;

	/**
	 * Decorates the boxButton with the necessary buttons.
	 * Replaces the default buttons with the ones for this specialized dialog.
	 */
	void addButtons(Storage::BoxButton& boxButton);

	/**
	 * Populates all the items and selects the ones in the group.
	 */
	void populateSelectables();

	/**
	 * Creates a selectable item for the given data and adds it to the box.
	 * @param data
	 */
	void createSelectableItem(Storage::Data* data);
};

} // namespace
