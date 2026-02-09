/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.cpp
 * @since     Feb 22, 2023
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

#include "DialogSelect.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogSelect* DialogSelect::instance = nullptr;

void DialogSelect::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogSelect", instance);
	}
}

DialogSelect* DialogSelect::getInstance() {
	return instance;
}

DialogSelect::DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(obj) {
	Gtk::Button
		* btnSelectAll,
		* btnSelectNone;
	Gtk::SearchEntry* filterEntry;
	// Box and btnAdd will be set on destination.
	builder->get_widget("BntProcessSelection", btnApply);
	builder->get_widget("BoxAll",              boxAll);
	builder->get_widget("BtnSelectAll",        btnSelectAll);
	builder->get_widget("BtnSelectNone",       btnSelectNone);
	builder->get_widget("entryFilterSelects",  filterEntry);

	btnSelectAll->signal_clicked().connect(sigc::mem_fun(*boxAll, &Gtk::FlowBox::select_all));

	btnSelectNone->signal_clicked().connect(sigc::mem_fun(*boxAll, &Gtk::FlowBox::unselect_all));

	// Set filter and apply signals.
	Defaults::setFilter(filterEntry, boxAll);

	signal_show().connect(sigc::mem_fun(*this, &DialogSelect::populateSelectables), true);
}

void DialogSelect::setDestinations(const StringBoxButtonCollectionUMap& itemCollections, const Storage::Data* caller) {
	this->itemCollections = itemCollections;
	this->caller          = caller;
}

void DialogSelect::setSettings(const SettingRequest& setting) {
	this->setting = &setting;
}

void DialogSelect::refresh() {
	setting->workingBox->wipe();
	for (auto& item : *getItemCollection()) {
		setting->workingBox->add(*item);
	}
	setting->workingBox->show_all();
}

void DialogSelect::reindex() {
	getItemCollection()->reindex(setting->workingBox);
}

void DialogSelect::runSelection() {
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		auto items(getItemCollection());
		auto box(setting->workingBox);
		items->wipe();
		box->wipe();
		for (auto* child : boxAll->get_selected_children()) {
			// Get the Selection object directly from the FlowBoxChild
			auto selection = dynamic_cast<Storage::Selection*>(child->get_child());
			// Link data.
			StringUMap data;
			Storage::Link* link      = new Storage::Link(data, setting->type, setting->parameter, selection->getData());
			Storage::BoxButton& bBox = items->create(link);
			addButtons(bBox);
			box->add(bBox);
		}
		box->show_all();
	}
	hide();
}

void DialogSelect::load(XMLHelper* values, const string& type) {
	StringUMapVector& rawCollection = values->getData(Defaults::createCommonUniqueId({caller->createUniqueId(), type}));
	string
		location(" in " + setting->type + " while loading " + caller->createPrettyName()),
		errors;
	for (auto& rawItem : rawCollection) {
		Storage::Link* link = nullptr;
		try {
			// Check if the provider parameter is set.
			if (rawItem.find(setting->parameter) == rawItem.end())
				throw Message("Missing " + setting->parameter + location);
			// Get the linked data.
			auto data = getCollection()->get(rawItem.at(setting->parameter));
			if (not data)
				throw Message("Cannot find " + setting->type + " with " + setting->parameter + " " + rawItem[setting->parameter] + location);
			rawItem.erase(setting->parameter);
			// TODO: add buttons values if any.
			link = new Storage::Link(rawItem, setting->type, setting->parameter, data);
		}
		catch (Message& e) {
			errors += e.getMessage() + '\n';
			Defaults::markDirty();
			continue;
		}
		// BoxButton will take care for data.
		Storage::BoxButton& b(itemCollections.at(setting->type)->create(link));
		addButtons(b);
		setting->workingBox->add(b);
	}
	if (not errors.empty()) {
		Message::displayError("Loading errors:\n" + errors);
	}
}

size_t DialogSelect::getNumberOfSelections() const {
	return boxAll->get_selected_children().size();
}

size_t DialogSelect::getNumberOfSelectables() const {
	return setting->workingBox->get_children().size();
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogSelect::getCollection() const {
	return CollectionHandler::getInstance(setting->sourceCollection);
}

LEDSpicerUI::Ui::Storage::BoxButtonCollection* DialogSelect::getItemCollection() const {
	return itemCollections.at(setting->type);
}


void DialogSelect::addButtons(Storage::BoxButton& boxButton) {
	const SettingRequest* currentSettings = setting;
	const Storage::Data*  currentCaller   = caller;
	if (setting->buttons & BUTTON_EDITER) {
		auto button(Gtk::make_managed<Gtk::Button>());
		boxButton.pack_start(*button, Gtk::PACK_SHRINK);
		button->set_image_from_icon_name("applications-engineering", Gtk::ICON_SIZE_BUTTON);
		button->signal_clicked().connect([&, currentSettings, currentCaller]() {
			setting = currentSettings;
			setSettings(*currentSettings);
			boxButton.getData()->activate();
			DialogSelect::getInstance()->refresh();
			runSelection();
		});
	}
	if (setting->buttons & BUTTON_COLORER) {
		auto button(Gtk::make_managed<Gtk::Button>());
		boxButton.pack_start(*button, Gtk::PACK_SHRINK);
		DialogColors::getInstance()->activateColorButton(button);
		// TODO: add buttons values.
	}
	if (setting->buttons & BUTTON_DELETER) {
		auto button(Gtk::make_managed<Gtk::Button>());
		boxButton.pack_start(*button, Gtk::PACK_SHRINK);
		button->set_image_from_icon_name("edit-delete", Gtk::ICON_SIZE_BUTTON);
		button->signal_clicked().connect([&, currentSettings]() {
			Defaults::markDirty();
			currentSettings->workingBox->remove(boxButton);
			itemCollections.at(currentSettings->type)->remove(boxButton);
		});
	}
	boxButton.show_all();
}

void DialogSelect::populateSelectables() {
	for (auto child : boxAll->get_children()) {
		boxAll->remove(*child);
	}
	// Populate with new selections.
	for (const auto& collection : *getCollection()) {

		Storage::Selection* selection = Gtk::make_managed<Storage::Selection>(collection.second);
		Gtk::FlowBoxChild*  flowChild = Gtk::make_managed<Gtk::FlowBoxChild>();

		// Connect the button click signal.
		selection->signal_clicked().connect([this, flowChild, selection]() {
			if (flowChild->is_selected()) {
				boxAll->unselect_child(*flowChild);
			}
			else {
				boxAll->select_child(*flowChild);
			}
		});

		// Add the button to the flow child and flow child to the box.
		flowChild->add(*selection);
		boxAll->add(*flowChild);
		// Set initial selection state.
		if (getItemCollection()->isSet(collection.second)) {
			boxAll->select_child(*flowChild);
		}
	}

	// Show all children.
	boxAll->show_all();
}
