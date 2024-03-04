/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.cpp
 * @since     Apr 30, 2023
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

#include "DialogRestrictor.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogRestrictor* DialogRestrictor::instance = nullptr;

void DialogRestrictor::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogRestrictor", instance);
	}
}

DialogRestrictor* DialogRestrictor::getInstance() {
	return instance;
}

DialogRestrictor::DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Restrictor Box and buttons.
	builder->get_widget_derived("BoxRestrictors", box);
	builder->get_widget("BtnAddRestrictor",       btnAdd);
	builder->get_widget("BtnApplyRestrictors",    btnApply);
	setSignalAdd();
	setSignalApply();

	// Restrictor fields and models..
	builder->get_widget("ComboBoxRestrictors",          comboBoxRestrictors);
	builder->get_widget("ComboBoxRestrictorId",         comboBoxId);
	builder->get_widget("InputRestrictorPort",          serialPort);
	builder->get_widget("InputRestrictorHasRestrictor", hasRestrictor);
	builder->get_widget("InputRestrictorHandleMouse",   handleMouse);
	builder->get_widget("InputRestrictorWilliamsMode",  williamsMode);
	builder->get_widget("InputRestrictorSpeedOn",       speedOn);
	builder->get_widget("InputRestrictorSpeedOff",      speedOff);
	builder->get_widget("BreafRestrictor",              breafRestrictor);

	restrictorsListstore = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictors").get());
	idListstore          = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictorsId").get());

	// Add buttons.
	Gtk::Button* btnAddRestrictorMap = nullptr;
	builder->get_widget("BtnAddRestrictorMap", btnAddRestrictorMap);

	// Set Restrictors
	auto row = *(restrictorsListstore->append());
	row.set_value(0, string());
	row.set_value(1, string("Select Restrictor"));
	row.set_value(2, false);
	for (auto& d : Defaults::restrictorsInfo) {
		row = *(restrictorsListstore->append());
		row.set_value(0, d.first);
		row.set_value(1, d.second.name);
		row.set_value(2, true);
	}

	// Restrictor Icons.
	for (auto& w : Defaults::wayIds) {
		Gtk::FlowBoxChild* i;
		builder->get_widget(w.first, i);
		i->hide();
		waysIcons.emplace(w.second, i);
	}

	comboBoxRestrictors->signal_changed().connect([&, btnAddRestrictorMap]() {
		const string name = comboBoxRestrictors->get_active_id();
		if (name.empty()) {
			clearFormOthers();
			btnAddRestrictorMap->set_sensitive(false);
			btnApply->set_sensitive(false);
			previousName = "";
			return;
		}

		if (previousName == name) {
			return;
		}

		const string currentName(currentData->getValue(NAME));
		// Changed Restriction option.
		if (currentName.empty()) {
			clearFormOthers();
		}
		else if (mode != Modes::LOAD and name != currentName) {
			if (Message::ask("Are you sure you want to change the restrictor? all settings will be loss") == Gtk::RESPONSE_YES) {
				currentData->destroy();
				clearFormOthers();
			}
			else {
				comboBoxRestrictors->set_active_id(previousName);
				return;
			}
		}

		previousName = name;

		if (name == "UltraStik360") {
			hasRestrictor->get_parent()->show();
		}
		if (name == "GPWiz49") {
			williamsMode->show();
		}
		if (name == "GPWiz40RotoX") {
			speedOn->get_parent()->get_parent()->show();
		}

		if (Defaults::isIdUser(name, false)) {
			Defaults::populateComboBoxWithIds(
				idListstore,
				Defaults::restrictorsInfo.at(name).maxIds,
				[=](const string& id) {
					return restrictorsHandler->isUsed(Defaults::createHardwareUniqueId({{NAME, name}, {ID, id}}, false));
				},
				"Restrictor Number",
				"Hardware #"
			);
			comboBoxId->get_parent()->show();
		}
		if (Defaults::isSerial(name, false)) {
			serialPort->get_parent()->show();
		}

		// Set ways.
		for (auto& w : Defaults::restrictorsInfo.at(name).ways) {
			waysIcons.at(w)->show();
		}

		breafRestrictor->set_label(Defaults::restrictorsInfo.at(name).breaf);
		btnAddRestrictorMap->set_sensitive(true);
		btnApply->set_sensitive(true);
	});
}

void DialogRestrictor::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_RESTRICTORS), values);
}

void DialogRestrictor::createSubItems(XMLHelper* values) {
	DialogRestrictorMap::getInstance()->load(values);
}

void DialogRestrictor::clearForm() {
	comboBoxRestrictors->set_active_id("");
	clearFormOthers();
}

void DialogRestrictor::clearFormOthers() {

	comboBoxId->get_parent()->hide();
	comboBoxId->set_active_id("");
	serialPort->get_parent()->hide();
	serialPort->set_text("");

	// UltraStik360
	hasRestrictor->get_parent()->hide();
	hasRestrictor->set_active(false);
	handleMouse->set_active(false);

	// GPWiz49
	williamsMode->hide();
	williamsMode->set_active(false);

	// GPWiz40RotoX
	speedOn->get_parent()->get_parent()->hide();
	speedOn->set_value(GZ40_DEFAULT_SPEED);
	speedOff->set_value(GZ40_DEFAULT_SPEED);

	for (auto& w : Defaults::allWays) {
		waysIcons.at(w)->hide();
	}
	breafRestrictor->set_label("");
}

