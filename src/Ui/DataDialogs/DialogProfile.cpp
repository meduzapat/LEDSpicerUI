/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.cpp
 * @since     Feb 14, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

DialogProfile* DialogProfile::instance = nullptr;

void DialogProfile::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogProfile", instance);
	}
}

DialogProfile* DialogProfile::getInstance() {
	return instance;
}

DialogProfile::DialogProfile(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Profile Box and button.
	builder->get_widget_derived("BoxProfiles", box);
	builder->get_widget("BtnAddProfile",       btnAdd);
	builder->get_widget("BtnApplyProfile",     btnApply);
	setSignalAdd();
	setSignalApply();

	builder->get_widget("InputProfileName",          inputProfileName);
	builder->get_widget("BtnProfileBackgroundColor", btnProfileBackgroundColor);
	DialogColors::getInstance()->activateColorButton(btnProfileBackgroundColor);

	// Always on elements selector.
	builder->get_widget("BtnProfilesAddElements", btnProfilesAddElements);
	btnProfilesAddElements->signal_clicked().connect([&]() {
		prepareSelect(Selectors::AlwaysOnElements);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileAlwaysOnElements", boxProfileAlwaysOnElements);

	// Always on group selector.
	builder->get_widget("BtnProfilesAddGroups", btnProfilesAddGroups);
	btnProfilesAddGroups->signal_clicked().connect([&]() {
		prepareSelect(Selectors::AlwaysOnGroups);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileAlwaysOnGroups", boxProfileAlwaysOnGroups);

	// Animations selector.
	builder->get_widget("BtnProfileAddAnimations", btnProfilesAddAnimations);
	btnProfilesAddAnimations->signal_clicked().connect([&]() {
		prepareSelect(Selectors::Animationss);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileAnimations", boxProfileAnimations);

	// Inputs selector.
	builder->get_widget("BtnProfileAddInputs", btnProfilesAddInputs);
	btnProfilesAddInputs->signal_clicked().connect([&]() {
		prepareSelect(Selectors::Inputs);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileInputs", boxProfileInputs);

	// Start transition selector.
	builder->get_widget("BtnAddStartTransitions", btnProfilesAddStartTransitions);
	btnProfilesAddStartTransitions->signal_clicked().connect([&]() {
		prepareSelect(Selectors::StartTransitions);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileStartTransitions", boxProfileStartTransitions, "BtnStartTransitionsUp", "BtnStartTransitionsDn");

	// End transition selector.
	builder->get_widget("BtnAddEndTransitions", btnProfilesAddEndTransitions);
	btnProfilesAddEndTransitions->signal_clicked().connect([&]() {
		prepareSelect(Selectors::EndTransitions);
		DialogSelect::getInstance()->RunDialog();
	});
	builder->get_widget_derived("BoxProfileEndTransitions", boxProfileEndTransitions, "BtnEndTransitionsUp", "BtnEndTransitionsDn");
}

void DialogProfile::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_PROFILES), values);
}

void DialogProfile::createSubItems(XMLHelper* values) {
	prepareSelect(Selectors::AlwaysOnElements);
	DataDialogs::DialogSelect::getInstance()->load(values);
	prepareSelect(Selectors::AlwaysOnGroups);
	DataDialogs::DialogSelect::getInstance()->load(values);
	prepareSelect(Selectors::Animationss);
	DataDialogs::DialogSelect::getInstance()->load(values);
	prepareSelect(Selectors::Inputs);
	DataDialogs::DialogSelect::getInstance()->load(values);
	prepareSelect(Selectors::StartTransitions);
	DataDialogs::DialogSelect::getInstance()->load(values);
	prepareSelect(Selectors::EndTransitions);
	DataDialogs::DialogSelect::getInstance()->load(values);
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
		if (mode != Modes::LOAD)
			inputProfileName->grab_focus();
		throw Message("Invalid profile name.");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (profileCollectionHandler->isUsed(name)) {
		if (mode != Modes::EDIT or currentData->createUniqueId() != name) {
			if (mode != Modes::LOAD)
				inputProfileName->grab_focus();
			throw Message("Profile with name " + name + " already exist.");
		}
	}
	if (btnProfileBackgroundColor->get_tooltip_text().empty()) {
		throw Message("Select a valid background color.");
	}
}

void DialogProfile::storeData() {
	if (mode == Modes::EDIT)
		profileCollectionHandler->replace(currentData->createUniqueId(), createUniqueId());
	else
		profileCollectionHandler->add(createUniqueId());

	// This will clean any anomaly.
	currentData->wipe();

	currentData->setValue(FILENAME, inputProfileName->get_text());
	currentData->setValue(BACKGROUND_COLOR, btnProfileBackgroundColor->get_tooltip_text());
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

LEDSpicerUI::Ui::Storage::Data* DialogProfile::getData(unordered_map<string, string>& rawData) {
	return new Storage::Profile(rawData);
}

void DialogProfile::prepareSelect(Selectors selector) const {
	switch (selector) {
	case Selectors::AlwaysOnElements:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileAlwaysOnElements,
			COLLECTION_ELEMENT,
			"element",
			COLLECTION_PROFILES,
			DialogSelect::COLORER | DialogSelect::DELETER
		);
	break;
	case Selectors::AlwaysOnGroups:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileAlwaysOnGroups,
			COLLECTION_GROUP,
			"group",
			COLLECTION_PROFILES,
			DialogSelect::COLORER | DialogSelect::DELETER
		);
	break;
	case Selectors::Inputs:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileInputs,
			COLLECTION_INPUT,
			"input",
			COLLECTION_PROFILES,
			DialogSelect::DELETER
		);
	break;
	case Selectors::Animationss:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileAnimations,
			COLLECTION_ANIMATIONS,
			"animation",
			COLLECTION_PROFILES,
			DialogSelect::DELETER
		);
	break;
	case Selectors::StartTransitions:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileStartTransitions,
			COLLECTION_ANIMATIONS,
			"startTransitions",
			COLLECTION_PROFILES,
			DialogSelect::DELETER
		);
	break;
	case Selectors::EndTransitions:
		DataDialogs::DialogSelect::getInstance()->setDestinationSettings(
			boxProfileEndTransitions,
			COLLECTION_ANIMATIONS,
			"endTransitions",
			COLLECTION_GROUP,
			DialogSelect::DELETER
		);
	break;
	}
	auto profile(dynamic_cast<Storage::Profile*>(currentData));
	profile->lateActivate(selector);
}
