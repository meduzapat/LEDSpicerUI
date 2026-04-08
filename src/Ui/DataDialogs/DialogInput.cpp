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
	DialogFileForm(obj, builder, COLLECTION_INPUT)
{

	// DataDialogs::DialogInputLinkMaps::buildInstance(builder);
	DataDialogs::DialogInputSource::buildInstance(builder, "DialogInputSource");
	childDialogs.push_back(DialogInputSource::getInstance());

	builder->get_widget("ComboBoxInputSelectInput",   selectorCombo);
	builder->get_widget_derived("BoxInputs",          box);
	builder->get_widget("BtnAddInput",                btnAddInput);
	builder->get_widget("BtnApplyInput",              btnApply);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);
	builder->get_widget("BtnAddInputSource",          btnAddInputSource);
	builder->get_widget("ComboBoxInputSpeed",         comboBoxInputSpeed);
	builder->get_widget("EntryInputName",             entryInputName);
	builder->get_widget("SwitchInputBlink",           switchInputBlink);
	builder->get_widget("SpinInputTimes",             spinInputTimes);
	builder->get_widget("BoxInputSourcesBox",         boxInputSourcesBox);
	builder->get_widget("BoxInputMapsBox",            boxInputMapsBox);
	builder->get_widget("BoxInputCreditsSettings",    boxInputCreditsSettings);
	builder->get_widget("BoxLinkedElementsAndGroups", boxLinkedElementsAndGroups);
	builder->get_widget("BriefInput",                 brief);

//	builder->get_widget_derived("BoxInputLinkedMaps", boxInputLinkedMaps);
//	builder->get_widget_derived("BoxInputMaps",       boxDirectMaps);

	setSignalAdd(btnAddInput);
	setSignalApply();

	// Populate Input menu.
	initializeSelector(noInput, Defaults::inputInfo);
	// Models.
	listStore = static_cast<Gtk::ListStore*>(selectorCombo->get_model().get());

	selectorCombo->signal_changed().connect([this]() {

		if (handleTypeSwitch(
			DialogInputSource::getInstance()->getBox(),
			"Are you sure you want to change the input type? All sources will be lost.")
		) {
			resetForm();
		}

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
	return Storage::CollectionHandler::getInstance(
		COLLECTION_INPUT + ownerData->getProperties().getValue(UID)
	);
}

void DialogInput::resetForm() {
	const string name(selectorCombo->get_active_id());
	bool needSources {Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE)};

	btnAddInputSource->set_sensitive(true);
	boxInputSourcesBox->set_visible(needSources);
	boxInputMapsBox->set_visible(not needSources);

	boxLinkedElementsAndGroups->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_LINKED_MAPS));

	btnAddInputMap->set_visible(not needSources);
	btnAddInputMap->set_sensitive(true);

	comboBoxInputSpeed->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED));
	spinInputTimes->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES));
	switchInputBlink->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK));
	boxInputCreditsSettings->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS));

	brief->set_text(Defaults::inputInfo.at(name).brief.data());
	btnApply->set_sensitive(true);
	DialogForm::resetForm();
}

void DialogInput::isValid() const {

	const string id(selectorCombo->get_active_id());
	if (id.empty()) throw Message("Select an input type.");

	const string filename(entryInputName->get_text());
	if (filename.empty()) {
		if (action != Actions::LOAD) entryInputName->grab_focus();
		throw Message("Invalid name.");
	}

	const string uid(createUniqueId());
	if (getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != uid) {
			if (action != Actions::LOAD) entryInputName->grab_focus();
			throw Message("Name already in use in this directory.");
		}
	}

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES)) {
		const auto blinks(spinInputTimes->get_value_as_int());
		if (blinks < 0 or blinks > 255) spinInputTimes->set_value(0);
	}
}

void DialogInput::storeData() {

	const string id(selectorCombo->get_active_id());

	currentData->setValue(NAME, id);
	currentData->getProperties().setValue(FILENAME, entryInputName->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_BLINK))
		currentData->setValue(BLINK, switchInputBlink->get_active() ? "true" : "false");

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES))
		currentData->setValue(TIMES, spinInputTimes->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_SPEED))
		currentData->setValue(SPEED, comboBoxInputSpeed->get_active_id());
}

void DialogInput::retrieveData() {

	const string name(currentData->getValue(NAME));

	selectorCombo->set_active_id(name);
	entryInputName->set_text(currentData->getProperties().getValue(FILENAME));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK))
		switchInputBlink->set_active(currentData->getValue(BLINK) == "true");

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES))
		spinInputTimes->set_text(currentData->getValue(TIMES));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED))
		comboBoxInputSpeed->set_active_id(currentData->getValue(SPEED));
}

const string DialogInput::createUniqueId() const {
	return Defaults::createCommonUniqueId({currentData->getProperties().getValue(PID), entryInputName->get_text()});
}

string_view DialogInput::getType() const noexcept {
	return TYPE_INPUT;
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::createData(StringUMap& rawData) noexcept {
	return new Storage::Input(rawData, currentDirectory);
}

void DialogInput::onEmpty() {
	btnAddInputSource->set_sensitive(false);
	btnAddInputMap->set_sensitive(false); // sourceless maps.
	boxInputSourcesBox->hide();
	boxInputMapsBox->hide();
	boxLinkedElementsAndGroups->hide();
	boxInputCreditsSettings->hide();
	entryInputName->set_text("");
	comboBoxInputSpeed->get_parent()->hide();
	comboBoxInputSpeed->set_active_id("Normal");
	spinInputTimes->get_parent()->hide();
	spinInputTimes->set_text("");
	switchInputBlink->get_parent()->hide();
	switchInputBlink->set_active(false);
	brief->set_text("");
	btnApply->set_sensitive(false);
}

void DialogInput::onSelected() {
	auto id{selectorCombo->get_active_id()};
	const bool needSources {Defaults::inputHasFlag(id, Defaults::INPUT_NEEDS_SOURCE)};
	if (not needSources)
		DialogInputSource::getInstance()->createPhantomSource();
	else
		DialogInputSource::getInstance()->populateSources(id);
}
