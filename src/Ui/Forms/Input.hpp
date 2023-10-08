/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.hpp
 * @since     Sep 27, 2023
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

#include "Form.hpp"
#include "BoxButtonCollection.hpp"
#include "../DialogInputLinkMaps.hpp"
#include "../DialogInputMap.hpp"

#ifndef INPUT_HPP_
#define INPUT_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::Input
 * Class with Input dialog data.
 */
class Input: public Form {

public:

	struct InputFields {
		Gtk::ComboBoxText* comboBoxInputSelectInput   = nullptr;
		Gtk::Entry*        inputInputName             = nullptr;
		Gtk::ComboBoxText* comboBoxInputSpeed         = nullptr;
		Gtk::Switch*       switchInputBlink           = nullptr;
		Gtk::SpinButton*   spinInputTimes             = nullptr;
		Gtk::Entry*        inputInputDevicesID        = nullptr;
		Gtk::Box*          linkedElementsAndGroupsBox = nullptr;
		Gtk::Button*       btnAddInputMap             = nullptr;
	};

	Input(unordered_map<string, string>& data);

	virtual ~Input();

	static void initialize(InputFields& fields);

	virtual const string createPrettyName() const;

	virtual void resetForm(Modes mode);

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

protected:

	static InputFields inputFields;

	static DialogInputMap* dialogInputMap;

	static DialogInputLinkMaps* dialogInputLinkMaps;

	static CollectionHandler* mapHandler;

	static CollectionHandler* mappingHandler;

	static CollectionHandler* inputNameHandler;

	BoxButtonCollection maps;

	BoxButtonCollection linkedMaps;

	virtual void onActivate();

	virtual void onDeActivate();
};

} /* namespace */

#endif /* INPUT_HPP_ */
