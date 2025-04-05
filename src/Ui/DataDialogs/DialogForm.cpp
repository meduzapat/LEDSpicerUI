/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FormDialog.cpp
 * @since     Feb 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

DialogForm::DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>&) : Gtk::Dialog(obj) {
	// this needs to be run last,
	signal_show().connect(sigc::mem_fun(*this, &DialogForm::refreshBox), true);
}

DialogForm::~DialogForm() {
	delete box;
}

void DialogForm::createItems(StringUMapVector& rawCollection, XMLHelper* values) {
	// Very similar to ADD but it only uses the form to validate data,
	// also items are not added to the box.
	action = Actions::LOAD;
	string errors;
	for (auto& rawItem : rawCollection) {
		resetForm();
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
		Message::displayError("Errors in " + getType() + ":\n" + errors);
	}
}

void DialogForm::refreshBox() {
	if (childDialog) {
		childDialog->refreshBox();
	}
	box->wipe();
	if (not items) return;
	items->populateBox(box);
	box->show_all();
}

void DialogForm::setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) {
	this->ownerData = owner;
	items = collection;
}

void DialogForm::resetForm() {
	clearForm();
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

LEDSpicerUI::Ui::Storage::Data* DialogForm::createData() {
	StringUMap rawData;
	return createData(rawData);
}

void DialogForm::setSignalAdd() {
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogForm::onAddClicked));
}

void DialogForm::setSignalApply() {
	btnApply->signal_clicked().connect([&]() {
		try {
			// Apply changes after sanitize.
			isValid();
			response(Gtk::RESPONSE_APPLY);
		}
		catch (Message& e) {
			e.displayError(this);
		}
	});
}

void DialogForm::createDeleteButton(Storage::BoxButton& boxButton, bool askConfirmation) {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("edit-delete", Gtk::ICON_SIZE_BUTTON);
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
	button->set_image_from_icon_name("applications-engineering", Gtk::ICON_SIZE_BUTTON);
	button->signal_clicked().connect([&]() {
		onEditClicked(boxButton);
	});
}

void DialogForm::createCloneButton(Storage::BoxButton& boxButton) {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton.pack_start(*button, Gtk::PACK_SHRINK);
	button->set_image_from_icon_name("edit-copy", Gtk::ICON_SIZE_BUTTON);
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
	// set dialog to add.
	action = Actions::ADD;
	resetForm();
	// Set label and title.
	set_title("Add New " + getType());
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
		// store.
		Storage::BoxButton& bBox(items->create(currentData));
		getCollectionHandler()->add(currentData);
		addButtons(bBox);
		// Add into the box.
		box->add(bBox);
		// Custom stuff.
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
	/*
	 * Steps:
	 * 1 Set the action to edit and reset the form
	 * 2 extract payload from the boxbutton and activate any children.
	 * 3 store the current unique id and set the dialog texts and fields.
	 * 4 run the dialog and wait for the apply signal (4a) or cancel (5).
	 * 4a clean previous data and store new values, update collection and label.
	 * 5 deactivate and hide.
	 */
	action = Actions::EDIT;
	resetForm();
	currentData = boxButton.getData();
	currentData->activate();
	string oldName(currentData->createUniqueId());
	// Set label and title.
	set_title("Edit " + getType() + " " + currentData->createPrettyName());
	btnApply->set_label("Save");
	// Populate form.
	retrieveData();
	if (run() == Gtk::RESPONSE_APPLY) {
		Defaults::markDirty();
		currentData->wipe();
		storeData();
		getCollectionHandler()->replace(currentData, oldName);
		boxButton.updateLabel();
		// This will reindex the box located on this dialog but that is handled by its children dialog.
		if (childDialog) childDialog->reindex();
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
	resetForm();
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

	currentData = tempData;
	// Add item and the box and set buttons.
	Storage::BoxButton& bBox(items->create(currentData));
	addButtons(bBox);
	box->add(bBox);
	getCollectionHandler()->add(currentData);
	Defaults::markDirty();
	bBox.updateLabel();
	currentData->deActivate();
	currentData = nullptr;
}
