/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputSource.cpp
 * @since     Apr 20, 2025
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

#include "DialogInputSource.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInputSource* DialogInputSource::instance = nullptr;

void DialogInputSource::initialize(Glib::RefPtr<Gtk::Builder> const& builder) {
	if (not instance) {
		builder->get_widget_derived("DialogInputSource", instance);
	}
}

DialogInputSource* DialogInputSource::getInstance() {
	return instance;
}

DialogInputSource::DialogInputSource(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	builder->get_widget_derived("BoxInputSources", box);
	builder->get_widget("BtnApplyInputSource",     btnApply);
	builder->get_widget("ComboBoxInputSource",     comboBoxInputSource);
	builder->get_widget("EntryInputSource",        entryInputSource);

	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddInputSource", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Show manual entry only when "Other" is selected.
	comboBoxInputSource->signal_changed().connect([&]() {
		bool isOther(comboBoxInputSource->get_active_text() == OTHER_OPTION);
		entryInputSource->set_visible(isOther);
		if (isOther) {
			entryInputSource->grab_focus();
		}
	});
}

void DialogInputSource::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_INPUT_EVENTS), values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInputSource::getCollectionHandler() const {
	return LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_INPUT_EVENTS);
}

void DialogInputSource::resetForm() {
	// Only scan when running on the target system.
	if (Defaults::getMode() != Defaults::Mode::Portable) {
		applyDeviceList(scanEventDevices());
	}
	else {
		comboBoxInputSource->hide();
		entryInputSource->show();
	}
	clearForm();
}

void DialogInputSource::clearForm() {
	entryInputSource->set_text("");
	if (comboBoxInputSource->is_visible()) {
		comboBoxInputSource->set_active(0);
	}
}

void DialogInputSource::isValid() const {
	if (createUniqueId().empty()) {
		if (comboBoxInputSource->is_visible()) {
			comboBoxInputSource->grab_focus();
		}
		else {
			entryInputSource->grab_focus();
		}
		throw Message("Enter a valid event source name.");
	}
}

void DialogInputSource::storeData() {

}

void DialogInputSource::retrieveData() {
	string source(currentData->createUniqueId());

	if (comboBoxInputSource->is_visible()) {
		// Try to find the stored source in the combo.
		comboBoxInputSource->set_active_text(source);
		// Not in the list — fall back to "Other" + manual entry.
		if (comboBoxInputSource->get_active_row_number() < 0) {
			comboBoxInputSource->set_active_text(OTHER_OPTION);
			entryInputSource->set_text(source);
		}
	}
	else {
		entryInputSource->set_text(source);
	}
}

const string DialogInputSource::createUniqueId() const {
	if (comboBoxInputSource->is_visible()) {
		string selected(comboBoxInputSource->get_active_text());
		if (selected == OTHER_OPTION or selected.empty()) {
			return entryInputSource->get_text();
		}
		return selected;
	}
	return entryInputSource->get_text();
}

void DialogInputSource::createSubItems(XMLHelper*) {

}

const string DialogInputSource::getType() const {
	return "InputSource";
}

LEDSpicerUI::Ui::Storage::Data* DialogInputSource::createData(StringUMap&) {
	return nullptr;
}

StringVector DialogInputSource::scanEventDevices() {
	StringVector devices;

	auto directory = Gio::File::create_for_path(DEV_INPUT);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;

	try {
		enumerator = directory->enumerate_children("standard::name,standard::type");
	}
	catch (const Glib::Error&) {
		// /dev/input/ not accessible — portable environment.
		return devices;
	}

	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		string name(fileInfo->get_name());
		if (name.find("event") != string::npos) {
			devices.push_back(name);
		}
	}

	std::sort(devices.begin(), devices.end());
	return devices;
}

void DialogInputSource::applyDeviceList(const StringVector& devices) {
	comboBoxInputSource->remove_all();

	if (devices.empty()) {
		// Local mode but no event devices found — degrade to entry.
		comboBoxInputSource->hide();
		entryInputSource->show();
		return;
	}

	for (const auto& device : devices) {
		comboBoxInputSource->append(device);
	}
	comboBoxInputSource->append(OTHER_OPTION);

	comboBoxInputSource->show();
	comboBoxInputSource->set_active(0);
	entryInputSource->hide();
}
