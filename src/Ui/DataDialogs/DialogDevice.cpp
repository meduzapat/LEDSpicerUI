/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.cpp
 * @since     Feb 26, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

DialogDevice* DialogDevice::instance = nullptr;

void DialogDevice::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogDevice", instance);
	}
}

DialogDevice* DialogDevice::getInstance() {
	return instance;
}

DialogDevice::DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Device Box and buttons.
	builder->get_widget_derived("BoxDevices", box);
	builder->get_widget("BtnAddDevice",       btnAdd);
	builder->get_widget("BtnApplyDevices",    btnApply);
	setSignalAdd();
	setSignalApply();

	/// Pointer to button add element to enable or disable.
	Gtk::Button* btnAddElement;
	builder->get_widget("BtnAddElement", btnAddElement);

	// Device fields and models..
	builder->get_widget("ComboBoxDevices",        comboBoxDevices);
	builder->get_widget("ComboBoxDeviceId",       comboBoxId);
	builder->get_widget("ScaleDeviceChangePoint", changePoint);
	builder->get_widget("SpinnerDeviceLeds",      spinnerLeds);
	builder->get_widget("InputDevicePort",        inputDevicePort);
	builder->get_widget("BriefDevice",            brief);

	Gtk::Notebook* notebookDeviceConnections;
	builder->get_widget("NotebookDeviceConnections", notebookDeviceConnections);

	devicesListstore = dynamic_cast<Gtk::ListStore*>(builder->get_object("ListstoreDevices").get());
	idListstore      = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreDeviceId").get());

	// Set Devices
	auto row = *(devicesListstore->append());
	row.set_value(0, string());
	row.set_value(1, string("Select Device"));
	row.set_value(2, false);
	for (auto& d : Defaults::devicesInfo) {
		row = *(devicesListstore->append());
		row.set_value(0, d.first);
		row.set_value(1, d.second.name);
		row.set_value(2, true);
	}

	/*
	 * On Number of LEDs is SET apply the changes to the Elements.
	 */
	spinnerLeds->signal_value_changed().connect([&, btnAddElement]() {
		const string name(comboBoxDevices->get_active_id());
		if (not name.empty() and not Defaults::isVariable(name)) {
			return;
		}
		btnAddElement->set_sensitive(spinnerLeds->get_value_as_int() > 1);
		const uint16_t pinsCount(spinnerLeds->get_value_as_int() * 3);
		if (comboBoxDevices->get_active_id().empty() or not pinsCount)
			return;
		DataDialogs::DialogElement::getInstance()->changeNumberOfPins(pinsCount);
	});

	/*
	 * On Number of LEDs is changed, update the add button.
	 */
	spinnerLeds->signal_changed().connect([&, btnAddElement]() {
		btnAddElement->set_sensitive(spinnerLeds->get_value_as_int() > 1);
	});

	comboBoxDevices->signal_changed().connect([&, btnAddElement, notebookDeviceConnections]() {
		const string name(comboBoxDevices->get_active_id());
		// If current ID is empty just reset the form for Add task.
		if (name.empty()) {
			clearForm();
			btnAddElement->set_sensitive(false);
			btnApply->set_sensitive(false);
			previousName = "";
			return;
		}

		// If the name is the same do nothing.
		if (previousName == name) {
			return;
		}

		const uint16_t totalPins(Defaults::devicesInfo.at(name).pins);
		const string currentName(currentData->getValue(NAME));
		// For loading or non stored data, do a change device without asking for losing the data.
		if (mode == Modes::LOAD or currentName.empty()) {
			// Destroy device but keep the pointer.
			currentData->destroy();
			clearForm();
		}
		// If there is a device already, clean the form and change to the new device, warn the user about losing data.
		else if (name != currentName) {
			if (Message::ask("Are you sure you want to change the device? all settings will be loss") == Gtk::RESPONSE_YES) {
				// Destroy device but keep the pointer.
				currentData->destroy();
				clearForm();
			}
			else {
				// If the user decide to keep the data, move the selection to the previous selected device.
				comboBoxDevices->set_active_id(previousName);
				return;
			}
		}

		previousName = name;

		if (Defaults::isIdUser(name)) {
			Defaults::populateComboBoxWithIds(
				idListstore,
				Defaults::devicesInfo.at(name).maxIds,
				[=](const string& id) {
					return devicesHandler->isUsed(Defaults::createHardwareUniqueId({{NAME, name}, {ID, id}}));
				},
				"Device Hardware Number",
				"Hardware #"
			);
			comboBoxId->get_parent()->show();
		}
		if (Defaults::isSerial(name)) {
			inputDevicePort->get_parent()->show();
		}

		if (Defaults::isMonochrome(name)) {
			changePoint->get_parent()->show();
		}

		if (Defaults::isVariable(name)) {
			spinnerLeds->get_parent()->show();
			spinnerLeds->get_adjustment()->set_upper(totalPins);
			// assume all variable hardware only supports RGB leds, for now.

			// if there are elements and is not
			btnAddElement->set_sensitive(not currentData->getValue(PINS).empty());
		}
		else {
			btnAddElement->set_sensitive(true);
			DataDialogs::DialogElement::getInstance()->changeNumberOfPins(totalPins);
		}

		// Special Features.
		DataDialogs::DialogElement::getInstance()->setRules(
			Defaults::isMonochrome(name),
			Defaults::devicesInfo.at(name).layoutRGB,
			Defaults::devicesInfo.at(name).supportStrip
		);
		brief->set_text(Defaults::devicesInfo.at(name).brief);
		btnApply->set_sensitive(true);
	});
}

