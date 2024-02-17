/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDialog.cpp
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

#include "DialogInput.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInput* DialogInput::instance = nullptr;

void DialogInput::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogInput", instance);
	}
}

DialogInput* DialogInput::getInstance() {
	return instance;
}

DialogInput::DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Groups Box and button.
	builder->get_widget_derived("BoxInputs", box);
	builder->get_widget("BtnAddInput",       btnAdd);
	builder->get_widget("BtnApplyInput",     btnApply);
	setSignalAdd();
	setSignalApply();

	// Generic.
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput);
	builder->get_widget("InputInputName",           inputInputName);

	// Actions.
	builder->get_widget("SwitchInputBlink",         switchInputBlink);

	// Necessary to select mappings.
	builder->get_widget_derived("BoxInputMap",        boxInputMap);
	builder->get_widget_derived("BoxInputLinkedMaps", boxInputLinkedMaps);

	// Blinker
	builder->get_widget("SpinInputTimes",           spinInputTimes);

	// Actions and Blinker
	builder->get_widget("ComboBoxInputSpeed",       comboBoxInputSpeed);

	// Actions, Blinker and Impulse
	builder->get_widget("InputInputDevicesID",      inputInputDevicesID);

	// Others
	builder->get_widget("LinkedElementsAndGroupsBox", linkedElementsAndGroupsBox);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);

	// Signals.
	// When the Blink switch is off disable speed.
	switchInputBlink->signal_state_changed().connect([&](bool state) {
		comboBoxInputSpeed->set_sensitive(switchInputBlink->get_state_flags() & Gtk::StateFlags::STATE_FLAG_CHECKED);
	});

	// When a plugin is selected, set fields rules.
	comboBoxInputSelectInput->signal_changed().connect([&]() {

		string name(comboBoxInputSelectInput->get_active_text());
		// Deactivate sections
		if (name == "Select Plugin") {
			btnApply->set_sensitive(false);
			btnAddInputMap->set_sensitive(false);
			return;
		}
		clearFormOthers();
		btnApply->set_sensitive(true);
		btnAddInputMap->set_sensitive(true);
		if (name == "Actions") {
			switchInputBlink->set_active(true);
			linkedElementsAndGroupsBox->show();
			switchInputBlink->get_parent()->show();
			comboBoxInputSpeed->get_parent()->show();
			boxInputMap->set_selection_mode(Gtk::SelectionMode::SELECTION_MULTIPLE);
		}
		if (name == "Blinker") {
			spinInputTimes->get_parent()->show();
			comboBoxInputSpeed->get_parent()->show();
		}
		if (
			name == "Actions" or
			name == "Blinker" or
			name == "Impulse"
		) {
			inputInputDevicesID->get_parent()->show();
		}
	});
}

void DialogInput::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_INPUT), values);
}

void DialogInput::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
	DialogInputLinkMaps::getInstance()->load(values);
}

void DialogInput::clearForm() {
	comboBoxInputSelectInput->set_active_text("Select Plugin");
	clearFormOthers();
}

void DialogInput::clearFormOthers() {

	boxInputMap->set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);

	linkedElementsAndGroupsBox->hide();
	inputInputName->set_text("");

	comboBoxInputSpeed->get_parent()->hide();
	comboBoxInputSpeed->set_active_text("Normal");

	spinInputTimes->get_parent()->hide();
	spinInputTimes->set_text("");

	inputInputDevicesID->get_parent()->hide();
	inputInputDevicesID->set_text("");

	switchInputBlink->get_parent()->hide();
	switchInputBlink->set_state_flags(Gtk::StateFlags::STATE_FLAG_CHECKED, false);
}

void DialogInput::isValid() const {

	string filename(createUniqueId());
	if (filename.empty()) {
		if (mode != Modes::LOAD)
			inputInputName->grab_focus();
		throw Message("Invalid name.");
	}
	// Check if is used.
	if (inputHandler->isUsed(filename)) {
		// If editing and they are the same is OK.
		if (mode != Modes::EDIT or filename != currentData->createUniqueId()) {
			if (mode != Modes::LOAD)
				inputInputName->grab_focus();
			throw Message("Name already in use.");
		}
	}

	if (comboBoxInputSelectInput->get_active_text() == "Blinker") {
		// Check if times is numeric?
		string blinks(spinInputTimes->get_text());
		if (not blinks.empty() and not Defaults::isNumber(blinks)) {
			if (mode != Modes::LOAD)
				spinInputTimes->grab_focus();
			throw Message("Enter a valid number for blink times.");
		}
	}
	if (
		comboBoxInputSelectInput->get_active_text() == "Actions" or
		comboBoxInputSelectInput->get_active_text() == "Blinker" or
		comboBoxInputSelectInput->get_active_text() == "Impulse"
	) {
		// Check if input device id is not empty.
		if (inputInputDevicesID->get_text().empty()) {
			if (mode != Modes::LOAD)
				inputInputDevicesID->grab_focus();
			throw Message("At least one device ID need to be specified.");
		}
	}
}

void DialogInput::storeData() {

	string name(comboBoxInputSelectInput->get_active_text());
	if (mode == Modes::EDIT)
		inputHandler->replace(currentData->createUniqueId(), createUniqueId());
	else
		inputHandler->add(createUniqueId());

	// This will clean any anomaly.
	currentData->wipe();
	currentData->setValue(FILENAME, inputInputName->get_text());
	currentData->setValue(NAME, name);

	if (name == "Actions") {
		//fieldsData["blink"] = switchInputBlink->get_state_flags() & Gtk::StateFlags::STATE_FLAG_CHECKED ? "true" : "false";
		currentData->setValue("blink", switchInputBlink->get_state() ? "true" : "false");
	}

	if (name == "Blinker") {
		currentData->setValue("times", spinInputTimes->get_text());
	}

	if (name == "Actions" or name == "Blinker") {
		currentData->setValue("speed", comboBoxInputSpeed->get_active_text());
	}

	if (name == "Actions" or name == "Blinker" or name == "Impulse") {
		currentData->setValue("listenEvents", inputInputDevicesID->get_text());
	}
}

void DialogInput::retrieveData() {
	string name(currentData->getValue(NAME));
	comboBoxInputSelectInput->set_active_text(name);
	inputInputName->set_text(currentData->getValue(FILENAME));

	if (name == "Actions") {
//		switchInputBlink->set_state_flags(Gtk::StateFlags::STATE_FLAG_CHECKED, fieldsData["blink"] != "true");
		switchInputBlink->set_active(currentData->getValue("blink") == "true");
	}

	if (name == "Blinker") {
		spinInputTimes->set_text(currentData->getValue("times"));
	}

	if (name == "Actions" or name == "Blinker") {
		comboBoxInputSpeed->set_active_text(currentData->getValue("speed"));
	}

	if (name == "Actions" or name == "Blinker" or name == "Impulse") {
		inputInputDevicesID->set_text(currentData->getValue("listenEvents"));
	}
}

string const DialogInput::createUniqueId() const {
	return Defaults::createCommonUniqueId({inputInputName->get_text()});
}

const string DialogInput::getType() const {
	return "input";
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::getData(unordered_map<string, string>& rawData) {
	return new Storage::Input(rawData);
}

