/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMap.cpp
 * @since     Sep 30, 2023
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

#include "InputMap.hpp"

using namespace LEDSpicerUI::Ui::Forms;

InputMap::InputMapFields InputMap::fields;
CollectionHandler* InputMap::groupHandler        = nullptr;
CollectionHandler* InputMap::elementHandler      = nullptr;
CollectionHandler* InputMap::inputTriggerHandler = nullptr;

InputMap::InputMap(unordered_map<string, string>& data) :
Form(data) {

	// Check load.
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
}

InputMap::~InputMap() {

}

void InputMap::initialize(InputMapFields& fields) {
	InputMap::fields    = std::move(fields);
	groupHandler        = CollectionHandler::getInstance("groups");
	elementHandler      = CollectionHandler::getInstance("elements");
	inputTriggerHandler = CollectionHandler::getInstance("inputTriggers");
}

void InputMap::resetForm(Modes mode) {
	CollectionHandler::getInstance("elements")->refreshComboBox(fields.comboBoxInputMapElement);
	CollectionHandler::getInstance("groups")->refreshComboBox(fields.comboBoxInputMapGroup);
	fields.comboBoxInputMapElement->set_active(-1);
	fields.comboBoxInputMapGroup->set_active(-1);
	fields.inputInputMapTrigger->set_text("");
	fields.comboBoxInputMapFilter->set_active_text("Normal");
	DialogColors::getInstance()->colorizeButton(fields.inputMapDefaultColor, NO_COLOR);
}

void InputMap::isValid(Modes mode) {
	if (fields.stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		if (fields.comboBoxInputMapElement->get_active_text().empty())
			throw Message("Enter a valid element name.\n");
	}
	else {
		if (fields.comboBoxInputMapGroup->get_active_text().empty())
			throw Message("Enter a valid group name.\n");
	}

	string trigger(fields.inputInputMapTrigger->get_text());
	if (trigger.empty()) {
		if (mode != Modes::LOAD)
			fields.inputInputMapTrigger->grab_focus();
		throw Message("Enter a trigger.\n");
	}
	if (inputTriggerHandler->count(trigger)) {
		if (mode != Modes::LOAD)
			fields.inputInputMapTrigger->grab_focus();
		throw Message("That trigger is aready in use.\n");
	}

	if (fields.inputMapDefaultColor->get_tooltip_text().empty()) {
		throw Message("You need to set a color.\n");
	}
}

void InputMap::storeData(Modes mode) {

	string trigger(fields.inputInputMapTrigger->get_text());
	if (mode == Modes::EDIT)
		inputTriggerHandler->replace(fieldsData.at("trigger"), trigger);
	else
		inputTriggerHandler->add(trigger);

	fieldsData.clear();

	if (fields.stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		fieldsData["type"]   = "Element";
		fieldsData["target"] = fields.comboBoxInputMapElement->get_active_text();
	}
	else {
		fieldsData["type"]   = "Group";
		fieldsData["target"] = fields.comboBoxInputMapGroup->get_active_text();
	}

	fieldsData["trigger"] = trigger;
	fieldsData["color"]   = fields.inputMapDefaultColor->get_tooltip_text();
	fieldsData["filter"]  = fields.comboBoxInputMapFilter->get_active_text();
}

void InputMap::retrieveData(Modes mode) {
	if (fieldsData["type"] == "Element") {
		fields.stackElementAndGroup->set_visible_child("InputTypeElement");
		fields.comboBoxInputMapElement->set_active_text(fieldsData["target"]);
	}
	else {
		fields.stackElementAndGroup->set_visible_child("InputTypeGroup");
		fields.comboBoxInputMapGroup->set_active_text(fieldsData["target"]);
	}
	fields.inputInputMapTrigger->set_text(fieldsData["trigger"]);
	DialogColors::getInstance()->colorizeButton(fields.inputMapDefaultColor, fieldsData["color"]);
	fields.comboBoxInputMapFilter->set_active_text(fieldsData["filter"]);
}

void InputMap::cancelData(Modes mode) {}

const string InputMap::createPrettyName() const {
	return fieldsData.at("trigger") + " -> " + fieldsData.at("type") + " " + fieldsData.at("target");
}

const string InputMap::getCssClass() const {
	return "InputMapBoxButton";
}

