/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogForm.hpp
 * @since     Feb 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include "../ConfigFile.hpp"
#include "DialogColors.hpp"
#include "Forms/BoxButtonCollection.hpp"
#include "OrdenableFlowBox.hpp"

#ifndef UI_FORMDIALOG_HPP_
#define UI_FORMDIALOG_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogForm
 * To be used with dialogs that have form items.
 */
class DialogForm: public Gtk::Dialog {

public:

	DialogForm() = delete;

	DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	virtual ~DialogForm();

	string toXml();

	/**
	 * Creates items from raw data and returns a pointer to the new created items.
	 * @param rawCollection
	 * @param wipe set to true to clean before import.
	 * @returns a pointer to the populated list.
	 */
	Forms::BoxButtonCollection* createItems(vector<unordered_map<string, string>>& rawCollection, bool wipe);

	/**
	 * Creates items from raw data and send them to its owner.
	 * @param rawCollection
	 * @param owner
	 * @param wipe set to true to clean before import,
	 */
	void createItems(vector<unordered_map<string, string>>& rawCollection, Forms::BoxButton* owner, bool wipe);

	/**
	 * Moves the items out.
	 * @param [out]collection
	 */
	void getItems(Forms::BoxButtonCollection& collection);

	/**
	 * Moves items in.
	 * @param [in]collection
	 */
	void setItems(Forms::BoxButtonCollection& collection);

	/**
	 * Remove items and empty the box.
	 */
	virtual void wipeContents();

	/**
	 * Returns an array with a list of stored key values values.
	 * @return
	 */
	vector<const unordered_map<string, string>*> getValues();

protected:

	/// If true the form is in Edit mode.
	Forms::Form::Modes mode = Forms::Form::Modes::ADD;

	Gtk::Button
		/// Add mew Button.
		* btnAdd = nullptr,
		/// Form save button.
		* btnApply = nullptr;

	/// The box that display the buttons.
	OrdenableFlowBox* box = nullptr;

	/// Created items in the dialog.
	Forms::BoxButtonCollection items;

	/// Current storage form.
	Forms::Form* currentForm = nullptr;

	/**
	 * Provides a name for the type of data this form will create.
	 * @return
	 */
	virtual string getType() = 0;

	/**
	 * Creates a form out of raw data.
	 * @param rawData
	 * @return
	 */
	virtual Forms::Form* getForm(unordered_map<string, string>& rawData) = 0;

	/**
	 * Functio to add generic Apply functionality.
	 */
	virtual void setSignalApply();

	/**
	 * Set any necesary signal.
	 * @param button
	 */
	virtual void setSignals(Forms::BoxButton* button);

	/**
	 * When the add button is clicked.
	 */
	virtual void onAddClicked();

	/**
	 * Called after Add, when the data is stored and the button created.
	 * @param boxButton the newly created box button.
	 */
	virtual void afterCreate(Forms::BoxButton* boxButton) {}

	/**
	 * When the edit button is clicked.
	 * @param id the item id that generated the call.
	 */
	virtual void onEditClicked(Forms::BoxButton* boxButton);

	/**
	 * Called After Edit before the data is stored.
	 * @param boxButton the button that called edit.
	 */
	virtual void afterEdit(Forms::BoxButton* boxButton) {}

	/**
	 * When the delete button is clicked.
	 * @param id the item id that generated the call.
	 */
	virtual void onDelClicked(Forms::BoxButton* boxButton);

	/**
	 * Called after delete confimation is acceted.
	 * @param boxButton the button that called delete.
	 */
	virtual void afterDeleteConfirmation(Forms::BoxButton* boxButton) {}
};

} /* namespace */

#endif /* UI_FORMDIALOG_HPP_ */
