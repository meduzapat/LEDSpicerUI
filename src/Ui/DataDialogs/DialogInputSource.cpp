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
	builder->get_widget_derived("BoxInputSources",  box);
	builder->get_widget("BtnApplyInputSource",      btnApply);
	builder->get_widget("ComboBoxInputSource",      comboBoxInputSource);
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput);
	builder->get_widget("EntryInputSource",         entryInputSource);
	builder->get_widget("BtnAddInputSourceMap",     btnAddMap);

	Gtk::Button * btnAdd = nullptr;
	builder->get_widget("BtnAddInputSource", btnAdd);

	setSignalAdd(btnAdd);
	setSignalApply();

	childDialog = DialogInputMap::getInstance();

	comboBoxInputSource->signal_changed().connect([this]() {

		string selected(comboBoxInputSource->get_active_text());

		btnAddMap->set_sensitive(false);
		entryInputSource->get_parent()->set_visible(false);

		if (selected == SOURCE_EMPTY_OPTION) return;

		if (selected == SOURCE_OTHER_OPTION) {
			entryInputSource->get_parent()->set_visible(true);
			entryInputSource->grab_focus();
		}
		else {
			btnAddMap->set_sensitive(true);
		}
	});

	entryInputSource->signal_changed().connect([this]() {
		btnAddMap->set_sensitive(not entryInputSource->get_text().empty());
	});
}

void DialogInputSource::load(XMLHelper* values) {
	createItems(
		values->getData(Defaults::createCommonUniqueId({ownerData->createUniqueId(), COLLECTION_INPUT_SOURCES})),
		values
	);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInputSource::getCollectionHandler() const {
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES);
}

void DialogInputSource::resetForm() {
	comboBoxInputSource->set_active(-1);
	clearForm();
}

void DialogInputSource::clearForm() {

	comboBoxInputSource->get_parent()->hide();
	entryInputSource->get_parent()->hide();

	if (Defaults::needSource(comboBoxInputSelectInput->get_active_text())) {
		populateSourcesList(scanEventDevices());
	}

	btnAddMap->set_sensitive(false);
	entryInputSource->set_text("");
}

void DialogInputSource::isValid() const {

	if (Defaults::needSource(comboBoxInputSelectInput->get_active_text())) {
		if (createUniqueId().empty())
			throw Message("Enter a valid event source name.");
	}
}

void DialogInputSource::storeData() {
	currentData->setValue(SOURCE, createUniqueId());
}

void DialogInputSource::retrieveData() {

	if (not Defaults::needSource(comboBoxInputSelectInput->get_active_text())) {
		return;
	}

	string source(currentData->getValue(SOURCE));

	// Try to find the stored source in the combo.
	comboBoxInputSource->set_active_text(source);
	// Not in the list — fall back to "Other" + manual entry.
	if (comboBoxInputSource->get_active_row_number() < 0) {
		comboBoxInputSource->set_active_text(SOURCE_OTHER_OPTION);
		entryInputSource->set_text(source);
	}
}

const string DialogInputSource::createUniqueId() const {
	if (comboBoxInputSource->is_visible()) {
		string selected(comboBoxInputSource->get_active_text());
		if (selected == SOURCE_OTHER_OPTION or selected.empty()) {
			return entryInputSource->get_text();
		}
		return selected;
	}
	return entryInputSource->get_text();
}

void DialogInputSource::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
}

const string DialogInputSource::getType() const {
	return "source";
}

LEDSpicerUI::Ui::Storage::Data* DialogInputSource::createData(StringUMap& rawData) {
	// Inject input PK and index as properties so InputSource::createUniqueId() works standalone.
	rawData[INPUT_PK] = ownerData->createUniqueId();
	rawData[INDEX]    = std::to_string(items->getSize());
	return new Storage::InputSource(rawData);
}

StringVector DialogInputSource::scanEventDevices() {

	StringVector devices;
	if (
		// Portable will use other system inputs that most likely won't be compatible with this.
		Defaults::getMode() == Defaults::Mode::Portable or
		// Non event sources won't be listed here.
		not Defaults::isDevInputListener(comboBoxInputSelectInput->get_active_text())
	) {
		return devices;
	}

	auto directory = Gio::File::create_for_path(DEV_INPUT);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;

	try {
		enumerator = directory->enumerate_children("standard::name,standard::type");
	}
	catch (const Glib::Error&) {
		// /dev/input/ not accessible.
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

void DialogInputSource::populateSourcesList(const StringVector& devices) {

	comboBoxInputSource->remove_all();

	// Local mode but no sources detected.
	if (devices.empty()) {
		entryInputSource->get_parent()->show();
		entryInputSource->grab_focus();
		return;
	}

	comboBoxInputSource->get_parent()->show();
	comboBoxInputSource->append(SOURCE_EMPTY_OPTION);

	for (const auto& device : devices) {
		comboBoxInputSource->append(device);
	}
	comboBoxInputSource->append(SOURCE_OTHER_OPTION);
	comboBoxInputSource->set_active_text(SOURCE_EMPTY_OPTION);
}
