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
	DialogFormHost(obj, builder)
{

	// From Input Dialog — sourceless path.
	Gtk::Button* btnAdd = nullptr;
	DataDialogs::DialogInputMap::buildInstance(builder, "DialogInputMap");

	builder->get_widget_derived("BoxInputSources",  box);
	builder->get_widget("BtnApplyInputSource",      btnApply);
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput);
	builder->get_widget("ComboBoxInputSource",      comboBoxInputSource);
	builder->get_widget("EntryInputSource",         entryInputSource);
	builder->get_widget("BtnAddInputSource",        btnAdd);
	builder->get_widget("BtnAddInputSourceMap",     btnAddMap);

	setSignalAdd(btnAdd);
	setSignalApply();

	childDialogs.push_back(DialogInputMap::getInstance());

	static const string question {"Are you sure you want to change the source? All maps will be lost."};

	comboBoxInputSource->signal_changed().connect([this]() {

		switch (
			handleTypeSwitch(
				comboBoxInputSource,
				DialogInputMap::getInstance()->getBox(),
				question
			)
		) {
		case TypeResult::Empty:
			btnApply->set_sensitive(false);
			btnAddMap->set_sensitive(false);
			return;
		case TypeResult::Unchanged:
			return;
		case TypeResult::Proceed:
			break;
		}

		if (comboBoxInputSource->get_active_id() == SOURCE_OTHER_OPTION) {
			entryInputSource->get_parent()->set_visible(true);
			entryInputSource->grab_focus();
			return;
		}
		entryInputSource->get_parent()->set_visible(false);
	});

	entryInputSource->signal_changed().connect([this]() {
		switch (
			handleTypeChange(
				entryInputSource,
				DialogInputMap::getInstance()->getBox(),
				question
			)
		) {
		case TypeResult::Empty:
			btnApply->set_sensitive(false);
			return;
		case TypeResult::Unchanged:
			return;
		case TypeResult::Proceed:
			break;
		}
	});

//	entryInputSource->signal_focus_out_event().connect([this](GdkEventFocus*) {
//		return false;
//	});
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
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES + ownerData->getProperty(UID));
}

void DialogInputSource::resetForm() {
	comboBoxInputSource->set_active_id("");
	entryInputSource->set_text("");
	if (Defaults::needSource(comboBoxInputSelectInput->get_active_id())) {
		if (Defaults::isDevInputListener(comboBoxInputSelectInput->get_active_id()))
			populateSourcesList(scanEventDevices());
	}
	clearForm();
}

void DialogInputSource::clearForm() {
//	comboBoxInputSource->get_parent()->hide();
	entryInputSource->get_parent()->hide();
	btnAddMap->set_sensitive(false);
}

void DialogInputSource::isValid() const {
	// no name for sourceless only.
	if (currentData->getProperty(SOURCELESS).empty()) {
		if (resolvedSource().empty()) throw Message("Enter a valid source name.");
	}
	if (not DialogInputMap::getInstance()->getBox()->getSize()) throw Message("Add at least one map.");
}

void DialogInputSource::storeData() {
	const string resolved(resolvedSource());
	currentData->setValue(SOURCE, resolved);
	// Store a human-friendly display label as a UI-only property.
	string label(comboBoxInputSource->get_active_text());
	if (label == SOURCE_OTHER_OPTION or label == SOURCE_EMPTY_OPTION or label.empty())
		label = resolved;
	currentData->setProperty(NAME, label);
}

void DialogInputSource::retrieveData() {
	if (not Defaults::needSource(comboBoxInputSelectInput->get_active_id())) return;

	const string source(currentData->getValue(SOURCE));
	previousName = source;
	// If setting the id fail because the source do not exists, set other and use entry instead.
	if (not comboBoxInputSource->set_active_id(source)) {
		comboBoxInputSource->set_active_id(SOURCE_OTHER_OPTION);
		entryInputSource->set_text(source);
	}
}

const string DialogInputSource::createUniqueId() const {
	return Defaults::createCommonUniqueId({ownerData->getProperty(UID), resolvedSource()});
}

void DialogInputSource::createPhantomSource() {
	// Activate the phantom immediately so the box gets populated.
	if (items->getSize()) {
		(*items->begin())->getData()->activate();
		return;
	}
	// Otherwise create the phantom source
	StringUMap rawData;
	auto phantom = createData(rawData);
	phantom->setProperty(SOURCELESS, "1");
//	phantom->setProperty(NAME, "<single>"); // never displayed
	phantom->activate();
	items->create(phantom);
	getCollectionHandler()->add(phantom);
}

void DialogInputSource::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
}

string_view DialogInputSource::getType() const {
	return TYPE_INPUT_SOURCE;
}

LEDSpicerUI::Ui::Storage::Data* DialogInputSource::createData(StringUMap& rawData) {
	auto is = new Storage::InputSource(rawData);
	is->setProperty(PID, ownerData->getProperty(UID));
	return is;
}

string DialogInputSource::resolvedSource() const {
	string s(comboBoxInputSource->get_active_id());
	return (s == SOURCE_OTHER_OPTION) ? string(entryInputSource->get_text()) : s;
}

StringMap DialogInputSource::scanEventDevices() {

	StringMap devices;

	if (
		Defaults::getMode() == Defaults::Mode::Portable or
		not Defaults::isDevInputListener(comboBoxInputSelectInput->get_active_id())
	) {
		return devices;
	}

	auto addDevices = [&](const std::filesystem::path& dirPath, bool useById) {
		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(dirPath, ec)) {
			if (ec) break;
			if (not entry.is_character_file() and not entry.is_symlink()) continue;
			const string name = entry.path().filename().string();
			if (useById and name.find("event") == string::npos) continue;
			string humanName = readDeviceName(name);
			devices.emplace(name, humanName.empty() ? name : humanName);
		}
	};

	addDevices(DEV_INPUT_BY_ID, true);
	if (devices.empty()) addDevices(DEV_INPUT, false);

	Defaults::indexDuplicateLabels(devices);
	return devices;
}

void DialogInputSource::populateSourcesList(const StringMap& devices) {

	comboBoxInputSource->remove_all();

	if (devices.empty()) {
		entryInputSource->get_parent()->show();
		entryInputSource->grab_focus();
		return;
	}

//	comboBoxInputSource->get_parent()->show();
	comboBoxInputSource->append("", SOURCE_EMPTY_OPTION);
	for (const auto& [id, display] : devices)
		comboBoxInputSource->append(id, display);
	comboBoxInputSource->append(SOURCE_OTHER_OPTION, SOURCE_OTHER_OPTION);
	comboBoxInputSource->set_active_id("");
}

string DialogInputSource::readDeviceName(const string& byIdName) {
	std::filesystem::path linkPath{DEV_INPUT_BY_ID + byIdName};
	if (not std::filesystem::exists(linkPath) or not std::filesystem::is_symlink(linkPath))
		return byIdName;

	std::error_code ec;
	auto target = std::filesystem::read_symlink(linkPath, ec);
	if (ec) return byIdName;

	std::filesystem::path sysPath{SYS_CLASS_INPUT + target.filename().string() + "/device/name"};
	std::ifstream file(sysPath);
	if (not file) return byIdName;

	string name;
	std::getline(file, name);
	Defaults::rtrim(name);
	return name;
}
