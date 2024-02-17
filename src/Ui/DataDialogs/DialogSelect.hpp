/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.hpp
 * @since     Feb 22, 2023
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

#include "DialogForm.hpp"
#include "Storage/CollectionHandler.hpp"
#include "Storage/NameOnly.hpp"

#ifndef DIALOGSELECT_HPP_
#define DIALOGSELECT_HPP_ 1

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogSelect
 * Handles the selection of already created items by other dialogs.
 */
class DialogSelect: public DialogForm {

	friend class Gtk::Builder;

public:

	/**
	 * ButtonFlags Button Flags
	 *
	 * Constants representing flags for different button types.
	 * @{
	 */
	static const uint8_t
		/// Flag indicating a delete button.
		DELETER = 1,
		/// Flag indicating a default color picker button.
		COLORER = 2,
		/// Flag indicating an edit button.
		EDITER  = 4;
	/** @} */

	DialogSelect() = delete;

	virtual ~DialogSelect() = default;

	void RunDialog();

	/**
	 * Instanciate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogSelect* getInstance();


	void load(XMLHelper* values) override;

	void setDestinationSettings(
		OrdenableFlowBox* box,
		const string& type,
		const string& collection,
		uint8_t buttons
	);

	void clearForm() override {};

	void clearFormOthers();

	void isValid() const override;

	void storeData() override {}

	void retrieveData() override {}

	const string createUniqueId() const override {return "";}

	/**
	 * @return The number of selected boxes.
	 */
	const size_t getNumberOfSelections() const;

	/**
	 * @return The numbner of available boxes to select from.
	 */
	const size_t getNumberOfSelectables() const;

protected:

	/// Self instance.
	static DialogSelect* instance;

	/// Box where the selectables are displayed.
	Gtk::FlowBox* boxAll = nullptr;

	/// The current form type.
	string type;

	/// The current Collection name.
	string collection;

	/// List of buttons to assign to the selected buttons.
	uint8_t buttons = 0;

	DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	/**
	 * Creates a color picket button.
	 * @param boxButton
	 */
	void createColorButton(Storage::BoxButton* boxButton);

	void addButtons(Storage::BoxButton* boxButton) override;

	void populateSelectables();

	/**
	 * Checks if name exist in the items.
	 * @param name
	 * @return true if  found.
	 */
	bool exist(const string& name);

};

} /* namespace */

#endif /* DIALOGSELECT_HPP_ */
