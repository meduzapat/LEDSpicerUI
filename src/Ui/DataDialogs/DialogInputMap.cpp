/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.cpp
 * @since     Sep 30, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

void DialogInputMap::initialize(Glib::RefPtr<Gtk::Builder> const& builder) {
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
	builder->get_widget_derived("BoxInputSourceMaps", box);
	builder->get_widget("BtnApplyInputMap",           btnApply);
	builder->get_widget("BtnInputMapDefaultColor",    inputMapDefaultColor);
	builder->get_widget("ComboBoxInputMapFilter",     comboBoxInputMapFilter);
	builder->get_widget("ComboBoxInputMapElement",    comboBoxInputMapElement);
	builder->get_widget("ComboBoxInputMapGroup",      comboBoxInputMapGroup);
	builder->get_widget("InputInputMapTrigger",       inputInputMapTrigger);
	builder->get_widget("StackElementAndGroup",       stackElementAndGroup);

	Gtk::Button
		* btnAdd  = nullptr,
		* btnAdd2 = nullptr;
	builder->get_widget("BtnAddInputMap",       btnAdd);
	builder->get_widget("BtnAddInputSourceMap", btnAdd2);
	setSignalAdd(btnAdd);
	setSignalAdd(btnAdd2);
	setSignalApply();

	// Activate color button.
	DialogColors::getInstance()->activateColorButton(inputMapDefaultColor);

	// When the stack page changes, refresh the relevant combo and clear the other.
	stackElementAndGroup->connect_property_changed("visible-child", [&]() {
		if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
			Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)->refreshComboBox(comboBoxInputMapElement);
			comboBoxInputMapGroup->set_active(-1);
		}
		else {
			Storage::CollectionHandler::getInstance(COLLECTION_GROUP)->refreshComboBox(comboBoxInputMapGroup);
			comboBoxInputMapElement->set_active(-1);
		}
	});
}

void DialogInputMap::load(XMLHelper* values) {
	createItems(
		values->getData(Defaults::createCommonUniqueId({ownerData->createUniqueId(), COLLECTION_INPUT_MAPS})),
		values
	);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInputMap::getCollectionHandler() const {
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT_MAPS);
}

void DialogInputMap::clearForm() {
	Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)->refreshComboBox(comboBoxInputMapElement);
	Storage::CollectionHandler::getInstance(COLLECTION_GROUP)->refreshComboBox(comboBoxInputMapGroup);
	comboBoxInputMapElement->set_active(-1);
	comboBoxInputMapGroup->set_active(-1);
	inputInputMapTrigger->set_text("");
	comboBoxInputMapFilter->set_active_text("Normal");
	DialogColors::getInstance()->colorizeButton(inputMapDefaultColor, NO_COLOR);
}

void DialogInputMap::isValid() const {
	if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		if (comboBoxInputMapElement->get_active_text().empty())
			throw Message("Select a valid element.");
	}
	else {
		if (comboBoxInputMapGroup->get_active_text().empty())
			throw Message("Select a valid group.");
	}

	string trigger(inputInputMapTrigger->get_text());
	if (trigger.empty()) {
		if (action != Actions::LOAD)
			inputInputMapTrigger->grab_focus();
		throw Message("Enter a trigger.");
	}

	if (inputMapDefaultColor->get_label().empty()) {
		throw Message("You need to set a color.");
	}

	// Build the full candidate ID and check for duplicates.
	string target(stackElementAndGroup->get_visible_child_name() == "InputTypeElement"
		? comboBoxInputMapElement->get_active_text()
		: comboBoxInputMapGroup->get_active_text());
	string candidateId(ownerData->createUniqueId() + "_" + trigger + "_" + target);

	if (action != Actions::EDIT or candidateId != currentData->createUniqueId()) {
		if (getCollectionHandler()->isIdSet(candidateId)) {
			throw Message("This map already exists for this source.");
		}
	}
}

void DialogInputMap::storeData() {
	string
		type(stackElementAndGroup->get_visible_child_name() == "InputTypeElement" ? ELEMENT : GROUP),
		target(type == ELEMENT
			? comboBoxInputMapElement->get_active_text()
			: comboBoxInputMapGroup->get_active_text());

	currentData->setValue(TRIGGER, createUniqueId());
	currentData->setValue(TYPE,    type);
	currentData->setValue(TARGET,  target);
	currentData->setValue(COLOR,   inputMapDefaultColor->get_label());
	currentData->setValue(FILTER,  comboBoxInputMapFilter->get_active_text());

	// Store source ID so InputMap::createUniqueId() can build the full key.
	currentData->setProperty(SOURCE, ownerData->createUniqueId());
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

const string DialogInputMap::createUniqueId() const {
	return inputInputMapTrigger->get_text();
}

LEDSpicerUI::Ui::Storage::Data* DialogInputMap::createData(StringUMap& rawData) {
	string
		type(rawData.count(TYPE)     ? rawData.at(TYPE)   : ""),
		target(rawData.count(TARGET) ? rawData.at(TARGET) : "");

	auto* handler = type == ELEMENT
		? Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)
		: Storage::CollectionHandler::getInstance(COLLECTION_GROUP);

	Storage::Data* linkTarget = handler->get(target);
	if (not linkTarget) {
		throw Message("Cannot find " + type + " \"" + target + "\" for input map.");
	}

	return new Storage::InputMap(rawData, "map", TARGET, linkTarget);
}

const string DialogInputMap::getType() const {
	return "Input Map";
}
