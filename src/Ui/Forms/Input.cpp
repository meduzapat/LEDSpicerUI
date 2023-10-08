/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.cpp
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

#include "Input.hpp"

using namespace LEDSpicerUI::Ui::Forms;

Input::InputFields Input::inputFields;
CollectionHandler* Input::mapHandler       = nullptr;
CollectionHandler* Input::mappingHandler   = nullptr;
CollectionHandler* Input::inputNameHandler = nullptr;

Input::Input(unordered_map<string, string>& data) : Form(data), maps(FILENAME) {
	mapHandler->registerDestination(&maps);
	mappingHandler->registerDestination(&linkedMaps);
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
}

Input::~Input() {
	mapHandler->release(&maps);
	mappingHandler->release(&linkedMaps);
}

void Input::initialize(InputFields& fields) {
	inputFields      = std::move(fields);
	mapHandler       = CollectionHandler::getInstance("maps");
	mappingHandler   = CollectionHandler::getInstance("mappings");
	inputNameHandler = CollectionHandler::getInstance("inputFilename");
}

string const Input::createPrettyName() const {
	return fieldsData.at(NAME) + " " + fieldsData.at(FILENAME);
}

void Input::resetForm(Modes mode) {
	inputFields.comboBoxInputSelectInput->set_active(0);
	inputFields.inputInputName->set_text("");

	inputFields.comboBoxInputSpeed->set_active_text("Normal");

	inputFields.spinInputTimes->set_text("");
	inputFields.inputInputDevicesID->set_text("");
}

void Input::isValid(Modes mode) {

	string filename(inputFields.inputInputName->get_text());
	if (filename.empty()) {
		if (mode != Modes::LOAD)
			inputFields.inputInputName->grab_focus();
		throw Message("Invalid name.\n");
	}
	if (inputNameHandler->count(filename)) {
		if (mode != Modes::EDIT or (mode == Modes::EDIT and filename != fieldsData[FILENAME])) {
			if (mode != Modes::LOAD)
				inputFields.inputInputName->grab_focus();
			throw Message("Name already in use.\n");
		}
	}

	if (inputFields.comboBoxInputSelectInput->get_active_text() == "Blinker") {
		// Check if times is numeric?
		string blinks(inputFields.spinInputTimes->get_text());
		if (not blinks.empty() and not Defaults::isNumber(blinks)) {
			if (mode != Modes::LOAD)
				inputFields.spinInputTimes->grab_focus();
			throw Message("At least one device ID need to be specified.\n");
		}
	}
	if (
		inputFields.comboBoxInputSelectInput->get_active_text() == "Actions" or
		inputFields.comboBoxInputSelectInput->get_active_text() == "Blinker" or
		inputFields.comboBoxInputSelectInput->get_active_text() == "Impulse"
	) {
		// Check if input device id is not empty.
		if (inputFields.inputInputDevicesID->get_text().empty()) {
			if (mode != Modes::LOAD)
				inputFields.inputInputDevicesID->grab_focus();
			throw Message("At least one device ID need to be specified.\n");
		}
	}
}

void Input::storeData(Modes mode) {
	string
		filename(inputFields.inputInputName->get_text()),
		name(inputFields.comboBoxInputSelectInput->get_active_text());
	if (mode == Modes::EDIT)
		inputNameHandler->replace(fieldsData.at(FILENAME), filename);
	else
		inputNameHandler->add(filename);

	// This will clean any anomaly.
	fieldsData.clear();
	fieldsData[FILENAME] = filename;
	fieldsData[NAME]     = name;

	if (name == "Actions") {
		//fieldsData["blink"] = inputFields.switchInputBlink->get_state_flags() & Gtk::StateFlags::STATE_FLAG_CHECKED ? "true" : "false";
		fieldsData["blink"] = inputFields.switchInputBlink->get_state() ? "true" : "false";
	}

	if (name == "Blinker") {
		fieldsData["times"] = inputFields.spinInputTimes->get_text();
	}

	if (name == "Actions" or name == "Blinker") {
		fieldsData["speed"] = inputFields.comboBoxInputSpeed->get_active_text();
	}

	if (name == "Actions" or name == "Blinker" or name == "Impulse") {
		fieldsData["listenEvents"] = inputFields.inputInputDevicesID->get_text();
	}

	// Save elements.
	onDeActivate();
}

void Input::retrieveData(Modes mode) {
	string name(fieldsData[NAME]);
	inputFields.inputInputName->set_text(fieldsData[FILENAME]);
	inputFields.comboBoxInputSelectInput->set_active_text(name);

	if (fieldsData[NAME] == "Actions") {
//		inputFields.switchInputBlink->set_state_flags(Gtk::StateFlags::STATE_FLAG_CHECKED, fieldsData["blink"] != "true");
		inputFields.switchInputBlink->set_active(fieldsData["blink"] == "true");
	}

	if (fieldsData[NAME] == "Blinker") {
		inputFields.spinInputTimes->set_text(fieldsData["times"]);
	}

	if (name == "Actions" or name == "Blinker") {
		inputFields.comboBoxInputSpeed->set_active_text(fieldsData["speed"]);
	}

	if (name == "Actions" or name == "Blinker" or name == "Impulse") {
		inputFields.inputInputDevicesID->set_text(fieldsData["listenEvents"]);
	}

	onActivate();
}

void Input::cancelData(Modes mode) {
	onDeActivate();
}

const string Input::getCssClass() const {
	return "InputBoxButton";
}

void Input::onDeActivate() {
	DialogInputMap::getInstance()->getItems(maps);
	DialogInputLinkMaps::getInstance()->getItems(linkedMaps);
}

void Input::onActivate() {
	DialogInputMap::getInstance()->setItems(maps);
	DialogInputLinkMaps::getInstance()->getItems(linkedMaps);
}

