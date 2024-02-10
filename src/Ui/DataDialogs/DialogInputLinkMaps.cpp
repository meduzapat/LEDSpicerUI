/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputLinkMap.cpp
 * @since     Oct 1, 2023
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

#include "DialogInputLinkMaps.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInputLinkMaps* DialogInputLinkMaps::instance = nullptr;

vector<string> DialogInputLinkMaps::localCollection;

void DialogInputLinkMaps::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogSortInputLinkMaps", instance);
	}
}

DialogInputLinkMaps* DialogInputLinkMaps::getInstance() {
	return instance;
}

DialogInputLinkMaps::DialogInputLinkMaps(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	builder->get_widget_derived("BoxInputLinkedMaps", box);
	// Create link, will open the dialog in edit mode.
	builder->get_widget("BtnCreateInputLinkedMap",    btnAdd);
	builder->get_widget("BtnApplyInputLinkedMap",     btnApply);

	// Temporary sorting box.
	builder->get_widget_derived(
		"BoxInputLinkedMappings",
		boxInputLinkedMappings,
		"BtnInputMappingUp",
		"BtnInputMappingDown"
	);

	builder->get_widget_derived("BoxInputMap", boxInputMap);

	/*
	 * If two or more selected mappings are selected activate the create link button.
	 */
	boxInputMap->signal_selected_children_changed().connect([&]() {
		btnAdd->set_sensitive(boxInputMap->get_selected_children().size() >= 2);
	});

	/*
	 * Button create linked mapping.
	 * This button will be disabled until at least 2 elements are selected.
	 * It will pick any selected element from the input map box and
	 * create a linked map into linked mappings box.
	 */
	btnAdd->signal_clicked().connect([&]() {
		vector<string>
			linkData,
			ids;
		// Extract selected maps.
		for (auto child : boxInputMap->get_selected_children()) {
			// Extract internal data to get ids and linkdata.
			auto data(dynamic_cast<Storage::BoxButton*>(child->get_child())->getData());
			ids.push_back(Defaults::addUnitSeparator(data->getValue(TRIGGER)));
			linkData.emplace_back(Defaults::createCommonUniqueId({
				data->getValue(TRIGGER),
				data->getValue(TYPE) + " " + data->getValue(TARGET)
			}));
		}
		boxInputMap->unselect_all();
		// ids: (32)4(32)|(32)5(32)|(32)6(32)
		string idsTxt(Defaults::implode(ids, ID_GROUP_SEPARATOR));
		// Check if the link exists.
		if (isUsed(idsTxt)) {
			Message::displayError("This linked mapping already exists");
			return;
		}
		localCollection.push_back(idsTxt);
		// Create linked item.
		unordered_map<string, string> rawData{
			{NAME, Defaults::implode(linkData, RECORD_SEPARATOR)},
			{ID, idsTxt}
		};
		auto map  = getData(rawData);
		auto bPtr = items->add(map);
		// Set signals and store.
		setSignals(bPtr);
		box->add(*bPtr);
	});

	setSignalApply();
}

DialogInputLinkMaps::~DialogInputLinkMaps() {
	instance = nullptr;
}

void DialogInputLinkMaps::load(XMLHelper* values) {
	createItems(values->getData(owner->createUniqueId() + COLLECTION_INPUT_LINKED_MAPS), values);
}

void DialogInputLinkMaps::clearForm() {
	boxInputLinkedMappings->wipe();
	indivitualMaps.wipe();
}

void DialogInputLinkMaps::isValid() const {
	if (isUsed(createUniqueId())) {
		throw Message("This linked mapping already exists.");
	}
}

