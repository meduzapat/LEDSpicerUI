/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProcess.cpp
 * @since     May 3, 2023
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

#include "DialogProcess.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogProcess* DialogProcess::instance = nullptr;

void DialogProcess::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogProcess", instance);
	}
}

DialogProcess* DialogProcess::getInstance() {
	return instance;
}

DialogProcess::DialogProcess(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Process Box and buttons.
	builder->get_widget_derived("BoxProcess", box);
	builder->get_widget("BtnAddProcess",      btnAdd);
	builder->get_widget("BtnApplyProcess",    btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogProcess::onAddClicked));
	setSignalApply();

	// Process fields.
	builder->get_widget("InputProcessName", inputProcessName);
	builder->get_widget("InputSystemType",  inputSystemType);
	builder->get_widget("InputRomPosition", inputRomPosition);
}

void DialogProcess::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_PROCESS), values);
}

void DialogProcess::clearForm() {
	inputProcessName->set_text("");
	inputSystemType->set_text("");
	inputRomPosition->set_text("0");
}

void DialogProcess::isValid() const {
	string name(inputProcessName->get_text());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			inputProcessName->grab_focus();
		throw Message("Invalid process name.");
	}

	if (inputSystemType->get_text().empty()) {
		if (mode != Modes::LOAD)
			inputSystemType->grab_focus();
		throw Message("Invalid system type.");
	}

	string pos(inputRomPosition->get_text());
	if (not pos.empty() and not Defaults::isNumber(pos)) {
		if (mode != Modes::LOAD)
			inputRomPosition->grab_focus();
		throw Message("Position needs to be a number.");
	}

	// If data is the same, just continue.
	if (mode == Modes::EDIT and currentData->createUniqueId() == name)
		return;

	// Check new name for existence.
	if (processHandler->isUsed(name)) {
		if (mode != Modes::LOAD)
			inputProcessName->grab_focus();
		throw Message("Process " + name + " already registered.");
	}
}

void DialogProcess::storeData() {
	if (mode == Modes::EDIT)
		processHandler->replace(currentData->createUniqueId(), createUniqueId());
	else
		processHandler->add(createUniqueId());

	// This will clean any anomaly.
	currentData->wipe();

	currentData->setValue(PARAM_PROCESS_NAME, inputProcessName->get_text());
	currentData->setValue(PARAM_SYSTEM, inputSystemType->get_text());
	// Not mandatory.
	if (not inputRomPosition->get_text().empty())
		currentData->setValue(PARAM_PROCESS_POS, inputRomPosition->get_text());
}

void DialogProcess::retrieveData() {
	inputProcessName->set_text(currentData->getValue(PARAM_PROCESS_NAME));
	inputSystemType->set_text(currentData->getValue(PARAM_SYSTEM));
	// Assume 0 if not present.
	inputRomPosition->set_text(currentData->getValue(PARAM_PROCESS_POS, "0"));
}

string const DialogProcess::createUniqueId() const {
	return inputProcessName->get_text();
}

const string DialogProcess::getType() const {
	return "map";
}

LEDSpicerUI::Ui::Storage::Data* DialogProcess::getData(unordered_map<string, string>& rawData) {
	return new Storage::Process(rawData);
}
