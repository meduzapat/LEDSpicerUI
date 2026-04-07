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
	DialogFormHost(obj, builder, COLLECTION_INPUT_SOURCES)
{

	// From Input Dialog — sourceless path.
	Gtk::Button* btnAdd = nullptr;
	DataDialogs::DialogInputMap::buildInstance(builder, "DialogInputMap");

	builder->get_widget("ComboBoxInputSource",      selectorCombo);
	builder->get_widget_derived("BoxInputSources",  box);
	builder->get_widget("BtnApplyInputSource",      btnApply);
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput);
	builder->get_widget("BtnAddInputSource",        btnAdd);
	builder->get_widget("BtnAddInputSourceMap",     btnAddMap);

	setSignalAdd(btnAdd);
	setSignalApply();

	childDialogs.push_back(DialogInputMap::getInstance());

	selectorCombo->signal_changed().connect([this]() {
		if (
			handleTypeSwitch(
				DialogInputMap::getInstance()->getBox(),
				"Are you sure you want to change the source? All maps will be lost."
			)
		) {
			resetForm();
		}
	});

	selectorCombo->get_entry()->signal_changed().connect([this]() {
		resetForm();
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
	// Collection ID + input ID
	return Storage::CollectionHandler::getInstance(
		COLLECTION_INPUT_SOURCES + ownerData->getProperties().getValue(UID)
	);
}

void DialogInputSource::resetForm() {
	if (resolvedSource().empty()) return;
	btnApply->set_sensitive(true);
	btnAddMap->set_sensitive(true);
}

void DialogInputSource::isValid() const {
	// no name for sourceless only.
	if (currentData->getProperties().getValue(SOURCELESS).empty()) {
		if (resolvedSource().empty()) throw Message("Enter a valid source name.");
	}
	if (not DialogInputMap::getInstance()->getBox()->getSize()) throw Message("Add at least one map.");
}

void DialogInputSource::storeData() {
	Glib::ustring
		id{selectorCombo->get_active_id()},
		label;
	if (id.empty()) {
		label = id = selectorCombo->get_entry()->get_text();
	}
	else {
		auto iter = selectorCombo->get_active();
		if (iter)
			iter->get_value(1, label);
		else
			label = selectorCombo->get_entry()->get_text();
	}
	currentData->setValue(SOURCE, id);
	// Store a human-friendly display label as a UI-only property.
	currentData->getProperties().setValue(NAME, label);
}

void DialogInputSource::retrieveData() {
	// sourceless will be handled at activation.
	if (not Defaults::needSource(comboBoxInputSelectInput->get_active_id())) return;

	const string source(currentData->getValue(SOURCE));

	// If setting the id fail because the source do not exists, set other and use entry instead.
	if (not selectorCombo->set_active_id(source))
		selectorCombo->get_entry()->set_text(source);
}

const string DialogInputSource::createUniqueId() const {
	return Defaults::createCommonUniqueId({
		ownerData->getProperties().getValue(UID),
		resolvedSource()
	});
}

void DialogInputSource::populateSources(const string& name) {
	if (Defaults::needSource(name)) {
		if (Defaults::isDevInputListener(name))
			populateSourcesComboBox(scanEventDevices());
		// add other future sources here
	}
}

void DialogInputSource::setOwner(Storage::BoxButtonCollection* collection, const Storage::Data* owner) {
	this->ownerData = owner;
	items = collection;
	auto s{comboBoxInputSelectInput->get_active_id()};
	if (not s.empty() and not Defaults::inputHasFlag(
		s,
		Defaults::INPUT_NEEDS_SOURCE
	))
		createPhantomSource();
	refreshItems();
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
	phantom->activate();
	items->create(phantom);
	getCollectionHandler()->add(phantom);
}

void DialogInputSource::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
}

string_view DialogInputSource::getType() const noexcept {
	return TYPE_INPUT_SOURCE;
}

LEDSpicerUI::Ui::Storage::Data* DialogInputSource::createData(StringUMap& rawData) noexcept {
	auto is{new Storage::InputSource(rawData, ownerData->getProperties().getValue(UID))};
	is->getProperties().setValue(PID, ownerData->getProperties().getValue(UID));
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

Glib::ustring DialogInputSource::resolvedSource() const {
	auto id{selectorCombo->get_active_id()};
	if (id.empty())
		id = selectorCombo->get_entry()->get_text();
	return id;
}

void DialogInputSource::populateSourcesComboBox(const StringMap& sources) {

	auto listStore = static_cast<Gtk::ListStore*>(selectorCombo->get_model().get());
	listStore->clear();

	if (sources.empty()) {
		selectorCombo->get_entry()->grab_focus();
		return;
	}

	auto row = *(listStore->append());
	row.set_value(0, string());
	row.set_value(1, string());
	row.set_value(2, true);

	for (const auto& [id, display] : sources) {
		auto row = *(listStore->append());
		row.set_value(0, id);
		row.set_value(1, display);
		row.set_value(2, true);
	}

	selectorCombo->set_active(-1);
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

void DialogInputSource::onEmpty() {
	btnApply->set_sensitive(false);
	btnAddMap->set_sensitive(false);
}
