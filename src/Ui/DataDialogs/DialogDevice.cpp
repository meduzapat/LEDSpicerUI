/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.cpp
 * @since     Feb 26, 2023
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

#include "DialogDevice.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogDevice::DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogFormHost(obj, builder)
{

	// Register its dialogs for refresh.
	registerChildDialog<DialogElement>(builder, "DialogElement", COLLECTION_ELEMENTS);

	// Register self so child dialogs can resolve the visible host window via familyToDialog.
	familyToDialog.emplace(COLLECTION_DEVICES, this);

	// Connect Device Box and buttons.
	builder->get_widget_derived("BoxDevices", box);

	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddDevice", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Device fields and models.
	builder->get_widget("ComboBoxDevices",        selectorCombo);
	builder->get_widget("ComboBoxDeviceId",       comboBoxId);
	builder->get_widget("ScaleDeviceChangePoint", changePoint);
	builder->get_widget("SpinnerDeviceLeds",      spinnerLeds); // RGB LEDs
	builder->get_widget("InputDevicePort",        inputDevicePort);
	builder->get_widget("BriefDevice",            brief);
	builder->get_widget("BtnAddElement",          btnAddElement);

	Gtk::Notebook* notebookDeviceConnections;
	builder->get_widget("NotebookDeviceConnections", notebookDeviceConnections);
	Defaults::attachNameFilter(inputDevicePort);

	// Populate Devices.
	initializeSelector(noDevice, Defaults::devicesInfo);
	// Models.
	idListstore = static_cast<Gtk::ListStore*>(builder->get_object("liststoreDeviceId").get());

	// On Number of LEDs is SET apply the changes to the Elements.
	spinnerLeds->signal_value_changed().connect([this]() {
		string name(selectorCombo->get_active_id());
		if (not name.empty() and not Defaults::isVariable(name)) return;
		btnAddElement->set_sensitive(spinnerLeds->get_value_as_int() > 1);
		// Convert LED into 3 Pins.
		const uint16_t pinsCount(spinnerLeds->get_value_as_int() * 3);
		if (selectorCombo->get_active_id().empty() or not pinsCount) return;
		DialogElement::getInstance()->changeNumberOfPins(pinsCount);
	});

	// On Number of LEDs is changed, update the add button.
	spinnerLeds->signal_changed().connect([this]() {
		btnAddElement->set_sensitive(spinnerLeds->get_value_as_int() > 1);
	});

	selectorCombo->signal_changed().connect([this]() {
		string newName{selectorCombo->get_active_id()};
		string msg;
		if (not newName.empty() and not previousName.empty()) {
			const auto
				& newInfo {Defaults::devicesInfo.at(newName)},
				& oldInfo {Defaults::devicesInfo.at(previousName)};
			msg = "Are you sure you want to convert \"" + oldInfo.name + "\" into \"" + newInfo.name + "\"?";
			if (Defaults::isVariable(newName)) {
				msg += "\nPin count will be set to the current value to preserve elements.";
			}
			else if (newInfo.pins < oldInfo.pins) {
				msg += "\nElements using pins above " + std::to_string(newInfo.pins) + " will be removed.";
			}
			if (oldInfo.layoutRGB != newInfo.layoutRGB)
				msg += "\nElement types incompatible with the new layout will be converted or removed.";
		}
		if (handleTypeSwitch(DialogElement::getInstance()->getBox(), msg)) {
			resetForm();
		}
	});
}

void DialogDevice::load(DataMap& values) noexcept {
	createItems(values[COLLECTION_DEVICES], values);
}

void DialogDevice::createSubItems(DataMap& values) noexcept {
	DialogElement::getInstance()->load(values);
}

void DialogDevice::resetForm() noexcept {

	string name{selectorCombo->get_active_id()};

	if (Defaults::isIdUser(name))
		comboBoxId->get_parent()->show();
	if (Defaults::isSerial(name))
		inputDevicePort->get_parent()->show();
	if (Defaults::isMonochrome(name))
		changePoint->get_parent()->show();
	if (Defaults::isVariable(name)) {
		spinnerLeds->get_parent()->show();
		// No pins no add elements.
		btnAddElement->set_sensitive(not currentData->getValue(PINS).empty());
	}
	else {
		btnAddElement->set_sensitive(true);
	}
	brief->set_text(Defaults::devicesInfo.at(name).brief);
	DialogForm::resetForm();
}

