/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	  Device.cpp
 * @since	 Feb 26, 2023
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

#include "Device.hpp"

using namespace LEDSpicerUI::Ui::Forms;

LEDSpicerUI::Ui::Forms::Device::DeviceFields* Device::fields = nullptr;

LEDSpicerUI::Ui::DialogElement* Device::dialogElement = nullptr;

LEDSpicerUI::Ui::Forms::CollectionHandler* Device::devices = nullptr;

const unordered_map<string, Device::DeviceInfo> Device::devicesInfo = {
//        ID                        NAME                MI  B/W    ML
	{"UltimarcUltimate", {"Ultimarc Ipac Ultimate IO",   4, false, 96}},
	{"UltimarcPacLed64", {"Ultimarc PacLed 64",          4, false, 64}},
	{"UltimarcPacDrive", {"Ultimarc Pac Drive",          4, true,  16}},
	{"UltimarcNanoLed",  {"Ultimarc NanoLed",            4, false, 60}},
	{"LedWiz32",         {"Groovy Game Gear Led-Wiz 32", 4, false, 32}},
	{"Howler",           {"Wolfware Howler",             4, false, 96}},
	{"Adalight",         {"Adalight (ESP8266)",          1, false, 60}},
	{"RaspberryPi",      {"Raspberry Pi GPIO pins",      1, false, 28}}
};

Device::Device(unordered_map<string, string>& data) :
	Form(data)
{
	// Check load.
	if (not fieldsData.empty()) {
		retrieveData(Forms::Form::Modes::LOAD);
		isValid(Forms::Form::Modes::LOAD);
		storeData(Forms::Form::Modes::LOAD);
		devices->add(createName());
		return;
	}

	// Devices are created after a valid device name is selected.
	fieldsData[NAME] = fields->comboBoxDevices->get_active_id();
	// Populate device pins with empty class.
	for (int i = 1; i <= devicesInfo.at(fieldsData[NAME]).pins; i++)
		pinClasses.emplace(std::to_string(i), NO_COLOR);
	onActivate();
	fields->changePoint->set_value(DEFAULT_CHANGE_VALUE);
	fields->comboBoxId->set_active_id("");
	fields->comboBoxDevices->set_sensitive(true);
}

Device::~Device() {
	// if destroyed mark device and id available.
	if (fieldsData.count(NAME) and fieldsData.count(ID)) {
		devices->remove(createName());
		markDevicesUsed();
	}
	// Remove all boxbutton.
	elements.wipe();
}

void Device::initialize(DeviceFields* fields) {

	Device::fields = fields;
	dialogElement  = DialogElement::getInstance();
	devices        = CollectionHandler::getInstance("devices");

	// Set Devices
	auto row = *(fields->devicesListstore->append());
	row.set_value(0, string());
	row.set_value(1, string("Select Device"));
	row.set_value(2, false);
	for (auto& d : devicesInfo) {
		row = *(fields->devicesListstore->append());
		row.set_value(0, d.first);
		row.set_value(1, d.second.name);
		row.set_value(2, true);
	}
	/*
	 * When the device selector changes, update fields.
	 */
	fields->comboBoxDevices->signal_changed().connect([fields]() {
		auto name = fields->comboBoxDevices->get_active_id();
		// empty Name is reset.
		if (name.empty()) {
			fields->comboBoxId->get_parent()->hide();
			fields->changePoint->get_parent()->hide();
			return;
		}
		if (isMultiple(name))
			fields->comboBoxId->get_parent()->show();
		else
			fields->comboBoxId->get_parent()->hide();

		if (isMonocrome(name))
			fields->changePoint->get_parent()->show();
		else
			fields->changePoint->get_parent()->hide();
	});
}

bool Device::isMultiple(const string& name) {
	return (devicesInfo.at(name).maxIds > 1);
}

bool Device::isMonocrome(const string& name) {
	return (devicesInfo.at(name).monochrome);
}

string const Device::createPrettyName() const {
	return devicesInfo.at(fieldsData.at(NAME)).name + " Id: " + fieldsData.at(ID);
}