void DialogInputLinkMaps::storeData() {
	/*
	 * This function will be called to store the sorted linked mappings.
	 */
	vector<string>
		linkData,
		ids;
	string
		oldIds,
		newIds;
	// grab reordered links and store into the linked map object.
	for (auto child : boxInputLinkedMappings->get_children()) {
		auto boxChild = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto bb = dynamic_cast<Storage::BoxButton*>(boxChild->get_child());
		// looks like this trigger(30)type target
		auto newValues(Defaults::explode(bb->getData()->getValue(NAME), FIELD_SEPARATOR));
		ids.push_back(Defaults::addUnitSeparator(newValues[0]));
		linkData.push_back(bb->getData()->getValue(NAME));
	}
	newIds = Defaults::implode(ids, ID_GROUP_SEPARATOR);
	ids.clear();
	// data is trigger(30)type target(31)trigger(30)type target(31)trigger(30)type target
	for (const auto& group : Defaults::explode(currentData->getValue(NAME), RECORD_SEPARATOR)) {
		// group is trigger(30)type target
		const auto parts(Defaults::explode(group, FIELD_SEPARATOR));
		ids.push_back(Defaults::addUnitSeparator(parts.at(0)));
	}
	oldIds = Defaults::implode(ids, ID_GROUP_SEPARATOR);

	// Is always edit
	std::replace(localCollection.begin(), localCollection.end(), oldIds, newIds);

	currentData->wipe();
	currentData->setValue(NAME, Defaults::implode(linkData, RECORD_SEPARATOR));
	currentData->setValue(ID, newIds);
}

void DialogInputLinkMaps::retrieveData() {
	// populates the sorting form with a linked map.
	// data is trigger(30)type target(31)trigger(30)type target(31)trigger(30)type target
	for (const auto& group : Defaults::explode(currentData->getValue(NAME), RECORD_SEPARATOR)) {
		unordered_map<string, string> rawData{{NAME, group}};
		auto form = new Storage::NameOnly(
			rawData,
			"",
			"LinkBoxButton",
			false, false,
			[](const unordered_map<string, string>& data) {
				const auto parts(Defaults::explode(data.at(NAME), FIELD_SEPARATOR));
				return parts.at(1);
			},
			// Tool-tip
			[](const unordered_map<string, string>& data) {
				const auto parts(Defaults::explode(data.at(NAME), FIELD_SEPARATOR));
				return string("Linked map for " + parts.at(1));
			}
		);
		auto button = indivitualMaps.add(form);
		boxInputLinkedMappings->add(*button);
	}
	boxInputLinkedMappings->show_all();
}

const string DialogInputLinkMaps::createUniqueId() const {
	vector<string> ids;
	// grab reordered links and store into the linked map object.
	for (auto child : boxInputLinkedMappings->get_children()) {
		auto boxChild = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto bb = dynamic_cast<Storage::BoxButton*>(boxChild->get_child());
		// data is trigger(30)type target
		auto newValues(Defaults::explode(bb->getData()->getValue(NAME), FIELD_SEPARATOR));
		ids.push_back(Defaults::addUnitSeparator(newValues[0]));
	}
	// ids: (32)4(32)|(32)5(32)|(32)6(32)
	return Defaults::implode(ids, ID_GROUP_SEPARATOR);
}

void DialogInputLinkMaps::setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) {
	DialogForm::setOwner(collection, owner);
	// When the dialog opens, generate the local collection.
	localCollection.clear();
	for(auto bb : *collection)
		localCollection.push_back(extractIds(bb->getData()));
}

const string DialogInputLinkMaps::getType() const {
	return "input Link";
}

LEDSpicerUI::Ui::Storage::Data* DialogInputLinkMaps::getData(unordered_map<string, string>& rawData) {
	return new Storage::InputMapLink(rawData);
}

void DialogInputLinkMaps::afterDeleteConfirmation(Storage::BoxButton* boxButton) {
	string idsTxt(extractIds(boxButton->getData()));
	localCollection.erase(std::remove(localCollection.begin(), localCollection.end(), idsTxt), localCollection.end());
}

bool DialogInputLinkMaps::isUsed(const string& ids) const {
	return std::find(localCollection.begin(), localCollection.end(), ids) != localCollection.end();
}

string DialogInputLinkMaps::extractIds(Storage::Data* data) const {
	vector<string> ids;
	// data is trigger(30)type target(31)trigger(30)type target(31)trigger(30)type target
	for (const auto& group : Defaults::explode(data->getValue(NAME), RECORD_SEPARATOR)) {
		// group is trigger(30)type target
		const auto parts(Defaults::explode(group, FIELD_SEPARATOR));
		ids.push_back(Defaults::addUnitSeparator(parts.at(0)));
	}
	// ids: (32)4(32)|(32)5(32)|(32)6(32)
	return Defaults::implode(ids, ID_GROUP_SEPARATOR);
}
