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

DialogDirectory::DialogDirectory(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder)
{

	builder->get_widget("EntryDirectoryName", entryDirectoryName);
	builder->get_widget("BtnApplyDirectory",  btnApply);

	setSignalApply();
	entryDirectoryName->signal_changed().connect([this]() {
		btnApply->set_sensitive(not Defaults::sanitizeFilename(entryDirectoryName->get_text()).empty());
	});
}

void DialogDirectory::setSettings(const SettingRequest& req) noexcept {
	setting = &req;
	box     = req.box;
}

void DialogDirectory::clearForm() noexcept {
	entryDirectoryName->set_text("");
	btnApply->set_sensitive(false);
}

void DialogDirectory::isValid() const {
	string name(Defaults::sanitizeFilename(entryDirectoryName->get_text()));
	if (name.empty()) {
		entryDirectoryName->grab_focus();
		throw Message("Enter a folder name.");
	}

	string uid(createUniqueId());
	if (currentData->getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != uid)
			throw Message("A folder with this name already exists here.");
	}
}

void DialogDirectory::storeData() noexcept {
	currentData->getProperties().setValue(FILENAME, Defaults::sanitizeFilename(entryDirectoryName->get_text()));
}

void DialogDirectory::retrieveData() noexcept {
	entryDirectoryName->set_text(currentData->getProperties().getValue(FILENAME));
}

string DialogDirectory::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		ownerData->getProperties().getValue(UID),
		Defaults::sanitizeFilename(entryDirectoryName->get_text())
	});
}

const string& DialogDirectory::getType() const noexcept {
	return setting->typeLabel;
}

void DialogDirectory::load(DataMap& values) noexcept {

	// Clean up any cache
	dirByPath.clear();

	// Set Root
	auto root{static_cast<Storage::DirectoryEntry*>(ownerData)};
	dirByPath[""] = root;

	// Point to store in root.
	setting->fileDialog->setOwner(root->getPrimaryChild(), ownerData);
	dynamic_cast<DirectoryAware*>(setting->fileDialog)->setCurrentDirectory(root);

	// load root items
	setting->fileDialog->load(values);

	// load other directories.
	createItems(values[COLLECTION_DIRECTORIES], values);
}

LEDSpicerUI::Ui::Storage::Data* DialogDirectory::createData(Values& rawData) const noexcept {
	auto parent {dirByPath.at(rawData.getValue(PATH_PARENT))};
	auto de{new Storage::DirectoryEntry(rawData, parent)};
	dirByPath[de->getFullPath()] = de;
	return de;
}

void DialogDirectory::wireChildrenDialogs() noexcept {
	currentData->setUp();
	auto de{static_cast<Storage::DirectoryEntry*>(currentData)};
	if (action == Actions::LOAD) {
		auto parentDE {static_cast<Storage::DirectoryEntry*>(de->getParent())};
		items = parentDE->getPrimaryChild();
	}
	setting->fileDialog->setOwner(de->getPrimaryChild(), currentData);
	dynamic_cast<DirectoryAware*>(setting->fileDialog)->setCurrentDirectory(de);
}

void DialogDirectory::disconnectChildrenDialogs() noexcept {
	setting->fileDialog->removeOwner();
	currentData->tearDown();
}

void DialogDirectory::createSubItems(DataMap& values) noexcept {
	setting->fileDialog->load(values);
}

void DialogDirectory::addButtons(Storage::BoxButton& bb) noexcept {

	auto navBtn = Gtk::make_managed<Gtk::Button>();
	navBtn->set_relief(Gtk::RELIEF_NONE);
	navBtn->set_hexpand(true);

	// Move the existing label into the nav button.
	auto lbox = static_cast<Gtk::HBox*>(bb.getLabel()->get_parent());
	lbox->remove(*bb.getLabel());
	navBtn->add(*bb.getLabel());
	lbox->pack_start(*navBtn, Gtk::PACK_EXPAND_WIDGET);
	navBtn->signal_clicked().connect([this, &bb]() {
		setting->enterDir(static_cast<Storage::DirectoryEntry*>(bb.getData()));
	});

	// Standard edit/delete buttons.
	DialogForm::addButtons(bb);
}
