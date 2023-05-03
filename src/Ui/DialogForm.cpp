/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FormDialog.cpp
 * @since     Feb 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include <Ui/DialogForm.hpp>

using namespace LEDSpicerUI::Ui;

DialogForm::DialogForm(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(obj) {
	signal_show().connect([&]() {
		box->wipe();
		items.populateBox(box);
	});
}

DialogForm::~DialogForm() {
	delete box;
}

string DialogForm::toXml() {
	string r;
	for (auto b : items)
		r += b->toXML("");
	return r;
}

Forms::BoxButtonCollection* DialogForm::createItems(vector<unordered_map<string, string>>& rawCollection, bool wipe) {
	if (wipe)
		wipeContents();
	for (auto& rawItem : rawCollection) {
		auto b = items.add(getType(), getForm(rawItem));
		setSignals(b);
		box->add(*b);
	}
	box->show_all();
	return &items;
}

void DialogForm::createItems(vector<unordered_map<string, string>>& rawCollection, Forms::BoxButton* owner, bool wipe) {
	mode = Forms::Form::Modes::LOAD;
	owner->getForm()->retrieveData(mode);
	createItems(rawCollection, wipe);
	owner->getForm()->storeData(mode);
}

void DialogForm::getItems(Forms::BoxButtonCollection& collection) {
	items.reindex(box);
	box->wipe();
	collection = std::move(items);
}

void DialogForm::setItems(Forms::BoxButtonCollection& collection) {
	items = std::move(collection);
	items.populateBox(box);
}

void DialogForm::wipeContents() {
	box->wipe();
	items.wipe();
}

vector<const unordered_map<string, string>*> DialogForm::getValues() {
	vector<const unordered_map<string, string>*> values;
	for (auto b : items)
		values.push_back(b->getForm()->getValues());
	return values;
}

void DialogForm::setSignalApply() {
	btnApply->signal_clicked().connect([&]() {
		try {
			// Apply changes after sanitize.
			currentForm->isValid(mode);
			response(Gtk::RESPONSE_APPLY);
		}
		catch (Message& e) {
			e.displayError(this);
		}
	});
}

void DialogForm::setSignals(Forms::BoxButton* button) {
	if (button->getForm()->canEdit()) {
		button->setEditFn([&, button]() {
			onEditClicked(button);
		});
	}
	if (button->getForm()->canDelete()) {
		button->setDelFn([&, button]() {
			if (Message::ask("Are you sure you want to remove " + button->getForm()->createPrettyName() + "?") == Gtk::ResponseType::RESPONSE_YES) {
				onDelClicked(button);
			}
		});
	}
}

void DialogForm::onAddClicked() {
	// set dialog to add.
	mode = Forms::Form::Modes::ADD;
	// Set label and title.
	set_title("Add New " + getType());
	btnApply->set_label("Create");
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		currentForm->storeData(mode);
		// store.
		auto bPtr = items.add(getType(), currentForm);
		setSignals(bPtr);
		// add into the box.
		box->add(*bPtr);
		// Custom stuff.
		afterCreate(bPtr);
	}
	// Create voided, destroy From.
	else if (currentForm) {
		currentForm->cancelData(mode);
		delete currentForm;
	}
	currentForm = nullptr;
	hide();
}

void DialogForm::onEditClicked(Forms::BoxButton* boxButton) {
	currentForm = boxButton->getForm();
	mode        = Forms::Form::Modes::EDIT;
	// Set label and title.
	set_title("Edit " + getType());
	btnApply->set_label("Save");
	// populate form.
	currentForm->retrieveData(mode);
	if (run() == Gtk::RESPONSE_APPLY) {
		afterEdit(boxButton);
		// store data.
		currentForm->storeData(mode);
		boxButton->updateLabel();
	}
	else if (currentForm) {
		currentForm->cancelData(mode);
	}
	currentForm = nullptr;
	hide();
}

void DialogForm::onDelClicked(Forms::BoxButton* boxButton) {
	afterDeleteConfirmation(boxButton);
	box->remove(*boxButton);
	items.remove(boxButton);
}
