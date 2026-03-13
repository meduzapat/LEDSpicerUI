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
	Gtk::Button
		* btnAdd          = nullptr,
		* btnAddMapSingle = nullptr; // From Input Dialog — sourceless path.

	/*
	 * Register before initializing child dialog so the sourceless creation
	 * signal is wired before DialogInputMap claims the same button.
	 */
	builder->get_widget("BtnAddInputMap",       btnAddMapSingle);
	btnAddMapSingle->signal_clicked().connect([this]() {
		createButtonDirectly();
	});

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

	/*
	 * Unified source-change handler.
	 * Fires when either the combo settles on a real value or the entry loses focus.
	 * Wipes maps after confirmation if any exist; reverts UI on cancel.
	 */
	std::function<void()> onSourceChanged = [this]() {
		const string resolved(resolvedSource());
		if (resolved.empty() or resolved == previousName) return;

		if (
			DialogInputMap::getInstance()->getBox()->getSize() == 0 or
			Message::ask("Changing the source will delete all maps. Are you sure?") == Gtk::ResponseType::RESPONSE_YES
		) {
			previousName = resolved;
			switchType();
			comboBoxInputSource->set_active_id(resolved);
			return;
		}

		// User cancelled — revert UI to previousName.
		if (not comboBoxInputSource->set_active_id(previousName)) {
			comboBoxInputSource->set_active_id(SOURCE_OTHER_OPTION);
			entryInputSource->set_text(previousName);
		}
	};

	comboBoxInputSource->signal_changed().connect([this, onSourceChanged]() {
		const string selected(comboBoxInputSource->get_active_id());
		btnAddMap->set_sensitive(false);
		entryInputSource->get_parent()->set_visible(false);

		if (selected.empty()) return;

		if (selected == SOURCE_OTHER_OPTION) {
			entryInputSource->get_parent()->set_visible(true);
			entryInputSource->grab_focus();
			return; // commit deferred until focus_out
		}

		onSourceChanged();
		// Re-enable after onSourceChanged() — previousName holds the committed value.
		btnAddMap->set_sensitive(not previousName.empty());
	});

	entryInputSource->signal_changed().connect([this]() {
		btnAddMap->set_sensitive(not entryInputSource->get_text().empty());
	});

	entryInputSource->signal_focus_out_event().connect([onSourceChanged](GdkEventFocus*) {
		onSourceChanged();
		return false;
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
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES + ownerData->getProperty(UID));
}

void DialogInputSource::resetForm() {
	previousName = "";
	comboBoxInputSource->set_active(-1);
	clearForm();
}

void DialogInputSource::clearForm() {
	comboBoxInputSource->get_parent()->hide();
	entryInputSource->get_parent()->hide();
	entryInputSource->set_text("");
	btnAddMap->set_sensitive(false);

	if (Defaults::needSource(comboBoxInputSelectInput->get_active_id()))
		populateSourcesList(scanEventDevices());
}

void DialogInputSource::isValid() const {
	if (Defaults::needSource(comboBoxInputSelectInput->get_active_id())) {
		if (resolvedSource().empty())
			throw Message("Enter a valid event source name.");
	}
	if (not DialogInputMap::getInstance()->getBox()->getSize())
		throw Message("Add at least one map.");
}

void DialogInputSource::storeData() {
	const string resolved(resolvedSource());
	currentData->setValue(SOURCE, resolved);
	/* Store a human-friendly display label as a UI-only property. */
	string label(comboBoxInputSource->get_active_text());
	if (label == SOURCE_OTHER_OPTION or label == SOURCE_EMPTY_OPTION or label.empty())
		label = resolved;
	currentData->setProperty(NAME, label);
}

void DialogInputSource::retrieveData() {
	if (not Defaults::needSource(comboBoxInputSelectInput->get_active_id())) return;

	const string source(currentData->getValue(SOURCE));
	if (not comboBoxInputSource->set_active_id(source)) {
		comboBoxInputSource->set_active_id(SOURCE_OTHER_OPTION);
		entryInputSource->set_text(source);
	}
	previousName = source;
}

const string DialogInputSource::createUniqueId() const {
	return Defaults::createCommonUniqueId({ownerData->getProperty(UID), resolvedSource()});
}

void DialogInputSource::createButtonDirectly() {
	if (items->getSize() > 0) {
		(*items->begin())->getData()->activate();
		return;
	}
	StringUMap rawData;
	auto phantom = createData(rawData);
	// TODO: future — preserve compatible sources when switching within the
	//       same INPUT_DEV_LISTENER family (same device model).
	phantom->setProperty(SOURCELESS, "true");
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
	return (s == SOURCE_OTHER_OPTION or s.empty()) ? string(entryInputSource->get_text()) : s;
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
		for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
			if (not entry.is_regular_file() and not entry.is_symlink()) continue;
			string name = entry.path().filename().string();
			if (name.empty() or name[0] == '.' or (useById and name.find("event") == string::npos)) continue;
			string humanName = readDeviceName(name);
			if (humanName.empty()) humanName = name;
			devices.emplace(name, humanName);
		}
	};

	addDevices(DEV_INPUT_BY_ID, true);
	if (not devices.empty()) {
		Defaults::indexDuplicateLabels(devices);
		return devices;
	}

	addDevices(DEV_INPUT, false);
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

	comboBoxInputSource->get_parent()->show();
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
