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
using namespace LEDSpicerUI::Ui::Storage;

DialogProfile::DialogProfile(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder)
{
	builder->get_widget_derived("BoxProfiles", box);
	builder->get_widget("BtnApplyProfile",     btnApply);
	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddProfile",             btnAdd);
	builder->get_widget("InputProfileName",          inputProfileName);
	builder->get_widget("BtnProfileBackgroundColor", btnProfileBackgroundColor);

	setSignalAdd(btnAdd);
	setSignalApply();

	DialogColors::getInstance()->activateColorButton(btnProfileBackgroundColor);

	// Always on elements selector.
	builder->get_widget("BtnProfilesAddElements",             btnProfilesAddElements);
	builder->get_widget_derived("BoxProfileAlwaysOnElements", boxProfileAlwaysOnElements);
	btnProfilesAddElements->signal_clicked().connect([this]() {
		setUpSelector(COLLECTION_ELEMENT, alwaysOnElementsRequest);
		DialogSelect::getInstance()->open();
	});

	// Always on groups selector.
	builder->get_widget("BtnProfilesAddGroups",             btnProfilesAddGroups);
	builder->get_widget_derived("BoxProfileAlwaysOnGroups", boxProfileAlwaysOnGroups);
	btnProfilesAddGroups->signal_clicked().connect([this]() {
		setUpSelector(COLLECTION_GROUP, alwaysOnGroupsRequest);
		DialogSelect::getInstance()->open();
	});

	// Animations selector.
	builder->get_widget("BtnProfileAddAnimations",      btnProfilesAddAnimations);
	builder->get_widget_derived("BoxProfileAnimations", boxProfileAnimations);
	btnProfilesAddAnimations->signal_clicked().connect([this]() {
		setUpSelector(COLLECTION_ANIMATIONS, animationsRequest);
		DialogSelect::getInstance()->open();
	});

	// Inputs selector.
	builder->get_widget("BtnProfileAddInputs",      btnProfilesAddInputs);
	builder->get_widget_derived("BoxProfileInputs", boxProfileInputs);
	btnProfilesAddInputs->signal_clicked().connect([this]() {
		setUpSelector(COLLECTION_INPUT, inputsRequest);
		DialogSelect::getInstance()->open();
	});

	// Wire SelectionRequests after all boxes are set.
	alwaysOnElementsRequest = {
		boxProfileAlwaysOnElements,
		NAME,
		TYPE_ELEMENT,
		CollectionHandler::getInstance(COLLECTION_ELEMENT),
		{
			{"color",  "Color",  "", Link::LinkField::Widget::COLOR_PICKER},
			{"filter", "Filter", "", Link::LinkField::Widget::COMBOBOX}
		}
	};
	alwaysOnGroupsRequest = {
		boxProfileAlwaysOnGroups,
		NAME,
		"group",
		CollectionHandler::getInstance(COLLECTION_GROUP),
		{
			{"color",  "Color",  "", Link::LinkField::Widget::COLOR_PICKER},
			{"filter", "Filter", "", Link::LinkField::Widget::COMBOBOX}
		}
	};
	animationsRequest = {
		boxProfileAnimations,
		NAME,
		"animation",
		CollectionHandler::getInstance(COLLECTION_ANIMATIONS),
		{}
	};
	inputsRequest = {
		boxProfileInputs,
		NAME,
		"input",
		CollectionHandler::getInstance(COLLECTION_INPUT),
		{}
	};
}

void DialogProfile::load(XMLHelper* values) noexcept {
	createItems(values->getData(COLLECTION_PROFILES), values);
}

void DialogProfile::createSubItems(XMLHelper* values) noexcept {
	auto* ds              = DialogSelect::getInstance();
	const string ownerUid = currentData->createUniqueId();

	setUpSelector(COLLECTION_ELEMENT,    alwaysOnElementsRequest);
	ds->load(values, ownerUid);
	setUpSelector(COLLECTION_GROUP,      alwaysOnGroupsRequest);
	ds->load(values, ownerUid);
	setUpSelector(COLLECTION_ANIMATIONS, animationsRequest);
	ds->load(values, ownerUid);
	setUpSelector(COLLECTION_INPUT,      inputsRequest);
	ds->load(values, ownerUid);
}

void DialogProfile::clearForm() noexcept {
	inputProfileName->set_text("");
	DialogColors::getInstance()->colorizeButton(btnProfileBackgroundColor, DEFAULT_PROFILE_BACKGROUND_COLOR);

	boxProfileAlwaysOnElements->wipe();
	boxProfileAlwaysOnGroups->wipe();
	boxProfileAnimations->wipe();
	boxProfileInputs->wipe();

	btnProfilesAddElements->set_sensitive(CollectionHandler::getInstance(COLLECTION_ELEMENT)->getSize());
	btnProfilesAddGroups->set_sensitive(CollectionHandler::getInstance(COLLECTION_GROUP)->getSize());
	btnProfilesAddInputs->set_sensitive(CollectionHandler::getInstance(COLLECTION_INPUT)->getSize());
	bool hasAnimations = CollectionHandler::getInstance(COLLECTION_ANIMATIONS)->getSize();
	btnProfilesAddAnimations->set_sensitive(hasAnimations);
}

void DialogProfile::isValid() const {
	const string name(createUniqueId());
	if (name.empty()) {
		if (action != Actions::LOAD)
			inputProfileName->grab_focus();
		throw Message("Invalid profile name.");
	}
	if (currentData->getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != name) {
			if (action != Actions::LOAD)
				inputProfileName->grab_focus();
			throw Message("Profile with name " + name + " already exists.");
		}
	}
	if (btnProfileBackgroundColor->get_label().empty())
		throw Message("Select a valid background color.");
}

void DialogProfile::storeData() noexcept {
	currentData->setValue(FILENAME,          inputProfileName->get_text());
	currentData->setValue(BACKGROUND_COLOR,  btnProfileBackgroundColor->get_label());

	auto* ds = DialogSelect::getInstance();
	setUpSelector(COLLECTION_ELEMENT,    alwaysOnElementsRequest);
	ds->reindex();
	setUpSelector(COLLECTION_GROUP,      alwaysOnGroupsRequest);
	ds->reindex();
	setUpSelector(COLLECTION_ANIMATIONS, animationsRequest);
	ds->reindex();
	setUpSelector(COLLECTION_INPUT,      inputsRequest);
	ds->reindex();
}

void DialogProfile::retrieveData() noexcept {
	inputProfileName->set_text(currentData->getValue(FILENAME));
	DialogColors::getInstance()->colorizeButton(
		btnProfileBackgroundColor,
		currentData->getValue(BACKGROUND_COLOR)
	);

	auto* ds = DialogSelect::getInstance();
	setUpSelector(COLLECTION_ELEMENT,    alwaysOnElementsRequest);
	ds->refresh();
	setUpSelector(COLLECTION_GROUP,      alwaysOnGroupsRequest);
	ds->refresh();
	setUpSelector(COLLECTION_ANIMATIONS, animationsRequest);
	ds->refresh();
	setUpSelector(COLLECTION_INPUT,      inputsRequest);
	ds->refresh();
}

string DialogProfile::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({inputProfileName->get_text()});
}

LEDSpicerUI::Ui::Storage::Data* DialogProfile::createData(StringUMap& rawData) const noexcept {
	return new Storage::Profile(rawData, currentDirectory);
}

void DialogProfile::setUpSelector(
	const string& collection,
	const DialogSelect::SelectionRequest& req
) noexcept {
	DialogSelect::getInstance()->setUp(
		static_cast<Storage::Parent*>(currentData)->getChild(collection),
		req
	);
}
