/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogForm.hpp
 * @since     Feb 15, 2023
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

#include "SingletonDialog.hpp"
#include "DialogColors.hpp"
#include "OrdenableFlowBox.hpp"
#include "Storage/CollectionHandler.hpp"
#include "Storage/Parent.hpp"

#pragma once

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
	enum class Actions : uint8_t {
		ADD,  /// Adding new
		LOAD, /// Loading new
		EDIT  /// Editing existing
	};

	DialogForm() = delete;

	virtual ~DialogForm() noexcept;

	/**
	 * Load Function.
	 */
	virtual void load(XMLHelper* values) noexcept abstract;

	/**
	 * Sets the collection where the items will be stored and if there is a data owner for this dialog.
	 *
	 * @param collection
	 * @param owner
	 */
	virtual void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) noexcept;

	/**
	 * Unsets the collection and owner, this will make the form unable to store or retrieve data.
	 */
	void removeOwner() noexcept;

	/**
	 * Clear the From leaving it empty for data entry.
	 */
	virtual void clearForm() noexcept abstract;

	/**
	 * Cleans and re-populates the box with stored items.
	 * Intended to be used when the parent's box needs a refresh.
	 * like when an item is deleted without using the delete button or startup refresh.
	 */
	virtual void refreshItems() noexcept;

	/**
	 * Does extra changes to the form based on current data.
	 */
	virtual void resetForm() noexcept;

	/**
	 * Check if the values on the fields are valid.
	 */
	virtual void isValid() const abstract;

	/**
	 * Send fields into the storage
	 * @param mode
	 */
	virtual void storeData() noexcept abstract;

	/**
	 * Bring stored info into fields.
	 */
	virtual void retrieveData() noexcept abstract;

	/**
	 * Returns an array with a list of stored key values values.
	 * @return
	 */
	vector<const StringUMap*> getValues() const noexcept;

	/**
	 * Create an unique using the dialog fields.
	 * Mimic the createUniqueId in Data, but with fresh data.
	 * While a Dialog is open, Data information must be considered stale.
	 * @return a unique id that identifies the Data inside the form.
	 */
	virtual string createUniqueId() const noexcept abstract;

	/**
	 * Using the box and the items it arrange the items based on the OrdenableFlowBox order.
	 */
	void reindex() noexcept;

	/**
	 * Returns the display box for items.
	 *
	 * @return OrdenableFlowBox pointer.
	 */
	OrdenableFlowBox* getBox() const noexcept { return box; }

	/**
	 * Method to add generic Add functionality that opens a dialog to create new Data (item).
	 * @param btnAdd Opens this form to create new Data (item), its located in the calling dialog.
	 * @param dialogToOpen Dialog to open when the button is clicked, it should be a child of this dialog.
	 */
	static void setSignalAddTo(Gtk::Button* btnAdd, DialogForm* dialogToOpen) noexcept;

