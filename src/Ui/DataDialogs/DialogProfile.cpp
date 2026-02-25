/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.cpp
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

#include "DialogProfile.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogProfile::DialogProfile(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Profile Box and button.
	builder->get_widget_derived("BoxProfiles", box);
	builder->get_widget("BtnApplyProfile",     btnApply);
	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddProfile", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	builder->get_widget("InputProfileName",          inputProfileName);
	builder->get_widget("BtnProfileBackgroundColor", btnProfileBackgroundColor);
	DialogColors::getInstance()->activateColorButton(btnProfileBackgroundColor);

	// Always on elements selector.
	builder->get_widget("BtnProfilesAddElements",             btnProfilesAddElements);
	builder->get_widget_derived("BoxProfileAlwaysOnElements", boxProfileAlwaysOnElements);
	btnProfilesAddElements->signal_clicked().connect([&]() {
		DialogSelect::getInstance()->setSettings(alwaysOnElementsSelectSetting);
		DialogSelect::getInstance()->runSelection();
	});

	// Always on group selector.
	builder->get_widget("BtnProfilesAddGroups",             btnProfilesAddGroups);
	builder->get_widget_derived("BoxProfileAlwaysOnGroups", boxProfileAlwaysOnGroups);
	btnProfilesAddGroups->signal_clicked().connect([&]() {
		DialogSelect::getInstance()->setSettings(alwaysOnGroupsSelectSetting);
		DialogSelect::getInstance()->runSelection();
	});

	// Animations selector.
	builder->get_widget("BtnProfileAddAnimations",      btnProfilesAddAnimations);
	builder->get_widget_derived("BoxProfileAnimations", boxProfileAnimations);
	btnProfilesAddAnimations->signal_clicked().connect([&]() {
		DialogSelect::getInstance()->setSettings(animationsSelectSetting);
		DialogSelect::getInstance()->runSelection();
	});

	// Inputs selector.
	builder->get_widget("BtnProfileAddInputs",      btnProfilesAddInputs);
	builder->get_widget_derived("BoxProfileInputs", boxProfileInputs);
	btnProfilesAddInputs->signal_clicked().connect([&]() {
		DialogSelect::getInstance()->setSettings(inputsSelectSetting);
		DialogSelect::getInstance()->runSelection();
	});

	// Start transition selector.
//	builder->get_widget("BtnAddStartTransitions",             btnProfilesAddStartTransitions);
//	builder->get_widget_derived("BoxProfileStartTransitions", boxProfileStartTransitions, "BtnStartTransitionsUp", "BtnStartTransitionsDn");
//	btnProfilesAddStartTransitions->signal_clicked().connect([&]() {
//		DialogSelect::getInstance()->setSettings(startTransitionsSelectSetting);
//		DialogSelect::getInstance()->runSelection();
//	});

	// End transition selector.
//	builder->get_widget("BtnAddEndTransitions",             btnProfilesAddEndTransitions);
//	builder->get_widget_derived("BoxProfileEndTransitions", boxProfileEndTransitions, "BtnEndTransitionsUp", "BtnEndTransitionsDn");
//	btnProfilesAddEndTransitions->signal_clicked().connect([&]() {
//		DialogSelect::getInstance()->setSettings(endTransitionsSelectSetting);
//		DialogSelect::getInstance()->runSelection();
//	});
}

void DialogProfile::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_PROFILES), values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogProfile::getCollectionHandler() const {
	return LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_PROFILES);
}

void DialogProfile::createSubItems(XMLHelper* values) {
	auto dialogSelect = DialogSelect::getInstance();
	dialogSelect->setSettings(alwaysOnElementsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
	dialogSelect->setSettings(alwaysOnGroupsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
	dialogSelect->setSettings(animationsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
	dialogSelect->setSettings(inputsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
	dialogSelect->setSettings(startTransitionsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
	dialogSelect->setSettings(endTransitionsSelectSetting);
	dialogSelect->load(values, COLLECTION_PROFILES);
}

void DialogProfile::clearForm() {
	inputProfileName->set_text("");
	DialogColors::getInstance()->colorizeButton(btnProfileBackgroundColor, DEFAULT_PROFILE_BACKGROUND_COLOR);
	// if no group, elements, animations or input disable button add
	btnProfilesAddElements->set_sensitive(Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)->getSize());
	btnProfilesAddGroups->set_sensitive(Storage::CollectionHandler::getInstance(COLLECTION_GROUP)->getSize());
	btnProfilesAddInputs->set_sensitive(Storage::CollectionHandler::getInstance(COLLECTION_INPUT)->getSize());

	bool animationsSize(Storage::CollectionHandler::getInstance(COLLECTION_ANIMATIONS)->getSize());
	btnProfilesAddAnimations->set_sensitive(animationsSize);
	btnProfilesAddStartTransitions->set_sensitive(animationsSize);
	btnProfilesAddEndTransitions->set_sensitive(animationsSize);
}

void DialogProfile::isValid() const {
	string name(createUniqueId());
	if (name.empty()) {
		if (action != Actions::LOAD)
			inputProfileName->grab_focus();
		throw Message("Invalid profile name.");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != name) {
			if (action != Actions::LOAD)
				inputProfileName->grab_focus();
			throw Message("Profile with name " + name + " already exist.");
		}
	}
	if (btnProfileBackgroundColor->get_label().empty()) {
		throw Message("Select a valid background color.");
	}
}

void DialogProfile::storeData() {
	currentData->setValue(FILENAME, inputProfileName->get_text());
	currentData->setValue(BACKGROUND_COLOR, btnProfileBackgroundColor->get_label());
}

void DialogProfile::retrieveData() {
	inputProfileName->set_text(currentData->getValue(FILENAME));
	DialogColors::getInstance()->colorizeButton(
		btnProfileBackgroundColor,
		currentData->getValue(BACKGROUND_COLOR)
	);
}

string const DialogProfile::createUniqueId() const {
	return Defaults::createCommonUniqueId({inputProfileName->get_text()});
}

const string DialogProfile::getType() const {
	return "profile";
}

LEDSpicerUI::Ui::Storage::Data* DialogProfile::createData(StringUMap& rawData) {
	return new Storage::Profile(rawData);
}

