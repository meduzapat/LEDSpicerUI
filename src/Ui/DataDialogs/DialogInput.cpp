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

DialogInput::DialogInput(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	DialogFormHost(obj, builder)
{

	registerChildDialog<DialogInputSource>(builder, "DialogInputSource", COLLECTION_INPUT_SOURCES);
	registerChildDialog<DialogInputLinkMaps>(builder, "DialogInputLinkMaps", COLLECTION_INPUT_LINKMAPS);

	builder->get_widget("ComboBoxInputSelectInput",   selectorCombo);
	builder->get_widget_derived("BoxInputs",          box);
	builder->get_widget("BtnAddInput",                btnAddInput);
	builder->get_widget("BtnApplyInput",              btnApply);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);
	builder->get_widget("BtnAddInputSource",          btnAddInputSource);
	builder->get_widget("ComboBoxInputSpeed",         comboBoxInputSpeed);
	builder->get_widget("ComboBoxInputCreditsMode",   comboBoxInputCreditsMode);
	builder->get_widget("EntryInputName",             entryInputName);
	builder->get_widget("SwitchInputBlink",           switchInputBlink);
	builder->get_widget("SpinInputTimes",             spinInputTimes);
	builder->get_widget("SwitchInputCreditsOnce",     switchInputCreditsOnce);
	builder->get_widget("SwitchInputCreditsAlwaysOn", switchInputCreditsAlwaysOn);
	builder->get_widget("SpinInputCreditsCoins",      spinInputCreditsCoins);
	builder->get_widget("BoxInputSourcesBox",         boxInputSourcesBox);
	builder->get_widget("BoxInputMapsBox",            boxInputMapsBox);
	builder->get_widget("BoxInputCreditsSettings",    boxInputCreditsSettings);
	builder->get_widget("BoxLinkedElementsAndGroups", boxLinkedElementsAndGroups);
	builder->get_widget("BriefInput",                 brief);

	Defaults::linkSwitchToWidget(switchInputBlink, comboBoxInputSpeed);

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

void DialogInput::load(DataMap& values) noexcept {
	createItems(
		values[Defaults::createCommonUniqueId({currentDirectory->getFullPath(), COLLECTION_INPUTS})],
		values
	);
}

void DialogInput::createSubItems(DataMap& values) noexcept {
	DialogInputSource::getInstance()->load(values);
	DialogInputLinkMaps::getInstance()->load(values);
}

void DialogInput::resetForm() noexcept {

	string name(selectorCombo->get_active_id());
	bool needSources {Defaults::needSource(name)};

	btnAddInputSource->set_sensitive(true);
	boxInputSourcesBox->set_visible(needSources);
	boxInputMapsBox->set_visible(not needSources);

	boxLinkedElementsAndGroups->set_visible(Defaults::hasLinkedMaps(name));

	btnAddInputMap->set_visible(not needSources);
	btnAddInputMap->set_sensitive(true);
	spinInputTimes->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES));
	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK)) {
		switchInputBlink->get_parent()->set_visible(true);
	}
	else {
		comboBoxInputSpeed->set_sensitive(true);
	}
	boxInputCreditsSettings->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS));
	comboBoxInputSpeed->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED));

	brief->set_text(Defaults::inputInfo.at(name).brief.data());
	btnApply->set_sensitive(true);
	DialogForm::resetForm();
}

void DialogInput::isValid() const {

	string id(selectorCombo->get_active_id());
	if (id.empty()) throw Message("Select an input type.");

	string filename(entryInputName->get_text());
	if (filename.empty()) {
		if (action != Actions::LOAD) entryInputName->grab_focus();
		throw Message("Invalid name.");
	}

	string uid(createUniqueId());
	if (currentData->getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != uid) {
			if (action != Actions::LOAD) entryInputName->grab_focus();
			throw Message("Name already in use in this directory.");
		}
	}

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES)) {
		const auto blinks(spinInputTimes->get_value_as_int());
		if (blinks < 0 or blinks > 255) spinInputTimes->set_value(0);
	}

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_CREDITS) and spinInputCreditsCoins->get_value() < 1) {
		if (action != Actions::LOAD) spinInputCreditsCoins->grab_focus();
			throw Message("Coins or tokens per credit, need to be more than 0.");
	}

	if (action != Actions::LOAD) {
		if (Defaults::needSource(id)) {
			if (not DialogInputSource::getInstance()->getBox()->getSize())
				throw Message("Add at least one source.");
		}
		else {
			if (not DialogInputMap::getInstance()->getBox()->getSize())
				throw Message("Add at least one map.");
		}
	}
}