protected:

	/// form action mode.
	Actions action = Actions::ADD;

	/// Maps family constants to their child DialogForm.
	static std::unordered_map<string, DialogForm*> familyToDialog;

	/// Store changes.
	Gtk::Button* btnApply = nullptr;

	/// The box that display the item handled by this dialog, its located in the calling dialog.
	OrdenableFlowBox* box = nullptr;

	/// Created items in the dialog.
	Storage::BoxButtonCollection* items = nullptr;

	/// Current item's data, been created, edited or loaded.
	Storage::Data* currentData = nullptr;

	/// The data record that handles this Dialog.
	Storage::Data* ownerData = nullptr;

	/// Child dialogs to refresh when this dialog is refreshed.
	vector<DialogForm*> childDialogs;

	/**
	 * Constructor.
	 * @param obj
	 * @param builder
	 */
	DialogForm(
		BaseObjectType* obj,
		const Glib::RefPtr<Gtk::Builder>& builder
	) noexcept;

	/**
	 * Builds a child dialog singleton and registers it for refresh propagation.
	 * @tparam T the type of the child dialog, it should be a singleton.
	 * @param builder the builder to build the dialog.
	 * @param gladeName the name of the dialog in the glade file.
	 * @param family the family constant of the dialog.
	 *
	 */
	template<typename T>
	void registerChildDialog(
		const Glib::RefPtr<Gtk::Builder>& builder,
		const string& gladeName,
		const string& family
	) noexcept {
		T::buildInstance(builder, gladeName);
		childDialogs.push_back(T::getInstance());
		familyToDialog.emplace(family, T::getInstance());
	}

	/**
	 * Very similar to ADD but it only uses the form to validate data,
	 * also items are not added to the box.
	 */
	void createItems(StringUMapVector& rawCollection, XMLHelper* values) noexcept;

	/**
	 * Creates any sub items, called from created items, per item.
	 * @param values (not used here)
	 */
	virtual void createSubItems(XMLHelper*) noexcept {}

	/**
	 * Provides a nice name for the type of data this dialog will create for the dialog.
	 * @return
	 */
	virtual const string& getType() const noexcept abstract;

	/**
	 * Creates a data object out of raw data.
	 * @param rawData this values will be moved into the class.
	 * @return
	 */
	virtual Storage::Data* createData(StringUMap& rawData) const noexcept abstract;

	/**
	 * Creates an empty object.
	 * @return
	 */
	virtual Storage::Data* createData() const noexcept;

	/**
	 * @return Returns a child primary BoxButton collection.
	 */
	Storage::BoxButtonCollection* getPrimaryChildCollection() const noexcept;

	/**
	 * @return Returns a child BoxButton collection by family.
	 */
	Storage::BoxButtonCollection* getChildCollection(const string& family) const noexcept;

	/**
	 * Prepares the owner's sub dialogs.
	 */
	virtual void wireChildrenDialogs() noexcept;

	/**
	 * Unlink the owner's sub dialogs.
	 */
	virtual void disconnectChildrenDialogs() noexcept;

	/**
	 * Method to add generic Add functionality.
	 * @param btnAdd Opens this form to create new Data (item), its located in the calling dialog.
	 */
	void setSignalAdd(Gtk::Button* btnAdd) noexcept;

	/**
	 * Utility that decorates with a button that allows deletion of itself.
	 *
	 * @param boxButton the boxButton that will receive this delete button to delte itself.
	 * @param askConfirmation if set will ask, default yes.
	 */
	virtual void createDeleteButton(Storage::BoxButton& boxButton, bool askConfirmation = true) noexcept;

	/**
	 * Utility that decorates with a button that allows to edit itself.
	 *
	 * @param boxButton the BoxButton that will receive this edit button to been able to get edited.
	 */
	virtual void createEditButton(Storage::BoxButton& boxButton) noexcept;

	/**
	 * Utility that decorates with a button that allows to clone itself.
	 *
	 * @param boxButton the BoxButton that will receive this clone button to been able to get cloned.
	 */
	void createCloneButton(Storage::BoxButton& boxButton) noexcept;

	/**
	 * Function to decorate the boxButton with the necessary buttons.
	 *
	 * @param boxButton The BoxButton that will get buttons.
	 */
	virtual void addButtons(Storage::BoxButton& boxButton) noexcept;

	/**
	 * Method to add generic Apply functionality.
	 */
	void setSignalApply() noexcept;

	/**
	 * When the add button is clicked.
	 */
	virtual void onAddClicked() noexcept;

	/**
	 * Called after Add, when the data is stored and the button created.
	 * @param boxButton the newly created box button.
	 */
	virtual void afterCreate(Storage::BoxButton&) noexcept {}

	/**
	 * When the edit button is clicked.
	 * @param boxButton the button that called delete.
	 */
	virtual void onEditClicked(Storage::BoxButton& boxButton) noexcept;

	/**
	 * When the delete button is clicked.
	 * @param boxButton the button that called delete.
	 */
	virtual void onDelClicked(Storage::BoxButton& boxButton) noexcept;

	/**
	 * Called after delete confirmation is accented.
	 * @param boxButton the button that called delete.
	 */
	virtual void afterDeleteConfirmation(Storage::BoxButton&) noexcept {}

	/**
	 * When the clone button is clicked.
	 * @param boxButton the button that called clone.
	 */
	virtual void onCloneClicked(Storage::BoxButton& boxButton) noexcept;

};

} // namespace
