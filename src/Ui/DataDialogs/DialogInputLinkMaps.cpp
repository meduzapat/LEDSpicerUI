/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputLinkMaps.cpp
 * @since     Oct 1, 2023
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

#include "DialogInputLinkMaps.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInputLinkMaps::DialogInputLinkMaps(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	DialogForm(obj, builder)
{

	Gtk::Button
		* btnAdd = nullptr,
		* btnSelectInputMap = nullptr;

	builder->get_widget_derived("BoxInputLinkedMaps", box);
	builder->get_widget("BtnApplyInputLinkedMap",     btnApply);
	builder->get_widget("BtnAddInputLinkedMap",       btnAdd);
	builder->get_widget("BtnSelectInputMap",          btnSelectInputMap);
	builder->get_widget_derived(
		"BoxInputLinkedMappings",
		boxInputLinkedMappings,
		"BtnInputMappingUp",
		"BtnInputMappingDown"
	);

	mapsRequest = {
		boxInputLinkedMappings,
		LINKED_ITEMS,
		TYPE_MAP,
		LINKED_ITEMS,
		CollectionHandler::getInstance(COLLECTION_TEMP_MAPS),
		{},
		2
	};

	setSignalAdd(btnAdd);
	setSignalApply();

	btnSelectInputMap->signal_clicked().connect([this]() {
		populateTempMaps();
		DialogSelect::getInstance()->open();
	});

}

void DialogInputLinkMaps::setOwner(
	Storage::BoxButtonCollection* collection,
	Storage::Data* owner
) noexcept {
	DialogForm::setOwner(collection, owner);
	populateTempMaps();
}

void DialogInputLinkMaps::load(XMLHelper* values) noexcept {
	createItems(
		values->getData(
			Defaults::createCommonUniqueId({ownerData->createUniqueId(),
			COLLECTION_INPUT_LINKMAPS})
		),
		values
	);
}

void DialogInputLinkMaps::clearForm() noexcept {
	innerItems.wipe();
	boxInputLinkedMappings->wipe();
}

void DialogInputLinkMaps::isValid() const {
	if (innerItems.getSize() < 2)
		throw Message("Select at least 2 maps.");
}

void DialogInputLinkMaps::storeData() noexcept {
	const auto indexes(DialogSelect::getInstance()->getSelectedIndexes());
	currentData->setValue(LINKED_ITEMS, Defaults::implode(indexes, ID_SEPARATOR));
}

void DialogInputLinkMaps::retrieveData() noexcept {
	const string& idxStr = currentData->getValue(LINKED_ITEMS);
	if (idxStr.empty()) return;
	populateTempMaps();
	DialogSelect::getInstance()->selectByIndexes(Defaults::explode(idxStr, ID_SEPARATOR));
}

string DialogInputLinkMaps::createUniqueId() const noexcept {
	return currentData->createUniqueId();
}

Storage::Data* DialogInputLinkMaps::createData(StringUMap& rawData) const noexcept {
	return new Storage::InputMapLink(rawData);
}

void DialogInputLinkMaps::wireChildrenDialogs() noexcept {
	innerItems.wipe();
	boxInputLinkedMappings->wipe();
	currentData->setUp();
	DialogSelect::getInstance()->setUp(&innerItems, mapsRequest);
	innerItems.registerSensitivity(btnApply, 2);
}

void DialogInputLinkMaps::disconnectChildrenDialogs() noexcept {
	innerItems.releaseSensitive(btnApply);
	innerItems.wipe();
	currentData->tearDown();
}

void DialogInputLinkMaps::populateTempMaps() noexcept {
	auto* tempMaps = CollectionHandler::getInstance(COLLECTION_TEMP_MAPS);

	vector<Storage::Data*> toRemove;
	for (auto& [id, data] : *tempMaps)
		toRemove.push_back(data);
	for (auto* data : toRemove)
		tempMaps->remove(data);

	auto* inputParent = dynamic_cast<Storage::Parent*>(ownerData);
	if (not inputParent) return;
	auto* sourcesBBC = inputParent->getChild(COLLECTION_INPUT_SOURCES);
	if (not sourcesBBC) return;

	for (auto& sourceBB : *sourcesBBC) {
		auto* source = dynamic_cast<Storage::Parent*>(sourceBB->getData());
		if (not source) continue;
		for (auto& mapBB : *source->getPrimaryChild())
			tempMaps->add(mapBB->getData());
}
