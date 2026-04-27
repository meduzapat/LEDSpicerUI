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

DialogInputMap::DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder)
{

	Gtk::Button
		* btnAddSourceMap = nullptr,
		* btnAddInputMap  = nullptr;

	builder->get_widget_derived("BoxInputSourceMaps", boxSourceMaps);
	builder->get_widget_derived("BoxInputMaps",       boxInputMaps);
	builder->get_widget("BtnApplyInputMap",           btnApply);
	builder->get_widget("BtnInputMapDefaultColor",    inputMapDefaultColor);
	builder->get_widget("ComboBoxInputMapFilter",     comboBoxInputMapFilter);
	builder->get_widget("ComboBoxInputMapElement",    comboBoxInputMapElement);
	builder->get_widget("ComboBoxInputMapGroup",      comboBoxInputMapGroup);
	builder->get_widget("InputInputMapTrigger",       inputInputMapTrigger);
	builder->get_widget("StackElementAndGroup",       stackElementAndGroup);
	builder->get_widget("BtnAddInputSourceMap",       btnAddSourceMap);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);

	setSignalAdd(btnAddSourceMap);
	setSignalAdd(btnAddInputMap);

	setSignalApply();

	// Activate color button.
	DialogColors::getInstance()->activateColorButton(inputMapDefaultColor);

	// When the stack page changes, refresh the relevant combo and clear the other.
	stackElementAndGroup->connect_property_changed("visible-child", [this]() {
		if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
			Storage::CollectionHandler::getInstance(COLLECTION_ELEMENTS)->refreshComboBox(comboBoxInputMapElement, {PROP_EXPAND});
			comboBoxInputMapGroup->set_active(-1);
		}
		else {
			Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)->refreshComboBox(comboBoxInputMapGroup);
			comboBoxInputMapElement->set_active(-1);
		}
	});

}

void DialogInputMap::setNormalBox(const bool flag) noexcept {
	box = flag ? boxSourceMaps : boxInputMaps;
}

void DialogInputMap::load(XMLHelper* values) noexcept {
	createItems(
		values->getData(Defaults::createCommonUniqueId({ownerData->createUniqueId(), COLLECTION_INPUT_MAPS})),
		values
	);
}

void DialogInputMap::setOwner(
	Storage::BoxButtonCollection* collection,
	Storage::Data* owner
) noexcept {
	boxSourceMaps->wipe();
	boxInputMaps->wipe();
	// Sourceless inputs route their maps into the input-level BoxInputMaps panel;
	setNormalBox(not owner->getProperties().isSet(SOURCELESS));
	DialogForm::setOwner(collection, owner);
}

void DialogInputMap::clearForm() noexcept {
	Storage::CollectionHandler::getInstance(COLLECTION_ELEMENTS)->refreshComboBox(comboBoxInputMapElement, {PROP_EXPAND});
	Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)->refreshComboBox(comboBoxInputMapGroup);
	comboBoxInputMapElement->set_active(-1);
	comboBoxInputMapGroup->set_active(-1);
	inputInputMapTrigger->set_text("");
	comboBoxInputMapFilter->set_active_id("Normal");
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

	if (inputInputMapTrigger->get_text().empty()) {
		if (action != Actions::LOAD)
			inputInputMapTrigger->grab_focus();
		throw Message("Enter a trigger.");
	}

	if (inputMapDefaultColor->get_label().empty())
		throw Message("You need to set a color.");

	string uid(createUniqueId());
	if (action != Actions::EDIT or currentData->createUniqueId() != uid) {
		if (currentData->getCollectionHandler()->isIdSet(uid))
			throw Message("This trigger already exists for this source.");
	}
}

void DialogInputMap::storeData() noexcept {
	string type;
	Storage::Data* data = nullptr;
	if (stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
		type = ELEMENT;
		data = Storage::CollectionHandler::getInstance(COLLECTION_ELEMENTS)->get(comboBoxInputMapElement->get_active_text());
	}
	else {
		type = GROUP;
		data = Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)->get(comboBoxInputMapGroup->get_active_text());
	}

	currentData->setValue(TRIGGER, inputInputMapTrigger->get_text());
	currentData->setValue(TYPE,    type);
	currentData->setValue(COLOR,   inputMapDefaultColor->get_label());
	currentData->setValue(FILTER,  comboBoxInputMapFilter->get_active_id());
	static_cast<Storage::InputMap*>(currentData)->setLink(data);
}

void DialogInputMap::retrieveData() noexcept {
	if (currentData->getValue(TYPE) == ELEMENT) {
		stackElementAndGroup->set_visible_child("InputTypeElement");
		comboBoxInputMapElement->set_active_id(currentData->getValue(TARGET));
	}
	else {
		stackElementAndGroup->set_visible_child("InputTypeGroup");
		comboBoxInputMapGroup->set_active_id(currentData->getValue(TARGET));
	}
	inputInputMapTrigger->set_text(currentData->getValue(TRIGGER));
	DialogColors::getInstance()->colorizeButton(inputMapDefaultColor, currentData->getValue(COLOR));
	comboBoxInputMapFilter->set_active_id(currentData->getValue(FILTER));
}

string DialogInputMap::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		ownerData->getProperties().getValue(UID),
		inputInputMapTrigger->get_text()
	});
}

LEDSpicerUI::Ui::Storage::Data* DialogInputMap::createData(StringUMap& rawData) const noexcept {

	const string
		type   = rawData.count(TYPE)   ? rawData.at(TYPE)   : emptyString,
		target = rawData.count(TARGET) ? rawData.at(TARGET) : emptyString;

	auto handler{type.empty() ? nullptr : (
		type == GROUP ?
			Storage::CollectionHandler::getInstance(COLLECTION_GROUPS) :
			Storage::CollectionHandler::getInstance(COLLECTION_ELEMENTS)
		)
	};

	auto im{new Storage::InputMap(rawData, handler ? handler->get(target) : nullptr)};
	im->getProperties().setValue(PID, ownerData->getProperties().getValue(UID));
	im->getProperties().setValue(IID, ownerData->getProperties().getValue(PID));
	return im;
}