void DialogRestrictor::isValid() const {
	string
		name(comboBoxRestrictors->get_active_id()),
		id(comboBoxId->get_active_id()),
		port(serialPort->get_text());

	bool checkDupe = true;

	if (name.empty()) {
		throw Message("Invalid restrictor.");
	}

	if (Defaults::isIdUser(name, false) and id.empty()) {
		throw Message("Invalid restrictor number.");
	}

	string
		newName(createUniqueId()),
		hardwareName("Hardware " + Defaults::restrictorsInfo.at(name).name );
	if (mode == Modes::EDIT) {
		checkDupe = (currentData->createUniqueId() != newName);
	}

	if (Defaults::isIdUser(name, false) and checkDupe and restrictorsHandler->isUsed(newName)) {
		throw Message(hardwareName + " ID " + id + " already exists.");
	}
	if (Defaults::isSerial(name, false) and checkDupe and restrictorsHandler->isUsed(newName)) {
		throw Message(hardwareName + " that connects to " + (port.empty() ? "<autodetect>" : port) + " already exists.");
	}
	if (checkDupe and restrictorsHandler->isUsed(newName)) {
		throw Message(hardwareName + " already exists.");
	}
}

void DialogRestrictor::storeData() {

	if (mode == Modes::EDIT) {
		restrictorsHandler->replace(currentData->createUniqueId(), createUniqueId());
	}
	else {
		restrictorsHandler->add(createUniqueId());
	}

	// This will clean any anomaly.
	currentData->wipe();

	const string name(comboBoxRestrictors->get_active_id());

	currentData->setValue(NAME, name);
	if (Defaults::isIdUser(name, false)) {
		currentData->setValue(ID, comboBoxId->get_active_id());
	}

	if (Defaults::isSerial(name, false)) {
		currentData->setValue(PORT, serialPort->get_text());
	}

	if (name == "UltraStik360") {
		currentData->setValue(US360_HAS_RESTRICTOR, hasRestrictor->get_active() ? "true" : "false");
		currentData->setValue(US360_USE_MOUSE,      handleMouse->get_active()   ? "true" : "false");
	}
	else if (name == "GPWiz49") {
		currentData->setValue(GZ49_WILLIAMS, williamsMode->get_active() ? "true" : "false");
	}
	else if (name == "GPWiz40RotoX") {
		currentData->setValue(GZ40_SPEED_ON, std::to_string(static_cast<uint8_t>(speedOn->get_value())));
		currentData->setValue(GZ40_SPEED_OFF, std::to_string(static_cast<uint8_t>(speedOff->get_value())));
	}
}

void DialogRestrictor::retrieveData() {

	const string name(currentData->getValue(NAME));

	comboBoxRestrictors->set_active_id(name);

	if (Defaults::isIdUser(name, false)) {
		comboBoxId->set_active_id(currentData->getValue(ID));
	}
	if (Defaults::isSerial(name, false)) {
		serialPort->set_text(currentData->getValue(PORT));
	}

	if (name == "UltraStik360") {
		hasRestrictor->set_active(currentData->getValue(US360_HAS_RESTRICTOR) == "true");
		handleMouse->set_active(currentData->getValue(US360_USE_MOUSE) == "true");
	}
	else if (name == "GPWiz49") {
		williamsMode->set_active(currentData->getValue(GZ49_WILLIAMS) == "true");
	}
	else if (name == "GPWiz40RotoX") {
		speedOn->set_value(std::stod(currentData->getValue(GZ40_SPEED_ON, std::to_string(GZ40_DEFAULT_SPEED))));
		speedOff->set_value(std::stod(currentData->getValue(GZ40_SPEED_OFF, std::to_string(GZ40_DEFAULT_SPEED))));
	}
	markRestrictorUsed();
}

string const DialogRestrictor::createUniqueId() const {
	return Defaults::createHardwareUniqueId({
		{NAME, comboBoxRestrictors->get_active_id()},
		{ID,   comboBoxId->get_active_id()},
		{PORT, serialPort->get_text()}
	}, false);
}

const string DialogRestrictor::getType() const {
	return "restrictor";
}

LEDSpicerUI::Ui::Storage::Data* DialogRestrictor::getData(unordered_map<string, string>& rawData) {
	return new Storage::Restrictor(rawData);
}

void DialogRestrictor::markRestrictorUsed() {
	for (auto& ri : Defaults::restrictorsInfo) {
		// count used devices.
		const uint total    = restrictorsHandler->count(ri.first);
		const auto& children = restrictorsListstore->children();

		for (auto iter = children.begin(); iter != children.end(); ++iter) {
			Gtk::TreeModel::Row dataRow = *iter;

			string data;
			dataRow.get_value(0, data);
			if (data == ri.first) {
				dataRow.set_value(2, total < ri.second.maxIds);
				break;
			}
		}
	}
}
