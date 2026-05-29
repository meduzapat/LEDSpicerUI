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
	DialogForm(obj, builder),
	alwaysOnElementsRequest{
		nullptr,
		NAME,
		TYPE_ELEMENT,
		COLLECTION_PROFILE_ELEMENTS,
		emptyString,
		{},
		CollectionHandler::getInstance(COLLECTION_ELEMENTS),
		{
			{"color",  "Color",  "", Link::LinkField::Widget::COLOR_PICKER},
			{"filter", "Filter", "", Link::LinkField::Widget::COMBOBOX}
		}
	},
	alwaysOnGroupsRequest{
		nullptr,
		NAME,
		TYPE_GROUP,
		COLLECTION_PROFILE_GROUPS,
		emptyString,
		{},
		CollectionHandler::getInstance(COLLECTION_GROUPS),
		{
			{"color",  "Color",  "", Link::LinkField::Widget::COLOR_PICKER},
			{"filter", "Filter", "", Link::LinkField::Widget::COMBOBOX}
		}
	},
	animationsRequest{
		nullptr,
		NAME,
		TYPE_ANIMATION,
		COLLECTION_PROFILE_ANIMATIONS,
		emptyString,
		{},
		CollectionHandler::getInstance(COLLECTION_ANIMATIONS),
		{}
	},
	inputsRequest{
		nullptr,
		NAME,
		TYPE_INPUT,
		COLLECTION_PROFILE_INPUTS,
		emptyString,
		{},
		CollectionHandler::getInstance(COLLECTION_INPUTS),
		{}
	}
{

	Gtk::Button
		* btnProfilesAddElements   = nullptr,
		* btnProfilesAddGroups     = nullptr,
		* btnProfilesAddAnimations = nullptr,
		* btnProfilesAddInputs     = nullptr,
		* btnAdd                   = nullptr;

	builder->get_widget_derived("BoxProfiles",       box);
	builder->get_widget("BtnApplyProfile",           btnApply);
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
		setUpSelector(alwaysOnElementsRequest);
		DialogSelect::getInstance()->open();
	});

	// Always on groups selector.
	builder->get_widget("BtnProfilesAddGroups",             btnProfilesAddGroups);
	builder->get_widget_derived("BoxProfileAlwaysOnGroups", boxProfileAlwaysOnGroups);
	btnProfilesAddGroups->signal_clicked().connect([this]() {
		setUpSelector(alwaysOnGroupsRequest);
		DialogSelect::getInstance()->open();
	});

	// Animations selector.
	builder->get_widget("BtnProfileAddAnimations",      btnProfilesAddAnimations);
	builder->get_widget_derived(
		"BoxProfileAnimations",
		boxProfileAnimations,
		"BtnProfileAnimationUp",
		"BtnProfileAnimationDn"
	);
	btnProfilesAddAnimations->signal_clicked().connect([this]() {
		setUpSelector(animationsRequest);
		DialogSelect::getInstance()->open();
	});

	// Inputs selector.
	builder->get_widget("BtnProfileAddInputs",      btnProfilesAddInputs);
	builder->get_widget_derived("BoxProfileInputs", boxProfileInputs);
	btnProfilesAddInputs->signal_clicked().connect([this]() {
		setUpSelector(inputsRequest);
		DialogSelect::getInstance()->open();
	});

	// Wire displayBox to all requests.
	alwaysOnElementsRequest.displayBox = boxProfileAlwaysOnElements;
	alwaysOnGroupsRequest.displayBox   = boxProfileAlwaysOnGroups;
	animationsRequest.displayBox       = boxProfileAnimations;
	inputsRequest.displayBox           = boxProfileInputs;

	// Register sensitives.
	CollectionHandler::getInstance(COLLECTION_ELEMENTS   )->registerSensitivity(btnProfilesAddElements);
	CollectionHandler::getInstance(COLLECTION_GROUPS     )->registerSensitivity(btnProfilesAddGroups);
	CollectionHandler::getInstance(COLLECTION_ANIMATIONS )->registerSensitivity(btnProfilesAddAnimations);
	CollectionHandler::getInstance(COLLECTION_INPUTS     )->registerSensitivity(btnProfilesAddInputs);

}

