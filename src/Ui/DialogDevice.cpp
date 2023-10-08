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
			// Switching devices.
			delete currentForm;
			currentForm = nullptr;
			DialogElement::getInstance()->wipeContents();
		}
		if (fields.comboBoxDevices->get_active_id().empty()) {
			// reset to nothing.
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
}

string DialogDevice::toXml() {
	string r(Defaults::tab() + "<devices>\n");
	Defaults::increaseTab();
	mode = Forms::Form::Modes::LOAD;
	for (auto b : items) {
		// load elements
		b->getForm()->retrieveData(mode);
		r += b->toXML(DialogElement::getInstance()->toXml());
		// unload elements
		b->getForm()->storeData(mode);
	}
	Defaults::reduceTab();
	return (r + Defaults::tab() + "</devices>\n");
}

Forms::Form* DialogDevice::getForm(unordered_map<string, string>& rawData) {
	if (mode == Forms::Form::Modes::ADD and fields.comboBoxDevices->get_active_id().empty())
		return nullptr;
	else
		return new Forms::Device(rawData);
}

string DialogDevice::getType() {
	return "device";
}
