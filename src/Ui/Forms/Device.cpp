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

Device::DeviceFields* Device::fields                  = nullptr;
LEDSpicerUI::Ui::DialogElement* Device::dialogElement = nullptr;
CollectionHandler* Device::devices                    = nullptr;

Device::Device(unordered_map<string, string>& data) :
	Form(data)
{
	// Populate device pins with empty class (this is needed for LOAD as well so the classes are created).
	Glib::ustring name(
		fieldsData.count(NAME) ?
			Glib::ustring(fieldsData.at(NAME)) :
			(not fields->comboBoxDevices->get_active_id().empty() ? fields->comboBoxDevices->get_active_id() : "")
	);

	if (not name.empty())
		for (int i = 1; i <= Defaults::devicesInfo.at(name).pins; i++)
			pinClasses.emplace(std::to_string(i), NO_COLOR);
	// Check load.
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}

	// Devices are created after a valid device name is selected.
	fieldsData[NAME] = fields->comboBoxDevices->get_active_id();
	onActivate();
	fields->comboBoxDevices->set_sensitive(true);
	fields->comboBoxId->set_active_id("");
	fields->changePoint->set_value(DEFAULT_CHANGE_VALUE);
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
	for (auto& d : Defaults::devicesInfo) {
		row = *(fields->devicesListstore->append());
		row.set_value(0, d.first);
		row.set_value(1, d.second.name);
		row.set_value(2, true);
	}
	/*
	 * When the device selector changes, update fields.
	 */
	fields->comboBoxDevices->signal_changed().connect([fields]() {
		const string name(fields->comboBoxDevices->get_active_id());
		if (name.empty()) {
			// reset to nothing.
			fields->comboBoxId->get_parent()->hide();
			fields->changePoint->get_parent()->hide();
			return;
		}
		if (Defaults::isMultiple(name))
			fields->comboBoxId->get_parent()->show();
		else
			fields->comboBoxId->get_parent()->hide();

		if (Defaults::isMonocrome(name))
			fields->changePoint->get_parent()->show();
		else
			fields->changePoint->get_parent()->hide();
	});
}

string const Device::createPrettyName() const {
	return Defaults::devicesInfo.at(fieldsData.at(NAME)).name + " Id: " + fieldsData.at(ID);
}

string const Device::createName() const {
	return fieldsData.at(NAME) + "_" + fieldsData.at(ID);
}

void Device::resetForm(Modes mode) {
	fields->comboBoxDevices->set_active(0);
}

void Device::isValid(Modes mode) {

	string
		name(fields->comboBoxDevices->get_active_id()),
		id(fields->comboBoxId->get_active_id());

	if (name.empty())
		throw Message("Invalid device\n");

	if (id.empty())
		throw Message("Invalid device number\n");

	// Is imposible to duplicate devices when Adding or editing because the selectors gets disable but LOAD can.
	if (mode == Modes::LOAD and devices->isUsed(name + "_" + id))
		throw Message("Devices " + Defaults::devicesInfo.at(name).name + " ID " + id + " already exists\n");
}

void Device::storeData(Modes mode) {

	string
		name  = fields->comboBoxDevices->get_active_id(),
		newId = fields->comboBoxId->get_active_id(),
		// only for edit:
		oldId = mode == Modes::EDIT ? createName() : "";
	// This will clean any anomaly.
	fieldsData.clear();
	fieldsData[NAME] = name;
	fieldsData[ID]   = newId;

	if (mode == Modes::EDIT)
		devices->replace(oldId, createName());
	else
		devices->add(createName());

	if (Defaults::isMonocrome(name))
		fieldsData[CHANGE_POINT] = std::to_string(static_cast<uint8_t>(fields->changePoint->get_value()));
	markDevicesUsed();
	onDeActivate();
}

void Device::retrieveData(Modes mode) {
	onActivate();
	const string name(fieldsData[NAME]);
	fields->comboBoxDevices->set_sensitive(false);
	fields->comboBoxDevices->set_active_id(name);
	fields->comboBoxId->set_active_id(fieldsData[ID]);
	if (Defaults::isMultiple(name))
		fields->comboBoxId->get_parent()->show();
	else
		fields->comboBoxId->get_parent()->hide();
	if (Defaults::isMonocrome(name)) {
		fields->changePoint->set_value(fieldsData.count(CHANGE_POINT) ? std::stod(fieldsData[CHANGE_POINT]) : DEFAULT_CHANGE_VALUE);
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
	if (Defaults::isMultiple(name)) {
		auto row = *(fields->idListstore->append());
		row.set_value(0, string());
		row.set_value(1, string("Select Device"));
		row.set_value(2, false);
		for (int c = 0; c < Defaults::devicesInfo.at(name).maxIds; ++c) {
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
	for (auto& di : Defaults::devicesInfo) {
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
}
