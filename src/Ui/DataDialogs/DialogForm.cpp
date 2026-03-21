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

DialogForm::DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(obj) {
	DialogColors::buildInstance(builder, "DialogColors");
}

DialogForm::~DialogForm() {
	delete box;
}

void DialogForm::createItems(StringUMapVector& rawCollection, XMLHelper* values) {

	string errors;
	for (auto& rawItem : rawCollection) {
		action = Actions::LOAD;
		clearForm();
		currentData = createData(rawItem);
		currentData->activate();
		// Sanity check by load and unload, this will sanitize (or error out) the data.
		retrieveData();
		try {
			isValid();
		}
		catch (Message& e) {
			delete currentData;
			errors += e.getMessage() + '\n';
			Defaults::markDirty();
			continue;
		}
		// This will clean any anomaly.
		currentData->wipe();
		storeData();
		// BoxButton will take care for data.
		Storage::BoxButton& b(items->create(currentData));
		getCollectionHandler()->add(currentData);
		addButtons(b);
		createSubItems(values);
		currentData->deActivate();
	}
	// Force a refresh to clean any box handle by main process and display the loaded items.
	currentData = nullptr;
	if (not errors.empty()) {
		Message::displayError("Errors in " + string(getType()) + ":\n" + errors);
	}
}

void DialogForm::refreshItems() {
	box->wipe();
	// No items no work.
	if (not items) return;
	items->populateBox(box);
	box->show_all();
	// this will avoid chaining into uninitialized dialogs.
	if (not currentData) return;
	for (auto childDialog : childDialogs) childDialog->refreshItems();
}

void DialogForm::setOwner(Storage::BoxButtonCollection* collection, const Storage::Data* owner) {
	this->ownerData = owner;
	items = collection;
	refreshItems();
}

void DialogForm::resetForm() {
	refreshItems();
}

vector<const StringUMap*> DialogForm::getValues() {
	vector<const StringUMap*> values;
	for (const auto& b : *items)
		values.push_back(b->getData()->getValues());
	return values;
}

void DialogForm::reindex() {
	items->reindex(box);
}

LEDSpicerUI::Ui::OrdenableFlowBox* DialogForm::getBox() {
	return box;
}

LEDSpicerUI::Ui::Storage::Data* DialogForm::createData() {
	return createData(Storage::Data::createEmptyData());
}

void DialogForm::setSignalAdd(Gtk::Button* btnAdd) {
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogForm::onAddClicked));
}

void DialogForm::setSignalAddTo(Gtk::Button* btnAdd, DialogForm* dialogToOpen) {
	btnAdd->signal_clicked().connect(sigc::mem_fun(*dialogToOpen, &DialogForm::onAddClicked));
}

void DialogForm::setSignalApply() {
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

void DialogForm::createDeleteButton(Storage::BoxButton& boxButton, bool askConfirmation) {
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

void DialogForm::createEditButton(Storage::BoxButton& boxButton) {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("emblem-system-symbolic", Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class("BoxBackgroundEdit");
	button->set_tooltip_text("Edit " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&]() {
		onEditClicked(boxButton);
	});
}

void DialogForm::createCloneButton(Storage::BoxButton& boxButton) {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("edit-copy-symbolic", Gtk::ICON_SIZE_BUTTON);
	button->get_style_context()->add_class("BoxBackgroundCopy");
	button->set_tooltip_text("Clone " + boxButton.getData()->createPrettyName());
	button->signal_clicked().connect([&]() {
		onCloneClicked(boxButton);
	});
}

void DialogForm::addButtons(Storage::BoxButton& boxButton) {
	createEditButton(boxButton);
	createDeleteButton(boxButton);
	boxButton.show_all();
}

void DialogForm::onAddClicked() {
	action = Actions::ADD;
	clearForm();
	set_title("Add New " + string(getType()));
	btnApply->set_label("Create");
	currentData = createData();
	currentData->activate();
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		try {
			isValid();
		}
		catch (Message& e) {
			delete currentData;
			e.displayError(this);
			Defaults::markDirty();
			currentData = nullptr;
			hide();
			return;
		}
		// This will clean any anomaly.
		currentData->wipe();
		storeData();
		Defaults::markDirty();
		// Update UI, trackers, and add buttons.
		Storage::BoxButton& bBox(items->create(currentData));
		getCollectionHandler()->add(currentData);
		addButtons(bBox);
		box->add(bBox);
		afterCreate(bBox);
		currentData->deActivate();
	}
	// Create voided, destroy form.
	else {
		currentData->deActivate();
		delete currentData;
	}
	currentData = nullptr;
	hide();
}

void DialogForm::onEditClicked(Storage::BoxButton& boxButton) {
	action = Actions::EDIT;
	clearForm();
	currentData = boxButton.getData();
	currentData->activate();
	string oldName(currentData->createUniqueId());
	// Set label and title.
	set_title("Edit " + string(getType()) + " " + currentData->createPrettyName());
	btnApply->set_label("Save");
	// Populate form.
	retrieveData();
	resetForm();
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		Defaults::markDirty();
		currentData->wipe();
		storeData();
		getCollectionHandler()->replace(currentData, oldName);
		boxButton.updateLabel();
		// This will reindex the box located on this dialog but that is handled by its children dialog.
		for (auto childDialog : childDialogs) childDialog->reindex();
	}
	currentData->deActivate();
	currentData = nullptr;
	hide();
}

void DialogForm::onDelClicked(Storage::BoxButton& boxButton) {
	currentData = boxButton.getData();
	currentData->activate();
	afterDeleteConfirmation(boxButton);
	getCollectionHandler()->remove(currentData);
	Defaults::markDirty();
	box->remove(boxButton);
	// This will also delete the object, the destructor must call deActivate if necessary.
	items->remove(boxButton);
	currentData = nullptr;
}

void DialogForm::onCloneClicked(Storage::BoxButton& boxButton) {
	action = Actions::ADD;
	clearForm();
	// clone the data.
	uint8_t count = 1;
	Storage::Data* tempData = nullptr;
	// Create copies until one that doesn't exist is created.
	do {
		if (tempData) delete tempData;
		StringUMap values(boxButton.getData()->copyValues(count++));
		tempData = createData(values);
	}
	while (getCollectionHandler()->isSet(tempData));

	// Add item and the box and set buttons.
	Storage::BoxButton& bBox(items->create(tempData));
	addButtons(bBox);
	box->add(bBox);
	getCollectionHandler()->add(tempData);
	Defaults::markDirty();
	bBox.updateLabel();
	tempData->deActivate();
	tempData = nullptr;
}
