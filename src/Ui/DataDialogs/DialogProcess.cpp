/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProcess.cpp
 * @since     May 3, 2023
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

#include "DialogProcess.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogProcess::DialogProcess(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	DialogForm(obj, builder)
{

	// Connect Process Box and buttons.
	builder->get_widget_derived("BoxProcess", box);

	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddProcess", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Process fields.
	builder->get_widget("EntryProcessName",       inputProcessName);
	builder->get_widget("EntryProcessSystemType", inputSystemType);
	builder->get_widget("SpinProcessRomPosition", spinProcessRomPosition);
	Defaults::attachNameFilter(inputProcessName);
	Defaults::attachNameFilter(inputSystemType);
}

void DialogProcess::load(DataMap& values) noexcept {
	createItems(values[COLLECTION_PROCESSES], values);
}

void DialogProcess::clearForm() noexcept {
	inputProcessName->set_text("");
	inputSystemType->set_text("");
	spinProcessRomPosition->set_text("0");
}

void DialogProcess::isValid() const {
	string name(inputProcessName->get_text());
	if (name.empty()) {
		if (action != Actions::LOAD)
			inputProcessName->grab_focus();
		throw Message("Invalid process name.");
	}

	if (inputSystemType->get_text().empty()) {
		if (action != Actions::LOAD)
			inputSystemType->grab_focus();
		throw Message("Invalid system type.");
	}

	string pos(spinProcessRomPosition->get_text());
	if (not pos.empty() and not Defaults::isNumber(pos)) {
		if (action != Actions::LOAD)
			spinProcessRomPosition->grab_focus();
		throw Message("Position needs to be a number.");
	}

	// If data is the same, just continue.
	if (action == Actions::EDIT and currentData->createUniqueId() == name)
		return;

	// Check new name for existence.
	if (currentData->getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::LOAD)
			inputProcessName->grab_focus();
		throw Message("Process " + name + " already registered.");
	}
}

void DialogProcess::storeData() noexcept {
	currentData->setValue(PARAM_PROCESS_NAME, inputProcessName->get_text());
	currentData->setValue(PARAM_SYSTEM, inputSystemType->get_text());
	// Not mandatory.
	if (not spinProcessRomPosition->get_text().empty())
		currentData->setValue(PARAM_PROCESS_POS, spinProcessRomPosition->get_text());
}

void DialogProcess::retrieveData() noexcept {
	inputProcessName->set_text(currentData->getValue(PARAM_PROCESS_NAME));
	inputSystemType->set_text(currentData->getValue(PARAM_SYSTEM));
	// Assume 0 if not present.
	spinProcessRomPosition->set_text(currentData->getValue(PARAM_PROCESS_POS, "0"));
}

string DialogProcess::createUniqueId() const noexcept {
	return inputProcessName->get_text();
}

LEDSpicerUI::Ui::Storage::Data* DialogProcess::createData(Values& rawData) const noexcept {
	return new Storage::Process(rawData);
}
