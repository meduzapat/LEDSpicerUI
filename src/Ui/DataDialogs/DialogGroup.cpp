/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.cpp
 * @since     Feb 13, 2023
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

#include "DialogGroup.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;
using namespace LEDSpicerUI::Ui::Storage;

DialogGroup::DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder),
	elementRequest{
		nullptr,
		NAME,
		TYPE_ELEMENT,
		COLLECTION_GROUP_LINKS,
		CollectionHandler::getInstance(COLLECTION_ELEMENTS),
		{}
	}
{
	Gtk::Button
		* btnAdd         = nullptr,
		* btnAddElements = nullptr;

	builder->get_widget_derived("BoxGroups",    box);
	builder->get_widget("BtnApplyGroup",        btnApply);
	builder->get_widget("BtnAddGroup",          btnAdd);
	builder->get_widget("InputGroupName",       inputGroupName);
	builder->get_widget("BtnGroupDefaultColor", btnGroupDefaultColor);
	builder->get_widget_derived("BoxGroupElements", boxElements, "BtnGroupElementUp", "BtnGroupElementDn");

	setSignalAdd(btnAdd);
	setSignalApply();

	elementRequest.displayBox = boxElements;

	DialogColors::getInstance()->activateColorButton(btnGroupDefaultColor);

	// Element selector button — opens picker with strip-expand support.
	builder->get_widget("BtnAddGroupElements", btnAddElements);
	btnAddElements->signal_clicked().connect([this]() {
		DialogSelect::getInstance()->setUp(getPrimaryChildCollection(), elementRequest);
		DialogSelect::getInstance()->open();
	});

	CollectionHandler::getInstance(COLLECTION_ELEMENTS)->registerSensitivity(btnAdd);

	// Group name generator dialog.
	Gtk::Dialog* dialogGenerateGroupName = nullptr;
	Gtk::Button* btnGenerateGroupName    = nullptr;
	Gtk::ComboBoxText
		* comboBoxGN1 = nullptr,
		* comboBoxGN2 = nullptr;

	builder->get_widget("DialogGenerateGroupName", dialogGenerateGroupName);
	builder->get_widget("BtnGenerateGroupName",    btnGenerateGroupName);
	builder->get_widget("ComboBoxGN1",             comboBoxGN1);
	builder->get_widget("ComboBoxGN2",             comboBoxGN2);

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

void DialogGroup::load(XMLHelper* values) noexcept {
	createItems(values->getData(COLLECTION_GROUPS), values);
}

void DialogGroup::createSubItems(XMLHelper* values) noexcept {
	DialogSelect::getInstance()->load(values, currentData->createUniqueId());
}

void DialogGroup::clearForm() noexcept {
	inputGroupName->set_text("");
	DialogColors::getInstance()->colorizeButton(btnGroupDefaultColor, NO_COLOR);
	boxElements->wipe();
}

void DialogGroup::isValid() const {
	string name(createUniqueId());
	if (name.empty()) {
		if (action != Actions::LOAD)
			inputGroupName->grab_focus();
		throw Message("Invalid group name.");
	}
	if (currentData->getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != name) {
			if (action != Actions::LOAD)
				inputGroupName->grab_focus();
			throw Message("Group with name " + name + " already exists.");
		}
	}

//	if (action != Actions::LOAD and not boxElements->getSize())
//		throw Message("Add at least one element.");
}

void DialogGroup::storeData() noexcept {
	currentData->setValue(NAME, inputGroupName->get_text());
	if (not btnGroupDefaultColor->get_label().empty())
		currentData->setValue(DEFAULT_COLOR, btnGroupDefaultColor->get_label());
	DialogSelect::getInstance()->reindex();
}

void DialogGroup::retrieveData() noexcept {
	inputGroupName->set_text(currentData->getValue(NAME));
	DialogColors::getInstance()->colorizeButton(
		btnGroupDefaultColor,
		currentData->getValue(DEFAULT_COLOR).empty() ? NO_COLOR : currentData->getValue(DEFAULT_COLOR)
	);
	DialogSelect::getInstance()->refresh();
}

string DialogGroup::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({inputGroupName->get_text()});
}

Data* DialogGroup::createData(StringUMap& rawData) const noexcept {
	return new Group(rawData);
}

void DialogGroup::wireChildrenDialogs() noexcept {
	// This is necessary because Group uses DialogSelect and its not registered.
	currentData->setUp();
	DialogSelect::getInstance()->setUp(getPrimaryChildCollection(), elementRequest);
	getPrimaryChildCollection()->registerSensitivity(btnApply);
}

void DialogGroup::disconnectChildrenDialogs() noexcept {
	getPrimaryChildCollection()->releaseSensitive(btnApply);
	currentData->tearDown();
}
