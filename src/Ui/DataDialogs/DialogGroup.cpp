/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroupDialog.cpp
 * @since     Feb 13, 2023
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

#include "DialogGroup.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogGroup* DialogGroup::instance = nullptr;

void DialogGroup::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogGroup", instance);
	}
}

DialogGroup* DialogGroup::getInstance() {
	return instance;
}

DialogGroup::DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Groups Box and button.
	builder->get_widget_derived("BoxGroups", box);
	builder->get_widget("BtnAddGroup",       btnAdd);
	builder->get_widget("BtnApplyGroup",     btnApply);
	setSignalAdd();
	setSignalApply();

	builder->get_widget("InputGroupName",       inputGroupName);
	builder->get_widget("BtnGroupDefaultColor", btnGroupDefaultColor);
	DialogColors::getInstance()->activateColorButton(btnGroupDefaultColor);

	// Name Generator.
	Gtk::Dialog* dialogGenerateGroupName = nullptr;
	Gtk::Button* btnGenerateGroupName    = nullptr;
	Gtk::ComboBoxText
		* comboBoxGN1 = nullptr,
		* comboBoxGN2 = nullptr;

	builder->get_widget("DialogGenerateGroupName", dialogGenerateGroupName);
	builder->get_widget("BtnGenerateGroupName",    btnGenerateGroupName);
	builder->get_widget("ComboBoxGN1", comboBoxGN1);
	builder->get_widget("ComboBoxGN2", comboBoxGN2);

	// Element Selector
	builder->get_widget_derived("BoxGroupElements", boxElements, "BtnGroupElementUp", "BtnGroupElementDn");
	Gtk::Button* btnAddElements = nullptr;
	builder->get_widget("BtnAddGroupElements", btnAddElements);
	btnAddElements->signal_clicked().connect([&]() {
		DialogSelect::getInstance()->RunDialog();
	});

	btnGenerateGroupName->signal_clicked().connect([=]() {
		comboBoxGN1->set_active(-1);
		comboBoxGN2->set_active(-1);
		if (dialogGenerateGroupName->run() == Gtk::ResponseType::RESPONSE_APPLY) {
			inputGroupName->set_text(comboBoxGN1->get_active_id() + comboBoxGN2->get_active_id());
			inputGroupName->grab_focus();
		}
		dialogGenerateGroupName->hide();
	});
}

void DialogGroup::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_GROUP), values);
}

void DialogGroup::createSubItems(XMLHelper* values) {
	DataDialogs::DialogSelect::getInstance()->load(values);
}

void DialogGroup::clearForm() {
	inputGroupName->set_text("");
	DialogColors::getInstance()->colorizeButton(btnGroupDefaultColor, NO_COLOR);
	// This need to be run before activate.
	DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
		boxElements,
		COLLECTION_ELEMENT,
		COLLECTION_GROUP,
		DialogSelect::DELETER
	);
}

void DialogGroup::isValid() const {
	string name(createUniqueId());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			inputGroupName->grab_focus();
		throw Message("Invalid group name.");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (groupCollectionHandler->isUsed(name)) {
		if (mode != Modes::EDIT or currentData->createUniqueId() != name) {
			if (mode != Modes::LOAD)
				inputGroupName->grab_focus();
			throw Message("Group with name " + name + " already exist.");
		}
	}

	// Check no selected elements.
	/*if (not dialogSelectElements->getNumberElements()) {
		if (editMode or (not editMode and not fieldsData.count(NAME)))
			throw Message("You need to add at least one element.");
	}*/
}

void DialogGroup::storeData() {

	if (mode == Modes::EDIT)
		groupCollectionHandler->replace(currentData->createUniqueId(), createUniqueId());
	else
		groupCollectionHandler->add(createUniqueId());

	// This will clean any anomaly.
	currentData->wipe();

	currentData->setValue(NAME, inputGroupName->get_text());
	if (not btnGroupDefaultColor->get_tooltip_text().empty()) {
		currentData->setValue(DEFAULT_COLOR, btnGroupDefaultColor->get_tooltip_text());
	}
}

void DialogGroup::retrieveData() {
	inputGroupName->set_text(currentData->getValue(NAME));
	DialogColors::getInstance()->colorizeButton(
		btnGroupDefaultColor,
		currentData->getValue(DEFAULT_COLOR).empty() ? NO_COLOR : currentData->getValue(DEFAULT_COLOR)
	);
}

string const DialogGroup::createUniqueId() const {
	return Defaults::createCommonUniqueId({inputGroupName->get_text()});
}

const string DialogGroup::getType() const {
	return "group";
}

LEDSpicerUI::Ui::Storage::Data* DialogGroup::getData(unordered_map<string, string>& rawData) {
	return new Storage::Group(rawData);
}