void DialogDevice::isValid() const {

	string
		name(selectorCombo->get_active_id()),
		id(comboBoxId->get_active_id()),
		port(inputDevicePort->get_text());

	bool checkDupe = true;

	if (name.empty()) throw Message("Invalid device");

	if (Defaults::isIdUser(name) and id.empty()) throw Message("Invalid device number");

	if (Defaults::isVariable(name)) {
		string pins(spinnerLeds->get_text());
		// LEDs vs Pins, need to divide.
		if (not Defaults::isBetween(pins, 1, Defaults::devicesInfo.at(name).pins / 3)) {
			if (action != Actions::LOAD) spinnerLeds->grab_focus();
			throw Message(
				"The number of pins need to be between one and the number of pins the device allows ("
				+ std::to_string(Defaults::devicesInfo.at(name).pins) + ")"
			);
		}
	}

	string
		newName(createUniqueId()),
		deviceName("Device ");
	deviceName += Defaults::devicesInfo.at(name).name;

	auto ch{currentData->getCollectionHandler()};
	if (action == Actions::EDIT)
		checkDupe = (ch->get(newName) != currentData);

	if (checkDupe and ch->isIdSet(newName)) {
		if (Defaults::isIdUser(name))
			throw Message(deviceName + " ID " + id + " already exists");
		if (Defaults::isSerial(name))
			throw Message(deviceName + " that connects to " + (port.empty() ? "<autodetect>" : port) + " already exists");
		throw Message(deviceName + " already exists");
	}
}

void DialogDevice::storeData() noexcept {

	string name(selectorCombo->get_active_id());

	currentData->setValue(NAME, name);
	if (Defaults::isIdUser(name))
		currentData->setValue(ID, comboBoxId->get_active_id());
	if (Defaults::isSerial(name))
		currentData->setValue(PORT, inputDevicePort->get_text());
	if (Defaults::isVariable(name))
		// Store Pins, not LEDs
		currentData->setValue(PINS, spinnerLeds->get_value_as_int() * 3);
	if (Defaults::isMonochrome(name))
		currentData->setValue(CHANGE_POINT, static_cast<int>(changePoint->get_value()));
}

void DialogDevice::retrieveData() noexcept {

	string name(currentData->getValue(NAME));

	selectorCombo->set_active_id(name);
	if (Defaults::isIdUser(name))
		comboBoxId->set_active_id(currentData->getValue(ID, "1"));
	if (Defaults::isSerial(name))
		inputDevicePort->set_text(currentData->getValue(PORT));
	if (Defaults::isVariable(name))
		// Read Pins, convert to LEDs
		spinnerLeds->set_value(currentData->getDouble(PINS) / 3);
	if (Defaults::isMonochrome(name) and currentData->isSet(CHANGE_POINT))
		changePoint->set_value(currentData->getDouble(CHANGE_POINT));

	markUsed([this](const string& id) {
		return currentData->getCollectionHandler()->countByKey(NAME, id) < Defaults::devicesInfo.at(id).maxIds;
	});
}

string DialogDevice::createUniqueId() const noexcept {
	return Defaults::createHardwareUniqueId({
		{NAME, selectorCombo->get_active_id()},
		{ID,   comboBoxId->get_active_id()},
		{PORT, inputDevicePort->get_text()}
	});
}

LEDSpicerUI::Ui::Storage::Data* DialogDevice::createData(Values& rawData) const noexcept {
	return new Storage::Device(rawData);
}

void DialogDevice::onEmpty() noexcept {
	btnAddElement->set_sensitive(true); // will be disabled by reset if needed.
	comboBoxId->get_parent()->hide();
	comboBoxId->set_active_id("");
	inputDevicePort->get_parent()->hide();
	inputDevicePort->set_text("");
	changePoint->get_parent()->hide();
	changePoint->set_value(DEFAULT_CHANGE_VALUE);
	spinnerLeds->get_parent()->hide();
	spinnerLeds->set_value(0.00);
	spinnerLeds->update();
	brief->set_text("");
}

void DialogDevice::onSelected() noexcept {

	const string name {selectorCombo->get_active_id()};

	const Defaults::DeviceInfo
		& oldInfo {Defaults::devicesInfo.at(previousName)},
		& newInfo {Defaults::devicesInfo.at(name)};

	// Detect variable pins or use device info.
	const uint16_t
		// Convert LEDs to Pins for spinner.
		oldPins   (oldInfo.variable ? spinnerLeds->get_value_as_int() * 3 : oldInfo.pins),
		newPins   {newInfo.variable ? oldPins : newInfo.pins},
		totalPins {newInfo.pins};

	const auto makeUid {[name](const std::string& id) {
		return Defaults::createHardwareUniqueId({{NAME, name}, {ID, id}});
	}};

	auto isUidSet {[this, &makeUid](const std::string& id) -> bool {
		return currentData->getCollectionHandler()->isIdSet(makeUid(id));
	}};

	const bool isIdUser {Defaults::isIdUser(name)};

	if (isIdUser) {

		Defaults::populateComboBoxWithIds(
			idListstore,
			Defaults::devicesInfo.at(name).maxIds,
			isUidSet,
			"Device Hardware Number",
			"Hardware #"
		);

	}
	if (Defaults::isVariable(name)) {
		// Convert Pins to LEDs
		spinnerLeds->get_adjustment()->set_upper(totalPins / 3);
		spinnerLeds->set_value(newPins / 3);
	}
	else {
		DialogElement::getInstance()->changeNumberOfPins(totalPins);
	}

	if (isIdUser)
		Defaults::selectFirstAvailableId(comboBoxId, newInfo.maxIds, isUidSet);

	DialogElement::getInstance()->handleLayoutChange(oldInfo, newInfo);
}
