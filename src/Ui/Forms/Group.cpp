/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.cpp
 * @since     Mar 16, 2023
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

#include "Group.hpp"

using namespace LEDSpicerUI::Ui::Forms;

Group::GroupFields Group::groupFields;
CollectionHandler* Group::groupHandler   = nullptr;
CollectionHandler* Group::elementHandler = nullptr;

Group::Group(unordered_map<string, string>& data) : Form(data) {
	elementHandler->registerDestination(&elements);
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
	onActivate();
}

Group::~Group() {
	elementHandler->release(&elements);
	if (fieldsData.count(NAME))
		groupHandler->remove(fieldsData.at(NAME));
}

void Group::initialize(GroupFields& groupFields) {
	Group::groupFields = std::move(groupFields);
	groupHandler       = CollectionHandler::getInstance("groups");
	elementHandler     = CollectionHandler::getInstance("elements");
}

string const Group::createPrettyName() const {
	return fieldsData.at(NAME);
}

void Group::resetForm(Modes mode) {
	groupFields.InputGroupName->set_text("");
	DialogColors::getInstance()->colorizeButton(
		groupFields.BtnGroupDefaultColor,
		NO_COLOR
	);
}

void Group::isValid(Modes mode) {
	string name(groupFields.InputGroupName->get_text());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			groupFields.InputGroupName->grab_focus();
		throw Message("Invalid group name.\n");
	}

	// If data is the same, just continue.
	if (mode == Modes::EDIT and fieldsData[NAME] == name)
		return;

	// Check for dupe group name.
	if (groupHandler->isUsed(name)) {
		if (mode != Modes::LOAD)
			groupFields.InputGroupName->grab_focus();
		throw Message("Group " + name + " already exists.\n");
	}

	// Check no selected elements.
	/*if (not dialogSelectElements->getNumberElements()) {
		if (editMode or (not editMode and not fieldsData.count(NAME)))
			throw Message("You need to add at least one element.\n");
	}*/
}

void Group::storeData(Modes mode) {

	const string name(groupFields.InputGroupName->get_text());

	if (mode == Modes::EDIT)
		groupHandler->replace(fieldsData.at(NAME), name);
	else
		groupHandler->add(name);

	// This will clean any anomaly.
	fieldsData.clear();

	fieldsData[NAME] = name;
	if (groupFields.BtnGroupDefaultColor->get_tooltip_text().empty())
		fieldsData.erase(DEFAULT_COLOR);
	else
		fieldsData[DEFAULT_COLOR] = groupFields.BtnGroupDefaultColor->get_tooltip_text();
	// Save elements.
	onDeActivate();
}

void Group::retrieveData(Modes mode) {
	groupFields.InputGroupName->set_text(fieldsData[NAME]);
	DialogColors::getInstance()->colorizeButton(
		groupFields.BtnGroupDefaultColor,
		fieldsData.count(DEFAULT_COLOR) ? fieldsData[DEFAULT_COLOR] : NO_COLOR
	);
	// Retrieve the elements.
	onActivate();
}

void Group::cancelData(Modes mode) {
	onDeActivate();
}

const string Group::getCssClass() const {
	return "GroupBoxButton";
}

void Group::onDeActivate() {
	DialogSelectElements::getInstance()->getItems(elements);
}

void Group::onActivate() {
	DialogSelectElements::getInstance()->setItems(elements);
}
