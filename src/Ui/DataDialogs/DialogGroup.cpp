/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.cpp
 * @since     Feb 13, 2023
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

#include "DialogGroup.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;
using namespace LEDSpicerUI::Ui::Storage;

DialogGroup::DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder),
	elementRequest{
		nullptr,
		NAME,
		TYPE_ELEMENT,
		COLLECTION_GROUP_LINKS,
		emptyString,
		{},
		CollectionHandler::getInstance(COLLECTION_ELEMENTS),
		{},
		1,
		// Group links are root config data.
		CSS_RO_LOCKED_CONFIG
	}
{
	Gtk::Button
		* btnAdd         = nullptr,
		* btnAddElements = nullptr;

	builder->get_widget_derived("BoxGroups",    box);
	builder->get_widget("BtnAddGroup",          btnAdd);
	builder->get_widget("InputGroupName",       inputGroupName);
	builder->get_widget("BtnGroupDefaultColor", btnGroupDefaultColor);
	builder->get_widget_derived(
		"BoxGroupElements",
		boxElements,
		"BtnGroupElementUp",
		"BtnGroupElementDn",
		"BtnGroupElementFirst",
		"BtnGroupElementLast"
	);
	Defaults::attachNameFilter(inputGroupName);

	setSignalAdd(btnAdd);
	setSignalApply();

	elementRequest.displayBox = boxElements;

	DialogColors::getInstance()->activateColorButton(btnGroupDefaultColor);

	// Element selector button — opens picker with strip-expand support.
	builder->get_widget("BtnAddGroupElements", btnAddElements);
	btnAddElements->signal_clicked().connect([this]() {
		DialogSelect::getInstance()->open();
	});

	CollectionHandler::getInstance(COLLECTION_ELEMENTS)->registerSensitivity(btnAdd);

	// Group name generator.
	Gtk::Button* btnGenerateGroupName = nullptr;
	builder->get_widget("BtnGenerateGroupName", btnGenerateGroupName);
	btnGenerateGroupName->signal_clicked().connect([this]() {
		const string name {DialogPrompt::getInstance()->askGroupName(this)};
		if (name.empty()) return;
		inputGroupName->set_text(name);
		inputGroupName->grab_focus();
	});

	// System groups (PROP_SYSTEM) sort to the front of the groups FlowBox.
	box->set_sort_func(
		[](Gtk::FlowBoxChild* a, Gtk::FlowBoxChild* b) -> int {
			auto rank {[](Gtk::FlowBoxChild* c) {
				return static_cast<Storage::BoxButton*>(c)
					->getData()->getProperties().isSet(PROP_SYSTEM) ? 0 : 1;
			}};
			return rank(a) - rank(b);
		}
	);
}

void DialogGroup::setOwner(BoxButtonCollection* collection, Data* owner) noexcept {
	DialogForm::setOwner(collection, owner);
	// First time the primary collection is wired: build All, register it,
	// and decorate the BoxButton with the edit button (no delete — system).
	if (allGroup) return;
	Values raw {{NAME, GROUP_ALL_NAME}};
	allGroup = new Group(raw);
	allGroup->getProperties().setValue(PROP_SYSTEM, "1");
	BoxButton& btn {items->create(allGroup)};
	createEditButton(btn);
	btn.get_style_context()->add_class(CSS_SYSTEM);
	btn.show_all();
	box->add(btn);
}

