/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDirectory.cpp
 * @since     Mar 2, 2026
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

#include "DialogDirectory.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

void DialogDirectory::isValid() const {
	const string name(Defaults::sanitizeFilename(entryDirectoryName->get_text()));
	if (name.empty()) {
		entryDirectoryName->grab_focus();
		throw Message("Enter a folder name.");
	}
	const string uid(createUniqueId());
	if (getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != uid)
			throw Message("A folder with this name already exists here.");
	}
}

void DialogDirectory::storeData() {
	currentData->setValue(NAME, Defaults::sanitizeFilename(entryDirectoryName->get_text()));
}

void DialogDirectory::retrieveData() {
	entryDirectoryName->set_text(currentData->getValue(NAME));
}

void DialogDirectory::clearForm() {
	entryDirectoryName->set_text("");
}

const string DialogDirectory::createUniqueId() const {
	auto parent = static_cast<const Storage::DirectoryEntry*>(ownerData);
	return Defaults::createCommonUniqueId({
		parent->getFsId(),
		Defaults::sanitizeFilename(entryDirectoryName->get_text())
	});
}

const string DialogDirectory::getType() const {
	return "Directory";
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogDirectory::getCollectionHandler() const {
	return nullptr;
}

//void DialogDirectory::setParent(Storage::DirectoryEntry* parent) {
//	setOwner(&parent->contents, parent);
//}

LEDSpicerUI::Ui::Storage::Data* DialogDirectory::createData(StringUMap& rawData) {
	return new Storage::DirectoryEntry(
		rawData,
		static_cast<const Storage::DirectoryEntry*>(ownerData)
	);
}
