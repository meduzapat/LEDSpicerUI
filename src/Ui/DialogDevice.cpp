/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.cpp
 * @since     Feb 26, 2023
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

#include "DialogDevice.hpp"

using namespace LEDSpicerUI::Ui;

DialogDevice::DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Device Box and buttons.
	builder->get_widget_derived("BoxDevices", box);
	builder->get_widget("BtnAddDevice",       btnAdd);
	builder->get_widget("BtnApplyDevices",    btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogDevice::onAddClicked));
	setSignalApply();

	/// Pointer to button add element to enable or disable.
	Gtk::Button* BtnAddElement;
	builder->get_widget("BtnAddElement", BtnAddElement);

	// Device fields and models..
	builder->get_widget("ComboBoxDevices",  fields.comboBoxDevices);
	builder->get_widget("ComboBoxDeviceId", fields.comboBoxId);
	builder->get_widget("ScaleChangePoint", fields.changePoint);
	fields.devicesListstore = dynamic_cast<Gtk::ListStore*>(builder->get_object("ListstoreDevices").get());
	fields.idListstore      = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreDeviceId").get());
	Forms::Device::initialize(&fields);

	// When the dialog shows, do some clean up for new devices.
	signal_show().connect([&, BtnAddElement]() {
		if (mode == Forms::Form::Modes::ADD) {
			fields.comboBoxDevices->set_sensitive(true);
			fields.comboBoxDevices->set_active_id("");
			BtnAddElement->set_sensitive(false);
		}
		else {
			BtnAddElement->set_sensitive(true);
		}
	});

	fields.comboBoxDevices->signal_changed().connect([&, BtnAddElement]() {
		// Setup only, edit or load.
		if (mode != Forms::Form::Modes::ADD)
			return;
		// Add.
		if (currentForm) {
			delete currentForm;
			currentForm = nullptr;
			DialogElement::getInstance()->wipeContents();
		}
		if (fields.comboBoxDevices->get_active_id().empty()) {
			BtnAddElement->set_sensitive(false);
			btnApply->set_sensitive(false);
		}
		else {
			BtnAddElement->set_sensitive(true);
			btnApply->set_sensitive(true);
			unordered_map<string, string> rawData;
			currentForm = getForm(rawData);
		}
	});
/*
		// Sets the device options disable if there is no more available devices of that type.
		auto model     = comboBoxDevices->get_model();
		auto children  = model->children();
		for (auto iter = children.begin(); iter != children.end(); ++iter) {
			auto child = *iter;
			string val;
			child->get_value(0, val);
			if (val == "") {
				continue;
			}
			child->set_value(2, Forms::Device::isAvailable(val));
		}
		if (mode == Forms::Form::Modes::EDIT) {
			// this prevents the onchange to fail, because device have no access to the combobox.
			comboBoxDevices->set_active(0);
			comboBoxDevices->set_active_id(currentForm->getValue(NAME));
			return;
		}
		// completes initialization for add.
		comboBoxDevices->set_active(0);
	});*/
}

Forms::Form* DialogDevice::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Device(rawData);
}

string DialogDevice::getType() {
	return "device";
}
