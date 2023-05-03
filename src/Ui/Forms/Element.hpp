/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
 * @since     Mar 16, 2023
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

#include "../DialogColors.hpp"
#include "CollectionHandler.hpp"
#include "Form.hpp"
#include "PinHandler.hpp"

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_ 1

#define E_TYPE "type"

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::ElementForm
 */
class Element: public Form {

public:

	struct ElementFields {
		Gtk::Entry
			* InputElementName = nullptr,
		// Single color.
			* pin = nullptr,
		// RGB.
			* pinR = nullptr,
			* pinG = nullptr,
			* pinB = nullptr,
		// onTime
			* timeOn = nullptr;
		Gtk::CheckButton* solenoid = nullptr;
		Gtk::Button* InputDefaultColor = nullptr;
		// Only used by the UI
		Gtk::ComboBoxText* InputElementType = nullptr;
	};

	Element(unordered_map<string, string>& data);

	virtual ~Element();

	static void initialize(ElementFields* fields);

	virtual const string createPrettyName() const;

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

protected:

	static PinHandler* pinLayout;

	/// Access to the form fields.
	static ElementFields* fields;

	/// Reference to element handler.
	static CollectionHandler* ElementHandler;

	/// Clean up used pins.
	void unmarkMyPins();

	/// Set used pins.
	void markMyPins();
};

} /* namespace */

#endif /* ELEMENT_HPP_ */
