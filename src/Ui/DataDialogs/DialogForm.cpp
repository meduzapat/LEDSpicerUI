/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FormDialog.cpp
 * @since     Feb 15, 2023
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

#include "DialogForm.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogForm::DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(obj) {
	signal_show().connect(sigc::mem_fun(*this, &DialogForm::refreshBox));
}

DialogForm::~DialogForm() {
	delete box;
}

void DialogForm::createItems(vector<unordered_map<string, string>>& rawCollection, XMLHelper* values) {
	mode = Modes::LOAD;
	string errors;
	for (auto& rawItem : rawCollection) {
		clearForm();
		currentData = getData(rawItem);
		// Sanity check by load and unload, this will sanitize (or error out) the data.
		currentData->activate();
		retrieveData();
		try {
			isValid();
		}
		catch (Message& e) {
			delete currentData;
			errors += e.getMessage() + '\n';
			continue;
		}
		storeData();
		// BoxButton will take care for data.
		auto b = items->add(currentData);
		setSignals(b);
		box->add(*b);
		createSubItems(values);
		currentData->deActivate();
	}
	currentData = nullptr;
	if (not errors.empty()) {
		Message::displayError("Errors in " + getType() + ":\n" + errors);
	}
}

void DialogForm::reindex() {
	items->reindex(box);
}

void DialogForm::refreshBox() {
	box->wipe();
	items->populateBox(box);
	box->show_all();
}

void DialogForm::setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) {
	box->wipe();
	DialogForm::owner = owner;
	items = collection;
	items->populateBox(box);
}

void DialogForm::releaseOwner() {
	owner = nullptr;
	items = nullptr;
}

vector<const unordered_map<string, string>*> DialogForm::getValues() {
	vector<const unordered_map<string, string>*> values;
	for (auto b : *items)
		values.push_back(b->getData()->getValues());
	return values;
}

LEDSpicerUI::Ui::Storage::Data* DialogForm::getData() {
	unordered_map<string, string> rawData;
	return getData(rawData);
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

void DialogForm::setSignals(Storage::BoxButton* button) {
	if (button->getData()->canEdit()) {
		button->setEditFn([&, button]() {
			onEditClicked(button);
		});
	}
	if (button->getData()->canDelete()) {
		button->setDelFn([&, button]() {
			if (Message::ask("Are you sure you want to remove " + button->getData()->createPrettyName() + "?") == Gtk::ResponseType::RESPONSE_YES) {
				onDelClicked(button);
			}
		});
	}
}

void DialogForm::onAddClicked() {
	// set dialog to add.
	mode = Modes::ADD;
	// Set label and title.
	set_title("Add New " + getType());
	btnApply->set_label("Create");
	currentData = getData();
	currentData->activate();
	clearForm();
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		storeData();
		Defaults::markDirty();
		// store.
		auto bPtr = items->add(currentData);
		setSignals(bPtr);
		// Add into the box.
		box->add(*bPtr);
		// Custom stuff.
		afterCreate(bPtr);
		currentData->deActivate();
	}
	// Create voided, destroy From.
	else {
		currentData->deActivate();
		delete currentData;
	}
	currentData = nullptr;
	hide();
}

void DialogForm::onEditClicked(Storage::BoxButton* boxButton) {
	mode = Modes::EDIT;
	clearForm();
	currentData = boxButton->getData();
	currentData->activate();
	// Set label and title.
	set_title("Edit " + getType());
	btnApply->set_label("Save");
	// Populate form.
	retrieveData();
	if (run() == Gtk::RESPONSE_APPLY) {
		afterEdit(boxButton);
		Defaults::markDirty();
		// Store data.
		storeData();
		boxButton->updateLabel();
	}
	currentData->deActivate();
	currentData = nullptr;
	hide();
}

void DialogForm::onDelClicked(Storage::BoxButton* boxButton) {
	currentData = boxButton->getData();
	currentData->activate();
	afterDeleteConfirmation(boxButton);
	Defaults::markDirty();
	box->remove(*boxButton);
	// this will also delete the object, the destructor must call deActivate if necessary.
	items->remove(boxButton);
	currentData = nullptr;
}
