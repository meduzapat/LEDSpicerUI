/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.hpp
 * @since     Feb 14, 2023
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
#include "Storage/Input.hpp"

#ifndef UI_DIALOGINPUT_HPP_
#define UI_DIALOGINPUT_HPP_ 1

#define inputHandler LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_INPUT)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogInput
 * Dialog to create and edit Inputs.
 */
class DialogInput: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogInput() = delete;

	virtual ~DialogInput() = default;

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
	static DialogInput* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogInput* instance;

	Gtk::ComboBoxText* comboBoxInputSelectInput   = nullptr;
	Gtk::Entry*        inputInputName             = nullptr;
	Gtk::ComboBoxText* comboBoxInputSpeed         = nullptr;
	Gtk::Switch*       switchInputBlink           = nullptr;
	Gtk::SpinButton*   spinInputTimes             = nullptr;
	Gtk::Entry*        inputInputDevicesID        = nullptr;
	Gtk::Box*          linkedElementsAndGroupsBox = nullptr;
	Gtk::Button*       btnAddInputMap             = nullptr;

	/// The box to select mappings.
	OrdenableFlowBox
		* boxInputMap = nullptr,
		* boxInputLinkedMaps = nullptr;

	DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	void clearFormOthers();

};

} /* namespace */

#endif /* UI_DIALOGINPUT_HPP_ */