void DialogGroup::load(DataMap& values) noexcept {
	auto& rawGroups {values[COLLECTION_GROUPS]};
	auto allLinks  {static_cast<Parent*>(allGroup)->getChild(COLLECTION_GROUP_LINKS)};

	// All insertions below target All's group_links.
	DialogSelect::getInstance()->setUp(allLinks, elementRequest);

	auto allIt {std::find_if(rawGroups.begin(), rawGroups.end(),
		[](const Values& v) { return v.getValue(NAME) == GROUP_ALL_NAME; })};

	if (allIt != rawGroups.end()) {
		// Apply top-level saved fields (DEFAULT_COLOR, etc.) onto existing All.
		for (auto& [k, v] : *allIt)
			if (k != NAME) allGroup->setValue(k, v);
		// Load saved Links in saved order.
		DialogSelect::getInstance()->load(
			values,
			allGroup->createUniqueId(),
			getType() + " " + allGroup->createPrettyName()
		);
		rawGroups.erase(allIt);
	}
	else {
		// No saved All — auto-fill from the elements collection.
		for (auto& [id, e] : *CollectionHandler::getInstance(COLLECTION_ELEMENTS))
			linkInto(GROUP_ALL_NAME, e);
	}

	createItems(rawGroups, values);
}

void DialogGroup::linkInto(const string& groupId, Data* target) noexcept {
	if (target->getProperties().isSet(PROP_NO_SELECT)) return;
	auto group {CollectionHandler::getInstance(COLLECTION_GROUPS)->get(groupId)};
	if (not group) return;
	auto dest {static_cast<Parent*>(group)->getChild(COLLECTION_GROUP_LINKS)};
	DialogSelect::getInstance()->setUp(dest, elementRequest);
	DialogSelect::getInstance()->createLink(target);
}

void DialogGroup::createSubItems(DataMap& values) noexcept {
	DialogSelect::getInstance()->load(
		values,
		currentData->createUniqueId(),
		Defaults::titleCase(getType()) + " " + currentData->createPrettyName()
	);
}

void DialogGroup::clearForm() noexcept {
	inputGroupName->set_text("");
	inputGroupName->set_sensitive(true);
	DialogColors::getInstance()->colorizeButton(btnGroupDefaultColor, NO_COLOR);
	boxElements->wipe();
}

void DialogGroup::isValid() const {
	string name(createUniqueId());
	if (name.empty()) {
		if (action != Actions::LOAD)
			inputGroupName->grab_focus();
		throw Message("Invalid group name.");
	}
	// GROUP_ALL_NAME is owned by the system group; users can't claim it.
	if (action == Actions::ADD and name == GROUP_ALL_NAME) {
		inputGroupName->grab_focus();
		throw Message("The name '" + string(GROUP_ALL_NAME) + "' is reserved for the system group.");
	}
	if (currentData->getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != name) {
			if (action != Actions::LOAD)
				inputGroupName->grab_focus();
			throw Message("Group with name " + name + " already exists.");
		}
	}
}

void DialogGroup::storeData() noexcept {
	currentData->setValue(NAME, inputGroupName->get_text());
	if (not btnGroupDefaultColor->get_label().empty())
		currentData->setValue(DEFAULT_COLOR, btnGroupDefaultColor->get_label());
	DialogSelect::getInstance()->reindex();
}

void DialogGroup::retrieveData() noexcept {
	inputGroupName->set_text(currentData->getValue(NAME));
	// System groups can't be renamed; the rest of the form stays editable.
	inputGroupName->set_sensitive(not currentData->getProperties().isSet(PROP_SYSTEM));
	DialogColors::getInstance()->colorizeButton(
		btnGroupDefaultColor,
		currentData->getValue(DEFAULT_COLOR).empty() ? NO_COLOR : currentData->getValue(DEFAULT_COLOR)
	);
	DialogSelect::getInstance()->refresh();
}

string DialogGroup::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({inputGroupName->get_text()});
}

Data* DialogGroup::createData(Values& rawData) const noexcept {
	return new Group(rawData);
}

void DialogGroup::wireChildrenDialogs() noexcept {
	// This is necessary because Group uses DialogSelect and its not registered.
	currentData->setUp();
	DialogSelect::getInstance()->setUp(getPrimaryChildCollection(), elementRequest);
	getPrimaryChildCollection()->registerSensitivity(btnApply);
}

void DialogGroup::disconnectChildrenDialogs() noexcept {
	getPrimaryChildCollection()->releaseSensitive(btnApply);
	currentData->tearDown();
}