void DialogProfile::load(DataMap& values) noexcept {
	createItems(
		values[Defaults::createCommonUniqueId({currentDirectory->getFullPath(), COLLECTION_PROFILES})],
		values
	);
}

void DialogProfile::createSubItems(DataMap& values) noexcept {
	auto ds{DialogSelect::getInstance()};
	const string& ownerUid {currentData->getProperties().getValue(PATH_BASE)};

	// Rewrite NAME paths to uniqueIds so DialogSelect::load resolves them.
	const auto scoped = [&ownerUid](const string& family) {
		return Defaults::createCommonUniqueId({ownerUid, family});
	};
	resolvePaths(values[scoped(COLLECTION_PROFILE_ANIMATIONS)], COLLECTION_ANIMATIONS);
	resolvePaths(values[scoped(COLLECTION_PROFILE_INPUTS)],     COLLECTION_INPUTS);

	setUpSelector(alwaysOnElementsRequest);
	ds->load(values, ownerUid);
	setUpSelector(alwaysOnGroupsRequest);
	ds->load(values, ownerUid);
	setUpSelector(animationsRequest);
	ds->load(values, ownerUid);
	setUpSelector(inputsRequest);
	ds->load(values, ownerUid);
}

void DialogProfile::clearForm() noexcept {
	inputProfileName->set_text("");
	DialogColors::getInstance()->colorizeButton(btnProfileBackgroundColor, DEFAULT_PROFILE_BACKGROUND_COLOR);

	boxProfileAlwaysOnElements->wipe();
	boxProfileAlwaysOnGroups->wipe();
	boxProfileAnimations->wipe();
	boxProfileInputs->wipe();
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
	currentData->getProperties().setValue(FILENAME, inputProfileName->get_text());
	currentData->setValue(BACKGROUND_COLOR, btnProfileBackgroundColor->get_label());

	auto ds{DialogSelect::getInstance()};
	setUpSelector(alwaysOnElementsRequest);
	ds->reindex();
	setUpSelector(alwaysOnGroupsRequest);
	ds->reindex();
	setUpSelector(animationsRequest);
	ds->reindex();
	setUpSelector(inputsRequest);
	ds->reindex();
}

void DialogProfile::retrieveData() noexcept {
	inputProfileName->set_text(currentData->getProperties().getValue(FILENAME));
	DialogColors::getInstance()->colorizeButton(
		btnProfileBackgroundColor,
		currentData->getValue(BACKGROUND_COLOR)
	);

	auto ds{DialogSelect::getInstance()};
	setUpSelector(alwaysOnElementsRequest);
	ds->refresh();
	setUpSelector(alwaysOnGroupsRequest);
	ds->refresh();
	setUpSelector(animationsRequest);
	ds->refresh();
	setUpSelector(inputsRequest);
	ds->refresh();
}

string DialogProfile::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		currentData->getProperties().getValue(PID),
		inputProfileName->get_text()
	});
}

LEDSpicerUI::Ui::Storage::Data* DialogProfile::createData(Values& rawData) const noexcept {
	auto* data {new Storage::Profile(rawData, currentDirectory)};
	// Rescue PATH_BASE to be used in loading process.
	if (action == Actions::LOAD)
		data->getProperties().setValue(PATH_BASE, data->getValue(PATH_BASE));
	return data;
}

void DialogProfile::setUpSelector(const DialogSelect::SelectionRequest& req) noexcept {
	DialogSelect::getInstance()->setUp(getChildCollection(req.collectionId), req);
}

void DialogProfile::resolvePaths(ValueVector& items, const string& sourceCollectionId) noexcept {
	auto* ch {CollectionHandler::getInstance(sourceCollectionId)};
	for (auto& raw : items) {
		const string path {raw.getValue(NAME)};
		for (auto& [id, data] : *ch) {
			auto* node {dynamic_cast<const Storage::DirNode*>(data)};
			if (node and node->getFullPath() == path) {
				raw.setValue(NAME, id);
				break;
			}
		}
	}
}

void DialogProfile::wireChildrenDialogs() noexcept {
	// Selectors bound lazily via setUpSelector().
	currentData->setUp();
}

void DialogProfile::disconnectChildrenDialogs() noexcept {
	currentData->tearDown();
}
