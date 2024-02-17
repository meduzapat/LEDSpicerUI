/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogForm.hpp
 * @since     Feb 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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
#include "OrdenableFlowBox.hpp"
#include "Storage/BoxButtonCollection.hpp"

#ifndef UI_FORMDIALOG_HPP_
#define UI_FORMDIALOG_HPP_ 1

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogForm
 * To be used with dialogs that have form items.
 * All dialogs are singletons, there is only one of each.
 * Dialogs handle user input but does not stores any data itself.
 * To work they need a Form to store data and create a BoxButton that will be stores into a BoxButtonCollection.
 * The BoxButtonCollection need to be provided before opening the form.
 */
class DialogForm: public Gtk::Dialog {

public:

	/**
	 * Possible states of the form,
	 */
	enum class Modes : uint8_t {
		ADD,  /// Adding new
		LOAD, /// Loading new
		EDIT  /// Editing existing
	};

	DialogForm() = delete;

	virtual ~DialogForm() noexcept;

	/**
	 * Load Function.
	 */
	virtual void load(XMLHelper* values) = 0;

	/**
	 * Reindex the items.
	 */
	void reindex();

	/**
	 * Refresh the box contents, intended to be used when an item is deleted directly without using the delete button.
	 */
	virtual void refreshBox();

	/**
	 * Sets the collection where the items will be stored and if there is a data owner for this dialog.
	 *
	 * @param collection
	 * @param owner
	 */
	virtual void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner = nullptr);

	/**
	 * Reset form.
	 */
	virtual void clearForm() = 0;

	/**
	 * Check if the values on the fields are valid.
	 */
	virtual void isValid() const = 0;

	/**
	 * Send fields into the storage
	 * @param mode
	 */
	virtual void storeData() = 0;

	/**
	 * Bring stored info into fields.
	 */
	virtual void retrieveData() = 0;

	/**
	 * Returns an array with a list of stored key values values.
	 * @return
	 */
	vector<const unordered_map<string, string>*> getValues();

	/**
	 * Create an unique using the dialog fields.
	 * @return
	 */
	virtual const string createUniqueId() const = 0;

protected:

	/// If true the form is in Edit mode.
	Modes mode = Modes::ADD;

	Gtk::Button
		/// Add mew Button.
		* btnAdd = nullptr,
		/// Data save button.
		* btnApply = nullptr;

	/// The box that display the buttons.
	OrdenableFlowBox* box = nullptr;

	/// Created items in the dialog.
	Storage::BoxButtonCollection* items = nullptr;

	/// Current storage form, been created, edited or loaded.
	Storage::Data* currentData = nullptr;

	/// Owner access, only set if this Dialog is part of other storage, also may not been save yet, query the dialog instead for values.
	Storage::Data* owner = nullptr;

	/**
	 * Constructor.
	 * @param obj
	 * @param builder
	 */
	DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	/**
	 * Creates items from raw data.
	 * @param rawCollection
	 */
	void createItems(vector<unordered_map<string, string>>& rawCollection, XMLHelper* values);

	/**
	 * Creates any sub items, called from created items, per item.
	 */
	virtual void createSubItems(XMLHelper* values) {}

	/**
	 * Provides a nice name for the type of data this dialog will create for the dialog.
	 * @return
	 */
	virtual const string getType() const = 0;

	/**
	 * Creates a data object out of raw data.
	 * @param rawData this values will be moved into the class.
	 * @return
	 */
	virtual Storage::Data* getData(unordered_map<string, string>& rawData) = 0;

	/**
	 * Creates an empty object.
	 * @return
	 */
	virtual Storage::Data* getData();

	/**
	 * Method to add generic Add functionality.
	 */
	void setSignalAdd();

	/**
	 * Utility to create and add a button that allows deletion of the BoxButtons.
	 *
	 * @param boxButton the boxButton that will receive this delete button to delete itself.
	 * @param askConfirmation if set will ask, default yes.
	 */
	virtual void createDeleteButton(Storage::BoxButton* boxButton, bool askConfirmation = true);

	/**
	 * Utility to create and add a button that allows to edit the BoxButtons.
	 *
	 * @param boxButton the BoxButton that will receive this edit button to been able to get edited.
	 */
	virtual void createEditButton(Storage::BoxButton* boxButton);

	/**
	 * Function to create the necesary buttons in the boxButton.
	 *
	 * @param boxButton The BoxButton that will get buttons.
	 */
	virtual void addButtons(Storage::BoxButton* boxButton);

	/**
	 * Method to add generic Apply functionality.
	 */
	virtual void setSignalApply();

	/**
	 * When the add button is clicked.
	 */
	virtual void onAddClicked();

	/**
	 * Called after Add, when the data is stored and the button created.
	 * @param boxButton the newly created box button.
	 */
	virtual void afterCreate(Storage::BoxButton* boxButton) {}

	/**
	 * When the edit button is clicked.
	 * @param boxButton the button that called delete.
	 */
	virtual void onEditClicked(Storage::BoxButton* boxButton);

	/**
	 * Called After Edit before the data is stored.
	 * @param boxButton the button that called edit.
	 */
	virtual void afterEdit(Storage::BoxButton* boxButton) {}

	/**
	 * When the delete button is clicked.
	 * @param boxButton the button that called delete.
	 */
	virtual void onDelClicked(Storage::BoxButton* boxButton);

	/**
	 * Called after delete confirmation is accented.
	 * @param boxButton the button that called delete.
	 */
	virtual void afterDeleteConfirmation(Storage::BoxButton* boxButton) {}
};

} /* namespace */

#endif /* UI_FORMDIALOG_HPP_ */
