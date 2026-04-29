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
	DialogForm(obj, builder),
	mapsRequest{
		nullptr,
		TRIGGER,
		TYPE_MAP,
		COLLECTION_INPUT_MAP_LINKS,
		// the correct scope for filtering maps per input UID is IID
		IID,
		{},
		CollectionHandler::getInstance(COLLECTION_INPUT_MAPS),
		{},
		2
	}
{
	Gtk::Button
		* btnAdd            = nullptr,
		* btnSelectInputMap = nullptr;

	builder->get_widget_derived("BoxInputLinkedMaps",  box);
	builder->get_widget("BtnApplyInputLinkedMap",      btnApply);
	builder->get_widget("BtnAddInputLinkedMap",        btnAdd);
	builder->get_widget("BtnSelectInputMap",           btnSelectInputMap);
	builder->get_widget_derived(
		"BoxInputLinkedMappings",
		boxInputLinkedMappings,
		"BtnInputMappingUp",
		"BtnInputMappingDown"
	);

	mapsRequest.displayBox = boxInputLinkedMappings;

	setSignalAdd(btnAdd);
	setSignalApply();

	mapsRequest.sourceCollection->registerSensitivity(btnApply, 2);

	btnSelectInputMap->signal_clicked().connect([this]() {
		DialogSelect::getInstance()->open();
	});
}

void DialogInputLinkMaps::setOwner(
	Storage::BoxButtonCollection* collection,
	Storage::Data* owner
) noexcept {
	DialogForm::setOwner(collection, owner);
	mapsRequest.sourceCollection = CollectionHandler::getInstance(COLLECTION_INPUT_MAPS);
	mapsRequest.filterValue      = ownerData->getProperties().getValue(UID);
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

void DialogInputLinkMaps::createSubItems(XMLHelper*) noexcept {
	const string& idxStr{currentData->getValue(LINKED_ITEMS)};
	if (idxStr.empty()) return;
	mapsRequest.sourceCollection = CollectionHandler::getInstance(COLLECTION_INPUT_MAPS);
	mapsRequest.filterValue      = ownerData->getProperties().getValue(UID);
	DialogSelect::getInstance()->selectByIndexes(Defaults::explode(idxStr, ','));
}

void DialogInputLinkMaps::clearForm() noexcept {
	boxInputLinkedMappings->wipe();
}

void DialogInputLinkMaps::isValid() const {
	if (getPrimaryChildCollection()->getSize() < 2)
		throw Message("Select at least 2 maps.");
}

void DialogInputLinkMaps::storeData() noexcept {
	DialogSelect::getInstance()->reindex();
}

void DialogInputLinkMaps::retrieveData() noexcept {
	DialogSelect::getInstance()->refresh();
}

string DialogInputLinkMaps::createUniqueId() const noexcept {
	return currentData->createUniqueId();
}

LEDSpicerUI::Ui::Storage::Data* DialogInputLinkMaps::createData(StringUMap& rawData) const noexcept {
	return new Storage::InputMapLink(rawData, ownerData->getProperties().getValue(UID));
}

void DialogInputLinkMaps::wireChildrenDialogs() noexcept {
	currentData->setUp();
	DialogSelect::getInstance()->setUp(getPrimaryChildCollection(), mapsRequest);
	getPrimaryChildCollection()->registerSensitivity(btnApply, 2);

	// On ADD with no pre-existing links, open DS immediately.
	if (action == Actions::ADD and getPrimaryChildCollection()->getSize() == 0)
		DialogSelect::getInstance()->open();
}

void DialogInputLinkMaps::disconnectChildrenDialogs() noexcept {
	getPrimaryChildCollection()->releaseSensitive(btnApply);
	currentData->tearDown();
}
