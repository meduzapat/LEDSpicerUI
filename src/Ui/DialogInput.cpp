/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDialog.cpp
 * @since     Feb 14, 2023
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

#include "DialogInput.hpp"

using namespace LEDSpicerUI::Ui;

DialogInput::DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Groups Box and button.
	builder->get_widget_derived("BoxInputs", box);
	builder->get_widget("BtnAddInput",       btnAdd);
	builder->get_widget("BtnApplyInput",     btnApply);
	setSignalAdd();
	setSignalApply();

	Forms::Input::InputFields inputFields;

	// Generic widgets.
	builder->get_widget("ComboBoxInputSelectInput", inputFields.comboBoxInputSelectInput);
	builder->get_widget("InputInputName",           inputFields.inputInputName);
	// Actions widgets.
	builder->get_widget("SwitchInputBlink",         inputFields.switchInputBlink);
	// Blinker
	builder->get_widget("SpinInputTimes",           inputFields.spinInputTimes);
	// Actions and Blinker
	builder->get_widget("ComboBoxInputSpeed",       inputFields.comboBoxInputSpeed);
	// Actions, Blinker and Impulse
	builder->get_widget("InputInputDevicesID",      inputFields.inputInputDevicesID);

	// Others
	builder->get_widget("LinkedElementsAndGroupsBox", inputFields.linkedElementsAndGroupsBox);
	builder->get_widget("BtnAddInputMap",             inputFields.btnAddInputMap);

	// Mapping.
	//DialogInputLinkMaps::getInstance()->setMappings(&maps);

	// Signals.
	// When the switch is off disable speed.
	inputFields.switchInputBlink->signal_state_changed().connect([inputFields](bool state) {
		inputFields.comboBoxInputSpeed->set_sensitive(inputFields.switchInputBlink->get_state_flags() & Gtk::StateFlags::STATE_FLAG_CHECKED);
	});

	// When a plugin is selected, set fields rules.
	inputFields.comboBoxInputSelectInput->signal_changed().connect([&, inputFields]() {

		// Hide uncommon Fields
		inputFields.switchInputBlink->get_parent()->hide();
		inputFields.comboBoxInputSpeed->get_parent()->hide();
		inputFields.spinInputTimes->get_parent()->hide();
		inputFields.inputInputDevicesID->get_parent()->hide();
		inputFields.linkedElementsAndGroupsBox->hide();
		inputFields.switchInputBlink->set_state_flags(Gtk::StateFlags::STATE_FLAG_CHECKED, false);
		// Deactivate sections
		if (!inputFields.comboBoxInputSelectInput->get_active_row_number()) {
			btnAdd->set_sensitive(false);
			btnApply->set_sensitive(false);
			inputFields.btnAddInputMap->set_sensitive(false);
		}
		else {
			btnAdd->set_sensitive(true);
			btnApply->set_sensitive(true);
			inputFields.btnAddInputMap->set_sensitive(true);
			if (inputFields.comboBoxInputSelectInput->get_active_text() == "Actions") {
				inputFields.switchInputBlink->set_active(true);
				inputFields.linkedElementsAndGroupsBox->show();
				inputFields.switchInputBlink->get_parent()->show();
				inputFields.comboBoxInputSpeed->get_parent()->show();
			}
			if (inputFields.comboBoxInputSelectInput->get_active_text() == "Blinker") {
				inputFields.spinInputTimes->get_parent()->show();
				inputFields.comboBoxInputSpeed->get_parent()->show();
			}
			if (
				inputFields.comboBoxInputSelectInput->get_active_text() == "Actions" or
				inputFields.comboBoxInputSelectInput->get_active_text() == "Blinker" or
				inputFields.comboBoxInputSelectInput->get_active_text() == "Impulse"
			) {
				inputFields.inputInputDevicesID->get_parent()->show();
			}
		}
	});

	// Send Fields to input.
	Forms::Input::initialize(inputFields);
}
/*
ustring DialogInput::toXml() {
	string r(Defaults::tab() + "<input>\n");
	Defaults::increaseTab();
	mode = Forms::Form::Modes::LOAD;
	for (auto b : items) {
		// load elements
		b->getForm()->retrieveData(mode);

		// unload elements
		b->getForm()->storeData(mode);
	}
	Defaults::reduceTab();
	return (r + Defaults::tab() + "</input>\n");
}
*/
string DialogInput::getType() {
	return "input";
}

Forms::Form* DialogInput::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Input(rawData);
}
