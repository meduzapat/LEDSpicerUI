/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.cpp
 * @since     Feb 14, 2023
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

#include "DialogInput.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInput::DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogFileForm(obj, builder)
{

	//	DataDialogs::DialogInputLinkMaps::buildInstance(builder);
	DataDialogs::DialogInputSource::buildInstance(builder, "DialogInputSource");

	childDialogs.push_back(DialogInputSource::getInstance());

	Gtk::Button
		* btnAddInputSource = nullptr, // Only to hide/show when source is selected.
		* btnAddInputMap    = nullptr, // Only to hide/show when source is selected.
		* btnAddInput       = nullptr; // Form shooter.

	builder->get_widget_derived("BoxInputs",          box);
	builder->get_widget("BtnAddInput",                btnAddInput);
	builder->get_widget("BtnApplyInput",              btnApply);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);
	builder->get_widget("BtnAddInputSource",          btnAddInputSource);
	builder->get_widget("ComboBoxInputSelectInput",   comboBoxInputSelectInput);
	builder->get_widget("ComboBoxInputSpeed",         comboBoxInputSpeed);
	builder->get_widget("EntryInputName",             entryInputName);
	builder->get_widget("SwitchInputBlink",           switchInputBlink);
	builder->get_widget("SpinInputTimes",             spinInputTimes);
	builder->get_widget("BoxInputSourcesBox",         boxInputSourcesBox);
	builder->get_widget("BoxInputMapsBox",            boxInputMapsBox);
	builder->get_widget("BoxInputCreditsSettings",    boxInputCreditsSettings);
	builder->get_widget_derived("BoxInputLinkedMaps", boxInputLinkedMaps);
	builder->get_widget_derived("BoxInputMaps",       boxDirectMaps);

	setSignalAdd(btnAddInput);
	setSignalApply();

	// Populate input types.
	comboBoxInputSelectInput->append("", "Select Input Type");
	for (const auto& [id, info] : Defaults::inputsInfo)
		comboBoxInputSelectInput->append(id, info.name);

	comboBoxInputSelectInput->signal_changed().connect([this, btnAddInputSource, btnAddInputMap]() {
		const string name(comboBoxInputSelectInput->get_active_id());
		switch (handleTypeSwitch(comboBoxInputSelectInput, name, "Are you sure you want to change the input type? All settings will be lost.")) {
		case TypeSwitchResult::Empty:
			btnAddInputSource->set_sensitive(false);
			btnAddInputMap->set_sensitive(false);
			btnApply->set_sensitive(false);
			return;
		case TypeSwitchResult::Unchanged:
			return;
		case TypeSwitchResult::Proceed:
			break;
		}

		const bool valid = not name.empty();
		btnAddInputSource->set_sensitive(valid);
		boxInputSourcesBox->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE));
		boxInputMapsBox->set_visible(not Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE));
		boxInputLinkedMaps->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_LINKED_MAPS));
		btnAddInputMap->set_visible(not Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE));
		btnAddInputMap->set_sensitive(valid);
		btnApply->set_sensitive(valid);
		comboBoxInputSpeed->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED));
		spinInputTimes->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES));
		switchInputBlink->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK));
		boxInputCreditsSettings->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS));
	});

}

DialogInput::~DialogInput() {
	//	delete DataDialogs::DialogInputLinkMaps::getInstance();
	delete DataDialogs::DialogInputSource::getInstance();
}

void DialogInput::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_INPUT), values);
}

void DialogInput::createSubItems(XMLHelper* values) {
	DialogInputSource::getInstance()->load(values);
//	DialogInputLinkMaps::getInstance()->load(values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInput::getCollectionHandler() const {
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT + ownerData->getProperty(UID));
}

void DialogInput::resetForm() {
	previousName = "";
	comboBoxInputSelectInput->set_active(0);
	clearForm();
}

void DialogInput::clearForm() {
	boxInputSourcesBox->hide();
	boxInputMapsBox->hide();
	boxInputLinkedMaps->hide();
	boxInputCreditsSettings->hide();
	entryInputName->set_text("");
	comboBoxInputSpeed->get_parent()->hide();
	comboBoxInputSpeed->set_active_text("Normal");
	spinInputTimes->get_parent()->hide();
	spinInputTimes->set_text("");
	switchInputBlink->get_parent()->hide();
	switchInputBlink->set_active(false);
	btnApply->set_sensitive(false);
	boxDirectMaps->wipe();
}

void DialogInput::isValid() const {

	const string id(comboBoxInputSelectInput->get_active_id());
	if (id.empty()) {
		throw Message("Select an input type.");
	}

	const string filename(entryInputName->get_text());
	if (filename.empty()) {
		if (action != Actions::LOAD)
			entryInputName->grab_focus();
		throw Message("Invalid name.");
	}
	if (not isUniqueFilename(filename)) {
		if (action != Actions::LOAD)
			entryInputName->grab_focus();
		throw Message("Name already in use in this directory.");
	}

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES)) {
		const auto blinks(spinInputTimes->get_value_as_int());
		if (blinks < 0 or blinks > 255)
			spinInputTimes->set_value(0);
	}

}

void DialogInput::storeData() {
	const string id(comboBoxInputSelectInput->get_active_id());
	currentData->setValue(NAME, id);
	currentData->setProperty(FILENAME, entryInputName->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_BLINK))
		currentData->setValue(BLINK, switchInputBlink->get_active() ? "true" : "false");

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES))
		currentData->setValue(TIMES, spinInputTimes->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_SPEED))
		currentData->setValue(SPEED, comboBoxInputSpeed->get_active_text());
}

void DialogInput::retrieveData() {
	const string name(currentData->getValue(NAME));
	comboBoxInputSelectInput->set_active_id(name);
	entryInputName->set_text(currentData->getProperty(FILENAME));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK))
		switchInputBlink->set_active(currentData->getValue(BLINK) == "true");

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES))
		spinInputTimes->set_text(currentData->getValue(TIMES));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED))
		comboBoxInputSpeed->set_active_text(currentData->getValue(SPEED));
}

const string DialogInput::createUniqueId() const {
	return entryInputName->get_text();
}

string_view DialogInput::getType() const {
	return TYPE_INPUT;
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::createData(StringUMap& rawData) {
	rawData[FILENAME] = rawData.count(FILENAME) ? rawData.at(FILENAME) : "";
	return new Storage::Input(rawData, currentDirectory);
}
