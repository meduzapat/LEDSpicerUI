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

void DialogForm::createItems(ValueVector& rawCollection, DataMap& values) noexcept {

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
		Storage::BoxButton& boxButton{items->create(currentData)};
		addButtons(boxButton);
		createSubItems(values);
		// updates labels and tooltips after children.
		boxButton.sync();
		disconnectChildrenDialogs();
	}
	currentData = nullptr;
	if (not errors.empty()) {
		Message::displayError("Errors in " + getType() + ":\n" + errors, this);
	}
}

void DialogForm::setOwner(
	Storage::BoxButtonCollection* collection,
	Storage::Data* owner
) noexcept {
	ownerData = owner;
	items     = collection;
}

void DialogForm::removeOwner() noexcept {
	ownerData = nullptr;
	items     = nullptr;
}

void DialogForm::refreshItems() noexcept {
	box->wipe();
	// No items no work to do.
	if (not items) return;
	items->populateBox(box);
	// Avoid chaining into uninitialized dialogs.
	if (not currentData) return;
	for (auto childDialog : childDialogs) childDialog->refreshItems();
}

void DialogForm::resetForm() noexcept {
	refreshItems();
}

void DialogForm::reindex() noexcept {
	items->reindex(box);
}

LEDSpicerUI::Ui::Storage::Data* DialogForm::createData() const noexcept {
	Values data;
	return createData(data);
}

void DialogForm::wireChildrenDialogs() noexcept {
	currentData->setUp();
	auto parent{dynamic_cast<Storage::Parent*>(currentData)};
	if (not parent) return;
	for (auto& [family, items] : parent->getChildren()) {
		familyToDialog.at(family)->setOwner(&items, currentData);
	}

	if (auto pc{getPrimaryChildCollection()}; pc) pc->registerSensitivity(btnApply);
}

void DialogForm::disconnectChildrenDialogs() noexcept {
	auto parent{dynamic_cast<Storage::Parent*>(currentData)};
	if (parent) {
		for (auto& [family, items] : parent->getChildren())
			familyToDialog.at(family)->removeOwner();

		if (auto pc{getPrimaryChildCollection()}; pc) pc->releaseSensitive(btnApply);
	}
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
	button->set_image_from_icon_name(ICON_TRASH, Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class(CSS_BOX_BACKGROUND_DELETE);
	button->set_tooltip_text("Delete " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&, askConfirmation]() {
		if (askConfirmation) {
			if (Message::ask(
					"Are you sure you want to remove " +
					boxButton.getData()->createPrettyName() + "?", this
				) != Gtk::ResponseType::RESPONSE_YES) return;
		}
		onDelClicked(boxButton);
	});
}

void DialogForm::createEditButton(Storage::BoxButton& boxButton) noexcept {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name(ICON_EDIT, Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class(CSS_BOX_BACKGROUND_EDIT);
	button->set_tooltip_text("Edit " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&]() {
		onEditClicked(boxButton);
	});
}

void DialogForm::createCloneButton(Storage::BoxButton& boxButton) noexcept {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name(ICON_COPY, Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class(CSS_BOX_BACKGROUND_COPY);
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

	// Set mode, clear form, set form details.
	action = Actions::ADD;
	clearForm();
	set_title("Add New " + getType());
	btnApply->set_label("Create");

	// Ask form to create an empty Data, and connect any children dialogs.
	currentData = createData();
	wireChildrenDialogs();
	refreshItems();

	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {

		// Data realized, wipe any leftover before save.
		currentData->wipe();

		// Form -> Data.
		storeData();
		Defaults::markDirty();
		// Create Button, will set tracker if applicable.
		Storage::BoxButton& boxButton(items->create(currentData));

		// Add buttons, update UI and call after create callback.
		addButtons(boxButton);
		box->add(boxButton);
		afterCreate(boxButton);
		disconnectChildrenDialogs();
	}
	else {
		// Create voided, destroy temporary data and disconnect children dialogs.
		disconnectChildrenDialogs();
		delete currentData;
	}
	currentData = nullptr;
	hide();
}

void DialogForm::onEditClicked(Storage::BoxButton& boxButton) noexcept {

	// Set mode, clear form, set form details.
	action = Actions::EDIT;
	clearForm();
	currentData = boxButton.getData();
	set_title("Edit " + getType() + " " + currentData->createPrettyName());
	btnApply->set_label("Save");

	// Connect any children dialogs.
	wireChildrenDialogs();
	refreshItems();
	// Data -> Form.
	retrieveData();
	// Modify the form based on the current Data.
	resetForm();

	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {

		// Edit confirmed.
		Defaults::markDirty();

		// Store old Id for update tracker.
		auto oldId{currentData->createUniqueId()};

		// Wipe any leftover.
		currentData->wipe();

		// Form -> Data.
		storeData();
		// Call tracker update.
		currentData->syncRegistration(oldId);

		// Update UI.
		boxButton.sync();

		// Sort items based on user input.
		for (auto childDialog : childDialogs) childDialog->reindex();
	}
	disconnectChildrenDialogs();
	currentData = nullptr;
	hide();
}

void DialogForm::onDelClicked(Storage::BoxButton& boxButton) noexcept {

	// Delete started.
	currentData = boxButton.getData();
	// Ask confirmation.
	afterDeleteConfirmation(boxButton);
	Defaults::markDirty();
	// Remove from UI.
	box->remove(boxButton);
	// Remove from collection, this will trigger any tracker and chain delete.
	items->remove(boxButton);
	currentData = nullptr;
}

void DialogForm::onCloneClicked(Storage::BoxButton& boxButton) noexcept {

	// Clone started, set mode, clear form.
	action = Actions::ADD;
	clearForm();

	// Clone Data, will return a copy with a different and unique ID.
	Values values(boxButton.getData()->copyValues());

	// Ask dialog to create a new Data with the cloned values.
	Storage::Data* tempData {createData(values)};

	// Create button, will set tracker if applicable.
	Storage::BoxButton& newBoxButton(items->create(tempData));
	Defaults::markDirty();

	// Update UI and call after create callback.
	addButtons(newBoxButton);
	box->add(newBoxButton);
	afterCreate(newBoxButton);
}

LEDSpicerUI::Ui::Storage::BoxButtonCollection* DialogForm::getPrimaryChildCollection() const noexcept {
	auto p{dynamic_cast<LEDSpicerUI::Ui::Storage::Parent*>(currentData)};
	if (not p) return nullptr;
	return p->getPrimaryChild();
}

LEDSpicerUI::Ui::Storage::BoxButtonCollection* DialogForm::getChildCollection(const string& family) const noexcept {
	return static_cast<LEDSpicerUI::Ui::Storage::Parent*>(currentData)->getChild(family);
}
