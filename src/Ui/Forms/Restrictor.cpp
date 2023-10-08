/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	Restrictor.cpp
 * @since	Apr 30, 2023
 * @author	Patricio A. Rossi (MeduZa)
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

#include "Restrictor.hpp"

using namespace LEDSpicerUI::Ui::Forms;

Restrictor::RestrictorFields* Restrictor::fields = nullptr;
CollectionHandler* Restrictor::restrictors       = nullptr;

Restrictor::Restrictor(unordered_map<string, string>& data) :
	Form(data)
{
	// Check load.
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
}

Restrictor::~Restrictor() {
	// if destroyed mark restrictor and id available.
	if (fieldsData.count(NAME) and fieldsData.count(ID)) {
		restrictors->remove(createName());
		markRestrictorUsed();
	}
}

void Restrictor::initialize(RestrictorFields* fields) {

	Restrictor::fields = fields;
	restrictors = CollectionHandler::getInstance("restrictors");

	// Set Restrictors
	auto row = *(fields->restrictorsListstore->append());
	row.set_value(0, string());
	row.set_value(1, string("Select Restrictor"));
	row.set_value(2, false);
	for (auto& d : Defaults::restrictorsInfo) {
		row = *(fields->restrictorsListstore->append());
		row.set_value(0, d.first);
		row.set_value(1, d.second.name);
		row.set_value(2, true);
	}
	/*
	 * When the device selector changes, update fields.
	 */
	fields->comboBoxRestrictors->signal_changed().connect([fields]() {
		auto name = fields->comboBoxRestrictors->get_active_id();
		// GPWiz49
		fields->williamsMode->hide();
		// UltraStik360
		fields->hasRestrictor->get_parent()->hide();
		// GPWiz40RotoX
		fields->speedOn->get_parent()->get_parent()->hide();
		// empty Name is reset.
		fields->comboBoxId->set_active(-1);
		fields->player->set_active(0);
		fields->joystick->set_active(0);
		fields->comboBoxId->set_sensitive(false);
		fields->player->set_sensitive(false);
		fields->joystick->set_sensitive(false);
		for (auto& w : Defaults::allWays)
			fields->waysIcons.at(w)->set_visible(false);
		if (name.empty())
			return;
		fields->comboBoxId->set_sensitive(true);
		fields->player->set_sensitive(true);
		fields->joystick->set_sensitive(true);
		if (name == "UltraStik360")
			fields->hasRestrictor->get_parent()->show();
		if (name == "GPWiz49")
			fields->williamsMode->show();
		if (name == "GPWiz40RotoX")
			fields->speedOn->get_parent()->get_parent()->show();
		for (auto& w : Defaults::restrictorsInfo.at(name).ways)
			fields->waysIcons.at(w)->set_visible(true);
	});
}

string const Restrictor::createPrettyName() const {
	return Defaults::restrictorsInfo.at(fieldsData.at(NAME)).name +
		" Id: " + fieldsData.at(ID) +
		" P" + fieldsData.at(PLAYER) +
		" J" + fieldsData.at(JOYSTICK);
}

string const Restrictor::createName() const {
	string extra(isDual(fieldsData.at(NAME)) ?  '-' + fieldsData.at(PLAYER) + '_' + fieldsData.at(JOYSTICK) : "");
	return fieldsData.at(NAME) + "_" + fieldsData.at(ID) + extra;
}

void Restrictor::resetForm(Modes mode) {
	if (mode == Modes::ADD) {
		// Restrictors are created after a valid restrictor name is selected.
		fieldsData[NAME] = fields->comboBoxRestrictors->get_active_id();
		processIdComboBox();
		fields->comboBoxRestrictors->set_sensitive(true);
		fields->comboBoxId->set_active_id("");
		fields->player->set_active(0);
		fields->joystick->set_active(0);
		fields->hasRestrictor->set_active(false);
		fields->handleMouse->set_active(false);
		fields->williamsMode->set_active(false);
		fields->speedOn->set_value(GZ40_DEFAULT_SPEED);
		fields->speedOff->set_value(GZ40_DEFAULT_SPEED);
	}
}

void Restrictor::isValid(Modes mode) {

	string
		name(fields->comboBoxRestrictors->get_active_id()),
		id(fields->comboBoxId->get_active_id()),
		player(fields->player->get_active_id()),
		joystick(fields->joystick->get_active_id()),
		nameId(name + "_" + id);

	if (name.empty())
		throw Message("Invalid restrictor\n");

	if (id.empty())
		throw Message("Invalid restrictor number\n");

	if (player == "0")
		throw Message("Invalid player number\n");
	if (joystick == "0")
		throw Message("Invalid joystick number\n");

	// Dual restrictors need extra check.
	if (isDual(name)) {
		nameId += ('-' + player + '_' + joystick);
		// edit with the same name is ok.
		if (mode == Modes::EDIT and nameId == createName())
			return;
	}
	// Is imposible to duplicate restrictors because the selectors gets disable but LOAD and dual can.
	if ((mode == Modes::LOAD or isDual(name)) and restrictors->isUsed(nameId))
		throw Message("Restrictor " + Defaults::restrictorsInfo.at(name).name + " ID " + id + " already exists\n");
}

