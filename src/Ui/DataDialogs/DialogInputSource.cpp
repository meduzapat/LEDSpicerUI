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

DialogInputSource::DialogInputSource(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	Gtk::Button
		* btnAdd          = nullptr,
		* btnAddMapSingle = nullptr; // From Input Dialog.

	/*
	 * Register before initializing child dialog to set fake source creation.
	 * DialogInputMap will create add button.
	 */
	builder->get_widget("BtnAddInputMap", btnAddMapSingle);
	btnAddMapSingle->signal_clicked().connect([this]() {
		createButtonDirectly();
	});

	DataDialogs::DialogInputMap::buildInstance(builder, "DialogInputMap");

	builder->get_widget_derived("BoxInputSources",  box);
	builder->get_widget("BtnApplyInputSource",      btnApply);
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput); // from Input Dialog.
	builder->get_widget("ComboBoxInputSource",      comboBoxInputSource);
	builder->get_widget("EntryInputSource",         entryInputSource);
	builder->get_widget("BtnAddInputSource",        btnAdd);
	builder->get_widget("BtnAddInputSourceMap",     btnAddMap); // From Input Map Dialog.

	setSignalAdd(btnAdd);
	setSignalApply();

	childDialogs.push_back(DialogInputMap::getInstance());

	comboBoxInputSource->signal_changed().connect([this]() {

		string selected(comboBoxInputSource->get_active_id());

		btnAddMap->set_sensitive(false);
		entryInputSource->get_parent()->set_visible(false);

		if (selected.empty()) return;

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

DialogInputSource::~DialogInputSource() {
	delete DataDialogs::DialogInputMap::getInstance();
}

void DialogInputSource::load(XMLHelper* values) {
	createItems(
		values->getData(Defaults::createCommonUniqueId({ownerData->createUniqueId(), COLLECTION_INPUT_SOURCES})),
		values
	);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInputSource::getCollectionHandler() const {
	return Storage::CollectionHandler::getInstance(
		// local to it's input.
		COLLECTION_INPUT_SOURCES + ownerData->getProperty(FILE_ID)
	);
}

void DialogInputSource::resetForm() {
	comboBoxInputSource->set_active(-1);
	clearForm();
}

void DialogInputSource::clearForm() {

	comboBoxInputSource->get_parent()->hide();
	entryInputSource->get_parent()->hide();

	if (Defaults::needSource(comboBoxInputSelectInput->get_active_id())) {
		populateSourcesList(scanEventDevices());
	}

	btnAddMap->set_sensitive(false);
	btnApply->set_sensitive(false);
	entryInputSource->set_text("");
}

void DialogInputSource::isValid() const {

	if (Defaults::needSource(comboBoxInputSelectInput->get_active_id())) {
		if (createUniqueId().empty())
			throw Message("Enter a valid event source name.");
	}
}

void DialogInputSource::storeData() {
	currentData->setValue(SOURCE, createUniqueId());
}

void DialogInputSource::retrieveData() {

	if (not Defaults::needSource(comboBoxInputSelectInput->get_active_id())) {
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
	string selected{comboBoxInputSource->get_active_id()};
	if (selected == SOURCE_OTHER_OPTION or selected.empty()) selected = entryInputSource->get_text();
	return Defaults::createCommonUniqueId({ownerData->createUniqueId(), selected});
}

void DialogInputSource::createButtonDirectly() {
	if (items->getSize() > 0) {
		(*items->begin())->getData()->activate();
		return;
	}
	StringUMap rawData;
	// Injects INPUT_PK and INDEX=0.
	auto* phantom = createData(rawData);
	// Wires DialogInputMap to phantom's maps.
	phantom->activate();
	items->create(phantom);
	getCollectionHandler()->add(phantom);
}

void DialogInputSource::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
}

const string DialogInputSource::getType() const {
	return "source";
}

LEDSpicerUI::Ui::Storage::Data* DialogInputSource::createData(StringUMap& rawData) {
	auto* is = new Storage::InputSource(rawData);
	is->setProperty(FILE_ID, ownerData->createUniqueId());
	return is;
}

StringMap DialogInputSource::scanEventDevices() {

	StringMap devices;

	if (
		Defaults::getMode() == Defaults::Mode::Portable or
		not Defaults::isDevInputListener(comboBoxInputSelectInput->get_active_id())
	) {
		return devices;
	}

	// Helper lambda to add devices from a directory.
	auto addDevices = [&](const std::filesystem::path& dirPath, bool useById) {
//		if (not std::filesystem::exists(dirPath) and not std::filesystem::is_directory(dirPath)) return;

		for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {

			if (not entry.is_regular_file() and not entry.is_symlink()) continue;

			string name = entry.path().filename().string();
			if (name.empty() or name[0] == '.' or (useById and name.find("event") == string::npos)) continue;

			string humanName = readDeviceName(name);
			if (humanName.empty()) humanName = name;

			devices.emplace(name, humanName);
		}
	};

	// Prefer /dev/input/by-id.
	addDevices(DEV_INPUT_BY_ID, true);
	if (!devices.empty()) return devices;

	// Fallback: /dev/input/event*.
	addDevices(DEV_INPUT, false);

	return devices;
}

void DialogInputSource::populateSourcesList(const StringMap& devices) {

	comboBoxInputSource->remove_all();

	if (devices.empty()) {
		// No devices detected — show manual entry directly.
		entryInputSource->get_parent()->show();
		entryInputSource->grab_focus();
		return;
	}

	comboBoxInputSource->get_parent()->show();
	comboBoxInputSource->append("", SOURCE_EMPTY_OPTION);

	for (const auto& [id, display] : devices)
		comboBoxInputSource->append(id, display);

	comboBoxInputSource->append(SOURCE_OTHER_OPTION, SOURCE_OTHER_OPTION);
	comboBoxInputSource->set_active_id("");
}

string DialogInputSource::readDeviceName(const string& byIdName) {
	std::filesystem::path link_path{DEV_INPUT_BY_ID + byIdName};
	if (not std::filesystem::exists(link_path) or not std::filesystem::is_symlink(link_path)) {
		return byIdName;
	}

	std::error_code ec;
	auto target = std::filesystem::read_symlink(link_path, ec);
	if (ec) return byIdName;

	string eventName = target.filename().string();

	std::filesystem::path sysPath{SYS_CLASS_INPUT + eventName + "/device/name"};
	std::ifstream file(sysPath);
	if (not file) return byIdName;

	string name;
	std::getline(file, name);

	Defaults::rtrim(name);

	return name;
}
