/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FormDialog.cpp
 * @since     Feb 15, 2023
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

#include "DialogForm.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

std::unordered_map<string, DialogForm*> DialogForm::familyToDialog;

DialogForm::DialogForm(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept : Gtk::Dialog(obj) {

	// Initialize the dialog colors.
	DialogColors::buildInstance(builder, "DialogColors");
}

DialogForm::~DialogForm() {
	for (auto child : childDialogs)
		delete child;
}

void DialogForm::createItems(StringUMapVector& rawCollection, XMLHelper* values) noexcept {

	string errors;
	for (auto& rawItem : rawCollection) {
		action = Actions::LOAD;
		clearForm();
		currentData = createData(rawItem);
		wireChildrenDialogs();
		// Sanity check by load and unload, this will sanitize (or error out) the data.
		retrieveData();
		try {
			isValid();
		}
		catch (Message& e) {
			disconnectChildrenDialogs();
			delete currentData;
			errors += e.getMessage() + '\n';
			Defaults::markDirty();
			continue;
		}
		// This will clean any anomaly.
		currentData->wipe();
		storeData();
		// BoxButton will take care for data.
		Storage::BoxButton& b{items->create(currentData)};
		addButtons(b);
		createSubItems(values);
		disconnectChildrenDialogs();
	}
	currentData = nullptr;
	if (not errors.empty()) {
		Message::displayError("Errors in " + getType() + ":\n" + errors);
	}
}

void DialogForm::setOwner(
	Storage::BoxButtonCollection* collection,
	Storage::Data* owner
) noexcept {
	this->ownerData = owner;
	items = collection;
	refreshItems();
}

void DialogForm::refreshItems() noexcept {
	box->wipe();
	// No items no work to do.
	if (not items) return;
	items->populateBox(box);
//	box->show_all(); // already called in populateBox.
	// Avoid chaining into uninitialized dialogs.
	if (not currentData) return;
	for (auto childDialog : childDialogs) childDialog->refreshItems();
}

void DialogForm::resetForm() noexcept {
	refreshItems();
}

vector<const StringUMap*> DialogForm::getValues() const noexcept {
	vector<const StringUMap*> values;
	for (const auto& b : *items)
		values.push_back(&b->getData()->getValues());
	return values;
}

void DialogForm::reindex() noexcept {
	items->reindex(box);
}

LEDSpicerUI::Ui::Storage::Data* DialogForm::createData() const noexcept {
	auto data{StringUMap{}};
	return createData(data);
}

void DialogForm::wireChildrenDialogs() noexcept {
	currentData->setUp();
	auto parent{dynamic_cast<Storage::Parent*>(currentData)};
	if (not parent) return;
	for (auto& [family, items] : parent->getChildren())
		familyToDialog.at(family)->setOwner(&items, currentData);
}

void DialogForm::disconnectChildrenDialogs() noexcept {
	auto parent{dynamic_cast<Storage::Parent*>(currentData)};
	if (parent)
		for (auto& [family, items] : parent->getChildren())
			familyToDialog.at(family)->setOwner(nullptr, nullptr);
	currentData->tearDown();
}

void DialogForm::setSignalAdd(Gtk::Button* btnAdd) noexcept {
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogForm::onAddClicked));
}

void DialogForm::setSignalAddTo(Gtk::Button* btnAdd, DialogForm* dialogToOpen) noexcept {
	btnAdd->signal_clicked().connect(sigc::mem_fun(*dialogToOpen, &DialogForm::onAddClicked));
}

void DialogForm::setSignalApply() noexcept {
	btnApply->signal_clicked().connect([&]() {
		try {
			// sanitize.
			isValid();
			response(Gtk::ResponseType::RESPONSE_APPLY);
		}
		catch (Message& e) {
			e.displayError(this);
		}
	});
}

void DialogForm::createDeleteButton(
	Storage::BoxButton& boxButton,
	bool askConfirmation
) noexcept {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("user-trash-symbolic", Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class("BoxBackgroundDelete");
	button->set_tooltip_text("Delete " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&, askConfirmation]() {
		if (askConfirmation) {
			if (Message::ask("Are you sure you want to remove " + boxButton.getData()->createPrettyName() + "?") != Gtk::ResponseType::RESPONSE_YES) {
				return;
			}
		}
		onDelClicked(boxButton);
	});
}

void DialogForm::createEditButton(Storage::BoxButton& boxButton) noexcept {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("emblem-system-symbolic", Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class("BoxBackgroundEdit");
	button->set_tooltip_text("Edit " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&]() {
		onEditClicked(boxButton);
	});
}

void DialogForm::createCloneButton(Storage::BoxButton& boxButton) noexcept {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("edit-copy-symbolic", Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class("BoxBackgroundCopy");
	button->set_tooltip_text("Clone " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&]() {
		onCloneClicked(boxButton);
	});
}

void DialogForm::addButtons(Storage::BoxButton& boxButton) noexcept {
	createEditButton(boxButton);
	createDeleteButton(boxButton);
	boxButton.show_all();
}

void DialogForm::onAddClicked() noexcept {
	action = Actions::ADD;
	clearForm();
	set_title("Add New " + getType());
	btnApply->set_label("Create");
	currentData = createData();
	wireChildrenDialogs();
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		// This will clean any anomaly.
		currentData->wipe();
		storeData();
		Defaults::markDirty();
		// Update UI, trackers, and add buttons.
		Storage::BoxButton& bBox(items->create(currentData));
		addButtons(bBox);
		box->add(bBox);
		afterCreate(bBox);
		disconnectChildrenDialogs();
	}
	// Create voided, destroy form.
	else {
		disconnectChildrenDialogs();
		delete currentData;
	}
	currentData = nullptr;
	hide();
}

void DialogForm::onEditClicked(Storage::BoxButton& boxButton) noexcept {
	action = Actions::EDIT;
	clearForm();
	currentData = boxButton.getData();
	wireChildrenDialogs();
	// Set label and title.
	set_title("Edit " + getType() + " " + currentData->createPrettyName());
	btnApply->set_label("Save");
	// Populate form.
	retrieveData();
	resetForm();
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		Defaults::markDirty();
		currentData->wipe();
		storeData();
		boxButton.sync();
		for (auto childDialog : childDialogs) childDialog->reindex();
	}
	disconnectChildrenDialogs();
	currentData = nullptr;
	hide();
}

void DialogForm::onDelClicked(Storage::BoxButton& boxButton) noexcept {

	currentData = boxButton.getData();
//	wireChildrenDialogs();
	afterDeleteConfirmation(boxButton);
	Defaults::markDirty();
	box->remove(boxButton);
//	disconnectChildrenDialogs();
	items->remove(boxButton);
	currentData = nullptr;
}

void DialogForm::onCloneClicked(Storage::BoxButton& boxButton) noexcept {

	action = Actions::ADD;
	clearForm();

	// clone the data.
	StringUMap values(boxButton.getData()->copyValues());
	Storage::Data* tempData{createData(values)};

	// Add item and the box and set buttons.
	Storage::BoxButton& bBox(items->create(tempData));
	addButtons(bBox);
	box->add(bBox);
	Defaults::markDirty();
	bBox.sync();
}

LEDSpicerUI::Ui::Storage::BoxButtonCollection* DialogForm::currentChildren(const string& family) const noexcept {
	return static_cast<LEDSpicerUI::Ui::Storage::Parent*>(currentData)->getChild(family);
}