void DialogInput::storeData() noexcept {

	string id(selectorCombo->get_active_id());

	currentData->setValue(NAME, id);
	currentData->getProperties().setValue(FILENAME, entryInputName->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_BLINK))
		currentData->setValue(BLINK, switchInputBlink->get_active() ? HUMAN_TRUE : HUMAN_FALSE);

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_TIMES))
		currentData->setValue(TIMES, spinInputTimes->get_text());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_SPEED))
		currentData->setValue(SPEED, comboBoxInputSpeed->get_active_id());

	if (Defaults::inputHasFlag(id, Defaults::INPUT_HAS_CREDITS)) {
		currentData->setValue(MODE, comboBoxInputCreditsMode->get_active_text());
		currentData->setValue(COINS_CREDIT, spinInputCreditsCoins->get_text());
		currentData->setValue(ONCE, switchInputCreditsOnce->get_active() ? HUMAN_TRUE : HUMAN_FALSE);
		currentData->setValue(ALWAYS_ON, switchInputCreditsAlwaysOn->get_active() ? HUMAN_TRUE : HUMAN_FALSE);
	}
}

void DialogInput::retrieveData() noexcept {

	currentData->getProperties().setValue(PATH_BASE, currentData->getValue(PATH_BASE));

	string name(currentData->getValue(NAME));

	selectorCombo->set_active_id(name);
	entryInputName->set_text(currentData->getProperties().getValue(FILENAME));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK))
		switchInputBlink->set_active(currentData->is(BLINK));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES))
		spinInputTimes->set_text(currentData->getValue(TIMES));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED))
		comboBoxInputSpeed->set_active_id(currentData->getValue(SPEED));

	if (Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS)) {
		comboBoxInputCreditsMode->set_active_text(currentData->getValue(MODE));
		spinInputCreditsCoins->set_value(std::stof(currentData->getValue(COINS_CREDIT)));
		switchInputCreditsOnce->set_active(currentData->is(ONCE));
		switchInputCreditsAlwaysOn->set_active(currentData->is(ALWAYS_ON));
	}
}

string DialogInput::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({currentData->getProperties().getValue(PID), entryInputName->get_text()});
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::createData(Values& rawData) const noexcept {
	return new Storage::Input(rawData, currentDirectory);
}

void DialogInput::onEmpty() noexcept {
	btnAddInputSource->set_sensitive(false);
	btnAddInputMap->set_sensitive(false); // sourceless maps.
	boxInputSourcesBox->hide();
	boxInputMapsBox->hide();
	boxLinkedElementsAndGroups->hide();
	boxInputCreditsSettings->hide();
	entryInputName->set_text("");
	comboBoxInputCreditsMode->set_active(0);
	spinInputTimes->get_parent()->hide();
	spinInputTimes->set_value(0.0f);
	spinInputCreditsCoins->set_value(1.0f);
	switchInputCreditsOnce->set_active(false);
	switchInputCreditsAlwaysOn->set_active(false);
	switchInputBlink->get_parent()->hide();
	switchInputBlink->set_active(false);
	comboBoxInputSpeed->get_parent()->hide();
	comboBoxInputSpeed->set_active_id(HUMAN_NORMAL);
	brief->set_text("");
	btnApply->set_sensitive(false);
}

void DialogInput::onSelected() noexcept {
	const auto name{selectorCombo->get_active_id()};
	const bool
//		oldSourced {Defaults::needSource(previousName)},
		newSourced {Defaults::needSource(name)};

//	if (oldSourced and not newSourced)
	if (newSourced)
		DialogInputSource::getInstance()->populateSources(name);
	else
		DialogInputSource::getInstance()->resolveSourcelessStorage();

	// Drop fields the new type doesn't support.
	if (not Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK))
		currentData->unSet(BLINK);
	if (not Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES))
		currentData->unSet(TIMES);
	if (not Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED))
		currentData->unSet(SPEED);
	if (not Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS)) {
		currentData->unSet(MODE);
		currentData->unSet(COINS_CREDIT);
		currentData->unSet(ONCE);
		currentData->unSet(ALWAYS_ON);
	}
	if (not Defaults::hasLinkedMaps(name))
		DialogInputLinkMaps::getInstance()->clearForm();
}