void DialogDevice::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_DEVICES), values);
}

void DialogDevice::createSubItems(XMLHelper* values) {
	DialogElement::getInstance()->load(values);
}

void DialogDevice::resetForm() {
	// reset to nothing.
	comboBoxDevices->set_active_id("");
	clearForm();
}

void DialogDevice::clearForm() {
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
	DataDialogs::DialogElement::getInstance()->changeNumberOfPins(0);
	refreshBox();
}

void DialogDevice::isValid() const {

	string
		name(comboBoxDevices->get_active_id()),
		id(comboBoxId->get_active_id()),
		port(inputDevicePort->get_text());

	bool checkDupe = true;

	if (name.empty()) {
		throw Message("Invalid device");
	}

	/* TODO: In some cases it is needed to check how many serial devices are set.
	 * if (Defaults::isSerial(name)) {
		// Serial can be empty only once per device.
		if (port.empty()) {
			inputDevicePort->grab_focus();
			throw Message("You need to enter a serial port\n");
		}
	}*/

	if (Defaults::isIdUser(name) and id.empty()) {
		throw Message("Invalid device number");
	}

	if (Defaults::isVariable(name)) {
		string pins(spinnerLeds->get_text());
		if (not Defaults::isBetween(pins, 1, Defaults::devicesInfo.at(name).pins)) {
			if (mode != Modes::LOAD)
				spinnerLeds->grab_focus();
			throw Message("The number of pins need to be between one and the number of pins the device allows (" + std::to_string(Defaults::devicesInfo.at(name).pins) + ")");
		}
	}

	string
		newName(createUniqueId()),
		deviceName("Device " + Defaults::devicesInfo.at(name).name);
	if (mode == Modes::EDIT) {
		checkDupe = (currentData->createUniqueId() != newName);
	}

	if (Defaults::isIdUser(name) and checkDupe and devicesHandler->isUsed(newName)) {
		throw Message(deviceName + " ID " + id + " already exists");
	}
	if (Defaults::isSerial(name) and checkDupe and devicesHandler->isUsed(newName)) {
		throw Message(deviceName + " that connects to " + (port.empty() ? "<autodetect>" : port) + " already exists");
	}
	if (checkDupe and devicesHandler->isUsed(newName)) {
		throw Message(deviceName + " already exists");
	}
}

void DialogDevice::storeData() {
	if (mode == Modes::EDIT) {
		devicesHandler->replace(currentData->createUniqueId(), createUniqueId());
	}
	else {
		devicesHandler->add(createUniqueId());
	}

	// This will clean any anomaly.
	currentData->wipe();

	const string name(comboBoxDevices->get_active_id());

	currentData->setValue(NAME, name);
	if (Defaults::isIdUser(name)) {
		currentData->setValue(ID, comboBoxId->get_active_id());
	}

	if (Defaults::isSerial(name)) {
		currentData->setValue(PORT, inputDevicePort->get_text());
	}

	if (Defaults::isVariable(name)) {
		currentData->setValue(PINS, spinnerLeds->get_text());
	}

	if (Defaults::isMonochrome(name)) {
		currentData->setValue(CHANGE_POINT, std::to_string(static_cast<uint8_t>(changePoint->get_value())));
	}
	DataDialogs::DialogElement::getInstance()->reindex();
}

void DialogDevice::retrieveData() {

	const string name(currentData->getValue(NAME));

	comboBoxDevices->set_active_id(name);

	if (Defaults::isIdUser(name)) {
		comboBoxId->set_active_id(currentData->getValue(ID));
	}
	if (Defaults::isSerial(name)) {
		inputDevicePort->set_text(currentData->getValue(PORT));
	}
	if (Defaults::isVariable(name)) {
		spinnerLeds->set_text(currentData->getValue(PINS));
	}
	if (Defaults::isMonochrome(name)) {
		changePoint->set_value(std::stod(currentData->getValue(CHANGE_POINT, std::to_string(DEFAULT_CHANGE_VALUE))));
	}
	markDevicesUsed();
}

string const DialogDevice::createUniqueId() const {
	return Defaults::createHardwareUniqueId({
		{NAME, comboBoxDevices->get_active_id()},
		{ID,   comboBoxId->get_active_id()},
		{PORT, inputDevicePort->get_text()}
	});
}

const string DialogDevice::getType() const {
	return "device";
}

LEDSpicerUI::Ui::Storage::Data* DialogDevice::getData(unordered_map<string, string>& rawData) {
	return new Storage::Device(rawData);
}

void DialogDevice::markDevicesUsed() {
	for (auto& di : Defaults::devicesInfo) {
		const uint total    = devicesHandler->count(di.first);
		const auto& children = devicesListstore->children();

		for (auto iter = children.begin(); iter != children.end(); ++iter) {
			Gtk::TreeModel::Row dataRow = *iter;

			string data;
			dataRow.get_value(0, data);
			if (data == di.first) {
				dataRow.set_value(2, total < di.second.maxIds);
				break;
			}
		}
	}
}
