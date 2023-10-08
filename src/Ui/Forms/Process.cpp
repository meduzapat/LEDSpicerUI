/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Process.cpp
 * @since     May 2, 2023
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

#include "Process.hpp"

using namespace LEDSpicerUI::Ui::Forms;

Process::Fields* Process::fields           = nullptr;
CollectionHandler* Process::processHandler = nullptr;

Process::Process(unordered_map<string, string>& data) : Form(data) {
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);

	}
	resetForm(Modes::ADD);
}

Process::~Process() {
	if (fieldsData.count(PARAM_PROCESS_NAME) and not fieldsData.at(PARAM_PROCESS_NAME).empty())
		processHandler->remove(fieldsData.at(PARAM_PROCESS_NAME));
}

void Process::resetForm(Modes mode) {
	fields->InputProcessName->set_text("");
	fields->InputSystemType->set_text("");
	fields->InputRomPosition->set_text("0");
}

void Process::initialize(Fields& fields) {
	Process::fields = &fields;
	processHandler  = CollectionHandler::getInstance("process");
}

void Process::isValid(Modes mode) {
	string name(fields->InputProcessName->get_text());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			fields->InputProcessName->grab_focus();
		throw Message("Invalid process name\n");
	}

	if (fields->InputSystemType->get_text().empty()) {
		if (mode != Modes::LOAD)
			fields->InputSystemType->grab_focus();
		throw Message("Invalid system type\n");
	}

	string pos(fields->InputRomPosition->get_text());
	if (not pos.empty() and not Defaults::isNumber(pos)) {
		if (mode != Modes::LOAD)
			fields->InputRomPosition->grab_focus();
		throw Message("Position needs to be a number\n");
	}

	// If data is the same, just continue.
	if (mode == Modes::EDIT and fieldsData[PARAM_PROCESS_NAME] == name)
		return;

	// Check new name for existence.
	if (processHandler->isUsed(name)) {
		if (mode != Modes::LOAD)
			fields->InputProcessName->grab_focus();
		throw Message("Process " + name + " already registered\n");
	}
}

void Process::storeData(Modes mode) {
	string oldName;
	if (mode == Modes::EDIT)
		oldName = createName();

	// This will clean any anomaly.
	fieldsData.clear();

	fieldsData[PARAM_PROCESS_NAME] = fields->InputProcessName->get_text();
	fieldsData[PARAM_SYSTEM]  = fields->InputSystemType->get_text();
	if (not fields->InputRomPosition->get_text().empty())
		fieldsData[PARAM_PROCESS_POS] = fields->InputRomPosition->get_text();

	if (mode == Modes::EDIT)
		processHandler->replace(createName(), oldName);
	else
		processHandler->add(createName());
}

void Process::retrieveData(Modes mode) {
	fields->InputProcessName->set_text(fieldsData[PARAM_PROCESS_NAME]);
	fields->InputSystemType->set_text(fieldsData[PARAM_SYSTEM]);
	fields->InputRomPosition->set_text(fieldsData.count(PARAM_PROCESS_POS) ? fieldsData[PARAM_PROCESS_POS] : "0");
}

const string Process::createPrettyName() const {
	return string("Process: " + fieldsData.at(PARAM_PROCESS_NAME) + " System: " + fieldsData.at(PARAM_SYSTEM));
}

const string Process::createName() const {
	return fieldsData.at(PARAM_PROCESS_NAME);
}

const string Process::getCssClass() const {
	return "ProcessBoxButton";
}
