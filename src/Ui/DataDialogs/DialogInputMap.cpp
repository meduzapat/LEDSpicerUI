/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.cpp
 * @since     Sep 30, 2023
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

#include "DialogInputMap.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInputMap* DialogInputMap::instance = nullptr;

void DialogInputMap::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogInputMap", instance);
	}
}

DialogInputMap* DialogInputMap::getInstance() {
	return instance;
}

DialogInputMap::DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
DialogForm(obj, builder)
{
	// Connect Input map
	builder->get_widget_derived("BoxInputMap", box);
	builder->get_widget("BtnAddInputMap",      btnAdd);
	builder->get_widget("BtnApplyInputMap",    btnApply);
	setSignalAdd();
	setSignalApply();

	// Color and Filter.
	builder->get_widget("BtnInputMapDefaultColor", inputMapDefaultColor);
	builder->get_widget("ComboBoxInputMapFilter",  comboBoxInputMapFilter);
	// Others
	builder->get_widget("ComboBoxInputMapElement", comboBoxInputMapElement);
	builder->get_widget("ComboBoxInputMapGroup",   comboBoxInputMapGroup);
	builder->get_widget("InputInputMapTrigger",    inputInputMapTrigger);
	builder->get_widget("StackElementAndGroup",    stackElementAndGroup);

	// Activate color button.
	DialogColors::getInstance()->activateColorButton(inputMapDefaultColor);

	// When the page change on the stack remove other selection.
	stackElementAndGroup->connect_property_changed("visible-child", [&]() {
		if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
			mapElementCollectionHandler->refreshComboBox(comboBoxInputMapElement);
			comboBoxInputMapGroup->set_active(-1);
		}
		else {
			mapGroupCollectionHandler->refreshComboBox(comboBoxInputMapGroup);
			comboBoxInputMapElement->set_active(-1);
		}
	});
}

void DialogInputMap::load(XMLHelper* values) {
	createItems(values->getData(Defaults::createCommonUniqueId({owner->createUniqueId(), COLLECTION_INPUT_MAPS})), values);
}

void DialogInputMap::clearForm() {
	mapElementCollectionHandler->refreshComboBox(comboBoxInputMapElement);
	mapGroupCollectionHandler->refreshComboBox(comboBoxInputMapGroup);
	comboBoxInputMapElement->set_active(-1);
	comboBoxInputMapGroup->set_active(-1);
	inputInputMapTrigger->set_text("");
	comboBoxInputMapFilter->set_active_text("Normal");
	DialogColors::getInstance()->colorizeButton(inputMapDefaultColor, NO_COLOR);
}

void DialogInputMap::isValid() const {
	if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		if (comboBoxInputMapElement->get_active_text().empty())
			throw Message("Enter a valid element name.");
	}
	else {
		if (comboBoxInputMapGroup->get_active_text().empty())
			throw Message("Enter a valid group name.");
	}

	string trigger(inputInputMapTrigger->get_text());
	if (trigger.empty()) {
		if (mode != Modes::LOAD)
			inputInputMapTrigger->grab_focus();
		throw Message("Enter a trigger.");
	}
	// Check if is used.
	if (inputMapCollectionHandler->isUsed(trigger)) {
		// If editing and they are the same is OK.
		if (mode != Modes::EDIT or trigger != currentData->createUniqueId()) {
			throw Message("Trigger already in use.");
		}
	}

	if (inputMapDefaultColor->get_tooltip_text().empty()) {
		throw Message("You need to set a color.");
	}
}

void DialogInputMap::storeData() {

	// New trigger, linked map id, type and target
	string trigger(createUniqueId()), newLinkedMapId, type, target;
	if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		type = ELEMENT;
		target =  comboBoxInputMapElement->get_active_text();
	}
	else {
		type = GROUP;
		target = comboBoxInputMapGroup->get_active_text();
	}
	newLinkedMapId = Defaults::createCommonUniqueId({trigger, type + " " + target});

	if (mode == Modes::EDIT) {
		// the problem with this is that will only update values if the trigger is replaced.
		inputMapCollectionHandler->replace(currentData->createUniqueId(), trigger);
		inputLinkMapCollectionHandler->replace(Defaults::createCommonUniqueId({
			currentData->createUniqueId(),
			currentData->getValue(TYPE) + " " + currentData->getValue(TARGET)}
		), newLinkedMapId);
	}
	else {
		inputMapCollectionHandler->add(trigger);
		inputLinkMapCollectionHandler->add(newLinkedMapId);
	}

	currentData->wipe();

	currentData->setValue(TYPE, type);
	currentData->setValue(TARGET, target);

	currentData->setValue(TRIGGER, trigger);
	currentData->setValue(COLOR, inputMapDefaultColor->get_tooltip_text());
	currentData->setValue(FILTER, comboBoxInputMapFilter->get_active_text());
}

void DialogInputMap::retrieveData() {
	if (currentData->getValue(TYPE) == ELEMENT) {
		stackElementAndGroup->set_visible_child("InputTypeElement");
		comboBoxInputMapElement->set_active_text(currentData->getValue(TARGET));
	}
	else {
		stackElementAndGroup->set_visible_child("InputTypeGroup");
		comboBoxInputMapGroup->set_active_text(currentData->getValue(TARGET));
	}
	inputInputMapTrigger->set_text(currentData->getValue(TRIGGER));
	DialogColors::getInstance()->colorizeButton(inputMapDefaultColor, currentData->getValue(COLOR));
	comboBoxInputMapFilter->set_active_text(currentData->getValue(FILTER));
}

string const DialogInputMap::createUniqueId() const {
	return inputInputMapTrigger->get_text();
}

LEDSpicerUI::Ui::Storage::Data* DialogInputMap::getData(unordered_map<string, string>& rawData) {
	return new Storage::InputMap(rawData);
}

const string DialogInputMap::getType() const {
	return "Input Map";
}
