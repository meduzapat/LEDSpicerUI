/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationDirectoryNavigator.cpp
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

#include "AnimationDirectoryNavigator.hpp"

using namespace LEDSpicerUI::Ui;
using namespace LEDSpicerUI::Config;

AnimationDirectoryNavigator::AnimationDirectoryNavigator(
	const Glib::RefPtr<Gtk::Builder>& builder,
	Gtk::Window* parentWindow
) noexcept :
	DirectoryNavigator(),
	dialogImportAnimation(DialogImport::Types::ANIMATION, parentWindow)
{
	DataDialogs::DialogAnimation::buildInstance(builder, "DialogAnimation");

	Gtk::Button
		* btnNewAnimationFolder = nullptr,
		* btnAddAnimation       = nullptr,
		* btnImportAnimation    = nullptr;

	builder->get_widget_derived("BoxAnimations",  box);
	builder->get_widget("BtnAnimationHome",       btnHome);
	builder->get_widget("BtnNewAnimationFolder",  btnNewAnimationFolder);
	builder->get_widget("BoxAnimationBreadcrumb", boxBreadcrumb);
	builder->get_widget("BtnAddAnimation",        btnAddAnimation);
	builder->get_widget("BtnImportAnimation",     btnImportAnimation);

	DataDialogs::DialogAnimation::getInstance()->setBox(box);

	sortDirectoriesFirst(box);

	// An animation needs at least one group to play on.
	auto chGroups {Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)};
	chGroups->registerSensitivity(btnAddAnimation);
	chGroups->registerSensitivity(btnImportAnimation);

	btnNewAnimationFolder->signal_clicked().connect([this]() { onNewDirClicked(); });

	// Import button.
	btnImportAnimation->signal_clicked().connect([this]() {
		const auto resp {dialogImportAnimation.run()};
		dialogImportAnimation.hide();
		if (resp == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportAnimation.get_filenames());
			Message::beginBatch();
			for (const auto& selectedFile : selectedFiles) {
				try {
					AnimationFile datafile(selectedFile, currentDir);
					DataDialogs::DialogAnimation::getInstance()->load(datafile.getDataMap());
				}
				catch (Message& e) {
					Message::collect(XMLHelper::cleanError(e.takeMessage()));
				}
			}
			Defaults::markDirty();
			Message::finishBatch("Animations imported");
		}
	});

	btnHome->signal_clicked().connect([this]() {
		enterDirectory(&rootDir);
	});
}

void AnimationDirectoryNavigator::clear() noexcept {
	rootDir.getPrimaryChild()->wipe();
	currentDir = &rootDir;
}

void AnimationDirectoryNavigator::extractData(const string& filePath, Storage::DirectoryEntry* parent) {
	AnimationFile datafile(filePath, parent);
	auto da {DataDialogs::DialogAnimation::getInstance()};
	da->setOwner(parent->getPrimaryChild(), parent);
	da->setCurrentDirectory(parent);
	da->load(datafile.getDataMap());
}

void AnimationDirectoryNavigator::saveItem(Storage::Data* item, const string& filePath) const noexcept {
	AnimationFile::save(*static_cast<Storage::Animation*>(item), filePath);
}

void AnimationDirectoryNavigator::setupDialog() noexcept {

	auto da {DataDialogs::DialogAnimation::getInstance()};
	da->setOwner(currentDir->getPrimaryChild(), currentDir);
	da->setCurrentDirectory(currentDir);
	da->refreshItems();
}
