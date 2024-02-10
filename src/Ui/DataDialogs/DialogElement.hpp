/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.h
 * @since     Mar 13, 2023
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
#include "Storage/Element.hpp"

#ifndef UI_DIALOGELEMENT_HPP_
#define UI_DIALOGELEMENT_HPP_ 1

#define elementHandler Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)
#define pinHandler Storage::PinHandler::getInstance()

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogElement
 */
class DialogElement: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogElement() = delete;

	virtual ~DialogElement() = default;

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
	static DialogElement* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

	void deleteElementByPin(const string& pin);

protected:

	/// Self instance.
	static DialogElement* instance;

	Gtk::Entry
		* inputElementName = nullptr,
	// Single color.
		* pin = nullptr,
	// RGB.
		* pinR = nullptr,
		* pinG = nullptr,
		* pinB = nullptr,
	// onTime
		* timeOn = nullptr;

	Gtk::Notebook* notebookDevice;

	Gtk::ToggleButton* solenoid    = nullptr;
	Gtk::Button* inputDefaultColor = nullptr;

	// Only used by the UI
	Gtk::ComboBoxText* inputElementType = nullptr;

	DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	/// Clean up used pins.
	void unmarkMyPins();

	/// Set used pins.
	void markMyPins();
};

} /* namespace */

#endif /* UI_DIALOGELEMENT_HPP_ */