string const Device::createName() const {
	return fieldsData.at(NAME) + "_" + fieldsData.at(ID);
}

void Device::isValid(Modes mode) {

	string
		name(fields->comboBoxDevices->get_active_id()),
		id(fields->comboBoxId->get_active_id());

	if (name.empty())
		throw Message("Select a valid device");

	if (id.empty())
		throw Message("Select a valid device number");

	// Is imposible to duplicate devices because the selectors gets disable but LOAD can.
	if (mode == Modes::LOAD and devices->isUsed(name + "_" + id))
		throw Message("Devices " + devicesInfo.at(name).name + " ID " + id + " already exists");
}

void Device::storeData(Modes mode) {

	onDeActivate();
	string
		name  = fields->comboBoxDevices->get_active_id(),
		newId = fields->comboBoxId->get_active_id(),
		// only for edit:
		oldId = mode == Modes::EDIT ? createName() : "";

	fieldsData[NAME] = name;
	fieldsData[ID]   = newId;

	if (mode == Modes::EDIT)
		devices->replace(oldId, createName());
	else
		devices->add(createName());

	if (isMonocrome(name))
		fieldsData[CHANGE_POINT] = std::to_string(fields->changePoint->get_value());
	markDevicesUsed();
}

void Device::retrieveData(Modes mode) {
	onActivate();
	fields->comboBoxDevices->set_sensitive(false);
	fields->comboBoxDevices->set_active_id(fieldsData[NAME]);
	fields->comboBoxId->set_active_id(fieldsData[ID]);
	if (isMultiple(fieldsData[NAME]))
		fields->comboBoxId->get_parent()->show();
	else
		fields->comboBoxId->get_parent()->hide();
	if (isMonocrome(fieldsData[NAME])) {
		fields->changePoint->set_value(std::stod(fieldsData[CHANGE_POINT]));
		fields->changePoint->get_parent()->show();
	}
	else {
		fields->changePoint->get_parent()->hide();
	}
}

void Device::cancelData(Modes mode) {
	onDeActivate();
}

string const Device::getCssClass() const {
	return "DeviceBoxButton";
}

void Device::onDeActivate() {
	dialogElement->getItems(elements);
}

void Device::onActivate() {
	processIdComboBox();
	PinHandler::getInstance()->setCurrentDevicePins(&pinClasses);
	dialogElement->setItems(elements);
}

void Device::processIdComboBox() {
	// clean id combobox.
	auto children = fields->idListstore->children();
	std::vector<Gtk::TreeIter> rowsToRemove;
	for (auto iter = children.begin(); iter != children.end(); ++iter)
		rowsToRemove.push_back(*iter);
	for (const auto& iter : rowsToRemove)
		fields->idListstore->erase(*iter);
	const string& name = fieldsData.at(NAME);
	if (isMultiple(name)) {
		auto row = *(fields->idListstore->append());
		row.set_value(0, string());
		row.set_value(1, string("Select Device"));
		row.set_value(2, false);
		for (int c = 0; c < devicesInfo.at(name).maxIds; ++c) {
			auto row = *(fields->idListstore->append());
			auto id(std::to_string(c + 1));
			row.set_value(0, id);
			row.set_value(1, "Board #" + id);
			row.set_value(2, not devices->isUsed(name + "_" + id));
		}
	}
	else {
		// Single Elements always use id 1.
		auto row = *(fields->idListstore->append());
		string s("1");
		row.set_value(0, s);
		row.set_value(1, s);
		row.set_value(2, true);
		fields->comboBoxId->set_active_id("1");
	}
}

void Device::markDevicesUsed() {
	for (auto& di : devicesInfo) {
		int total = 0;
		// count used devices.
		for (auto& d : *devices)
			if (d.find(di.first) != d.npos)
				++total;
		auto children = fields->devicesListstore->children();
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
	/*
	 // Mark new Id used
	auto a = idComboBox->get_active();
	a->set_value(2, false);
	 */
}
