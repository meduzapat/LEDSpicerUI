/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictorMap.cpp
 * @since     Oct 14, 2023
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

#include "DialogRestrictorMap.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogRestrictorMap* DialogRestrictorMap::instance = nullptr;

void DialogRestrictorMap::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogRestrictorMap", instance);
	}
}

DialogRestrictorMap* DialogRestrictorMap::getInstance() {
	return instance;
}

DialogRestrictorMap::DialogRestrictorMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Restrictor Box and buttons.
	builder->get_widget_derived("BoxRestrictorMappings", box);
	builder->get_widget("BtnAddRestrictorMap",           btnAdd);
	builder->get_widget("BtnApplyRestrictorMap",         btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogRestrictorMap::onAddClicked));
	setSignalApply();

	builder->get_widget("ComboboxRestrictorPlayer",    player);
	builder->get_widget("ComboboxRestrictorJoystick",  joystick);
	builder->get_widget("ComboboxRestrictorInterface", interface);

	// For checking.
	builder->get_widget("ComboBoxRestrictors", comboBoxRestrictors);

	// list of map ids to pick from.
	liststoreRestrictorMapId = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictorMapId").get());
}

void DialogRestrictorMap::load(XMLHelper* values) {
	createItems(values->getData(Defaults::createCommonUniqueId({owner->createUniqueId(), COLLECTION_RESTRICTOR_MAP})), values);
}

void DialogRestrictorMap::clearForm() {
	player->set_active(0);
	joystick->set_active(0);
	populateInterfacesCombobox();
	// Check if the hardware have interfaces left.
	if (Defaults::isMulti(comboBoxRestrictors->get_active_id()))
		interface->get_parent()->show();
	else
		interface->get_parent()->hide();
}

void DialogRestrictorMap::isValid() const {

	string newPlayer(createUniqueId());

	if (player->get_active_id() == "0") {
		throw Message("Invalid player number.");
	}
	if (joystick->get_active_id() == "0") {
		throw Message("Invalid joystick number.");
	}
	if (Defaults::isMulti(comboBoxRestrictors->get_active_id()) and interface->get_active_id().empty()) {
		throw Message("Select a hardware interface.");
	}
	if (mode != Modes::EDIT or newPlayer != currentData->createUniqueId()) {
		if (playerCombinations->isUsed(newPlayer)) {
			throw Message("That player - joystick combination is already in use.");
		}
	}
}

void DialogRestrictorMap::storeData() {

	if (mode == Modes::EDIT) {
		playerCombinations->replace(currentData->createUniqueId(), createUniqueId());
	}
	else {
		playerCombinations->add(createUniqueId());
	}

	currentData->wipe();

	currentData->setValue(PLAYER,   player->get_active_id());
	currentData->setValue(JOYSTICK, joystick->get_active_id());
	// This is only necessary for multi hardware.
	if (Defaults::isMulti(comboBoxRestrictors->get_active_id())) {
		currentData->setValue(RESTRICTOR_INTERFACE, interface->get_active_id());
	}
}

void DialogRestrictorMap::retrieveData() {
	player->set_active_id(currentData->getValue(PLAYER));
	joystick->set_active_id(currentData->getValue(JOYSTICK));
	if (Defaults::isMulti(comboBoxRestrictors->get_active_id())) {
		interface->set_active_id(currentData->getValue(RESTRICTOR_INTERFACE));
	}
}

string const DialogRestrictorMap::createUniqueId() const {
	return Defaults::createCommonUniqueId({player->get_active_id(), joystick->get_active_id()});
}

const bool DialogRestrictorMap::checkAvailableInterfaces() const {
	// Allowed interfaces.
	auto total = Defaults::restrictorsInfo.at(comboBoxRestrictors->get_active_id()).interfaces;
	// Size of the map box.
	auto size = box->getSize();
	return size < total;
}

const string DialogRestrictorMap::getType() const {
	return "Player Mapping";
}

LEDSpicerUI::Ui::Storage::Data* DialogRestrictorMap::getData(unordered_map<string, string>& rawData) {
	return new Storage::RestrictorMap(rawData);
}

void DialogRestrictorMap::populateInterfacesCombobox() {
	string name(comboBoxRestrictors->get_active_id());
	Defaults::populateComboBoxWithIds(
		liststoreRestrictorMapId,
		Defaults::restrictorsInfo.at(name).interfaces,
		[&](const string& id) {
			for (auto i : *items) {
				if (i->getData()->getValue(RESTRICTOR_INTERFACE) == id)
					return true;
			}
			return false;
		},
		"Select Interface",
		"Interface #"
	);
	interface->set_active(0);
}

void DialogRestrictorMap::afterCreate(Storage::BoxButton* boxButton) {
	// This is necessary to disable the add if needed.
	btnAdd->set_sensitive(checkAvailableInterfaces());
}

void DialogRestrictorMap::afterDeleteConfirmation(Storage::BoxButton* boxButton) {
	DialogForm::afterDeleteConfirmation(boxButton);
	// Assume that after deleting we can add a new map
	btnAdd->set_sensitive(true);
}
