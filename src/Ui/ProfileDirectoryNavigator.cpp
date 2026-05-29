/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileDirectoryNavigator.cpp
 * @since     May 2026
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

#include "ProfileDirectoryNavigator.hpp"

using namespace LEDSpicerUI::Ui;
using namespace LEDSpicerUI::Config;

ProfileDirectoryNavigator::ProfileDirectoryNavigator(
	const Glib::RefPtr<Gtk::Builder>& builder,
	Gtk::Window* parentWindow
) noexcept :
	DirectoryNavigator(),
	dialogImportProfile(DialogImport::Types::PROFILE, parentWindow)
{
	DataDialogs::DialogProfile::buildInstance(builder, "DialogProfile");

	Gtk::Button
		* btnNewProfileFolder = nullptr,
		* btnAddProfile       = nullptr,
		* btnImportProfile    = nullptr;

	builder->get_widget_derived("BoxProfiles",  box);
	builder->get_widget("BtnProfileHome",       btnHome);
	builder->get_widget("BtnNewProfileFolder",  btnNewProfileFolder);
	builder->get_widget("BoxProfileBreadcrumb", boxBreadcrumb);
	builder->get_widget("BtnAddProfile",        btnAddProfile);
	builder->get_widget("BtnImportProfile",     btnImportProfile);

	DataDialogs::DialogProfile::getInstance()->setBox(box);

	sortDirectoriesFirst(box);

	btnNewProfileFolder->signal_clicked().connect([this]() { onNewDirClicked(); });

	// Import button.
	btnImportProfile->signal_clicked().connect([this]() {
		if (dialogImportProfile.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportProfile.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					ProfileFile datafile(selectedFile, currentDir);
					DataDialogs::DialogProfile::getInstance()->load(datafile.getDataMap());
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportProfile.hide();
	});

	btnHome->signal_clicked().connect([this]() {
		enterDirectory(&rootDir);
	});

	// Track selected tile as the default profile.
	box->signal_selected_children_changed().connect([this]() {
		const auto selected {box->get_selected_children()};
		if (selected.empty()) return;
		auto bb {dynamic_cast<Storage::BoxButton*>(selected.front())};
		if (not bb or not bb->getData()) return;
		// Skip directory tiles.
		if (dynamic_cast<Storage::DirectoryEntry*>(bb->getData())) return;
		const string& name {bb->getData()->getValue(FILENAME)};
		if (name.empty() or name == defaultProfileName) return;
		defaultProfileName = name;
		Defaults::markDirty();
	});
}

void ProfileDirectoryNavigator::clear() noexcept {
	rootDir.getPrimaryChild()->wipe();
	currentDir = &rootDir;
}

void ProfileDirectoryNavigator::extractData(
	const string& filePath,
	Storage::DirectoryEntry* parent
) {
	ProfileFile datafile(filePath, parent);
	auto dp {DataDialogs::DialogProfile::getInstance()};
	dp->setOwner(parent->getPrimaryChild(), parent);
	dp->setCurrentDirectory(parent);
	dp->load(datafile.getDataMap());
}

void ProfileDirectoryNavigator::saveItem(
	Storage::Data* item,
	const string& filePath
) const noexcept {
	ProfileFile::save(*static_cast<Storage::Profile*>(item), filePath);
}

void ProfileDirectoryNavigator::setupDialog() noexcept {
	auto dp {DataDialogs::DialogProfile::getInstance()};
	dp->setOwner(currentDir->getPrimaryChild(), currentDir);
	dp->setCurrentDirectory(currentDir);
	dp->refreshItems();
}