void Restrictor::storeData(Modes mode) {
	string
		name  = fields->comboBoxRestrictors->get_active_id(),
		newId = fields->comboBoxId->get_active_id(),
		// only for edit:
		oldId = mode == Modes::EDIT ? createName() : "";

	// This will clean any anomaly.
	fieldsData.clear();

	fieldsData[NAME] = name;
	fieldsData[ID]   = newId;

	fieldsData[PLAYER]   = fields->player->get_active_id();
	fieldsData[JOYSTICK] = fields->joystick->get_active_id();

	if (mode == Modes::EDIT)
		restrictors->replace(oldId, createName());
	else
		restrictors->add(createName());

	if (name == "UltraStik360") {
		fieldsData[US360_HAS_RESTRICTOR] = fields->hasRestrictor->get_active() ? "true" : "false";
		fieldsData[US360_USE_MOUSE]      = fields->handleMouse->get_active()   ? "true" : "false";
	}
	else if (name == "GPWiz49") {
		fieldsData[GZ49_WILLIAMS] = fields->williamsMode->get_active() ? "true" : "false";
	}
	else if (name == "GPWiz40RotoX") {
		fieldsData[GZ40_SPEED_ON]  = std::to_string(static_cast<uint8_t>(fields->speedOn->get_value()));
		fieldsData[GZ40_SPEED_OFF] = std::to_string(static_cast<uint8_t>(fields->speedOff->get_value()));
	}

	markRestrictorUsed();
	resetForm(mode);
}

void Restrictor::retrieveData(Modes mode) {
	fields->comboBoxRestrictors->set_sensitive(false);
	// Force Change on name.
	fields->comboBoxRestrictors->set_active_id("");
	fields->comboBoxRestrictors->set_active_id(fieldsData[NAME]);
	processIdComboBox();
	fields->comboBoxId->set_active_id(fieldsData[ID]);
	fields->player->set_active_id(fieldsData[PLAYER]);
	fields->joystick->set_active_id(fieldsData[JOYSTICK]);

	if (fieldsData[NAME] == "UltraStik360") {
		fields->hasRestrictor->set_active(fieldsData.count(US360_HAS_RESTRICTOR) and fieldsData.at(US360_HAS_RESTRICTOR) == "true");
		fields->handleMouse->set_active(fieldsData.count(US360_USE_MOUSE) and fieldsData.at(US360_USE_MOUSE) == "true");
	}
	else if (fieldsData[NAME] == "GPWiz49") {
		fields->williamsMode->set_active(fieldsData.count(GZ49_WILLIAMS) and fieldsData.at(GZ49_WILLIAMS) == "true");
	}
	else if (fieldsData[NAME] == "GPWiz40RotoX") {
		fields->speedOn->set_value(fieldsData.count(GZ40_SPEED_ON) ? std::stod(fieldsData.at(GZ40_SPEED_ON)) : GZ40_DEFAULT_SPEED);
		fields->speedOff->set_value(fieldsData.count(GZ40_SPEED_OFF) ? std::stod(fieldsData.at(GZ40_SPEED_OFF)) : GZ40_DEFAULT_SPEED);
	}
}

void Restrictor::cancelData(Modes mode) {
	resetForm(mode);
}

string const Restrictor::getCssClass() const {
	return "RestrictorBoxButton";
}

bool Restrictor::isDual(const string& name) {
	return name == "GPWiz40RotoX";
}

bool Restrictor::isUsed(const string& name, const string& id) {
	string nameId(name + '_' + id);
	if (isDual(name)) {
		if (restrictors->count(nameId) == 2)
			return true;
		return false;
	}
	return restrictors->isUsed(nameId);
}

void Restrictor::processIdComboBox() {
	// clean id combobox.
	auto children = fields->idListstore->children();
	std::vector<Gtk::TreeIter> rowsToRemove;
	for (auto iter = children.begin(); iter != children.end(); ++iter)
		rowsToRemove.push_back(*iter);
	for (const auto& iter : rowsToRemove)
		fields->idListstore->erase(*iter);
	const string& name = fieldsData.at(NAME);

	auto row = *(fields->idListstore->append());
	row.set_value(0, string());
	row.set_value(1, string("Select Device Number"));
	row.set_value(2, false);
	for (int c = 0; c < Defaults::restrictorsInfo.at(name).maxIds; ++c) {
		auto row = *(fields->idListstore->append());
		auto id(std::to_string(c + 1));
		row.set_value(0, id);
		row.set_value(1, "Board #" + id);
		row.set_value(2, not isUsed(name, id));
	}
}

void Restrictor::markRestrictorUsed() {
	for (auto& ri : Defaults::restrictorsInfo) {
		// count used devices.
		int total = restrictors->count(ri.first);
		// duales
		if (isDual(ri.first))
			total /= 2;
		// set check restrictors.
		auto children = fields->restrictorsListstore->children();
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
