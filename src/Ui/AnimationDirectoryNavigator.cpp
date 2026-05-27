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

	builder->get_widget_derived("BoxAnimations",  boxAnimations);
	builder->get_widget("BtnAnimationHome",       btnHome);
	builder->get_widget("BtnNewAnimationFolder",  btnNewAnimationFolder);
	builder->get_widget("BoxAnimationBreadcrumb", boxBreadcrumb);
	builder->get_widget("BtnAddAnimation",        btnAddAnimation);
	builder->get_widget("BtnImportAnimation",     btnImportAnimation);

	DataDialogs::DialogAnimation::getInstance()->setBox(boxAnimations);

	sortDirectoriesFirst(boxAnimations);

	// An animation needs at least one group to play on.
	auto chGroups {Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)};
	chGroups->registerSensitivity(btnAddAnimation);
	chGroups->registerSensitivity(btnImportAnimation);

	btnNewAnimationFolder->signal_clicked().connect([this]() { onNewDirClicked(); });

	// Import button.
	btnImportAnimation->signal_clicked().connect([this]() {
		if (dialogImportAnimation.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportAnimation.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					AnimationFile datafile(selectedFile, currentDir);
					DataDialogs::DialogAnimation::getInstance()->load(datafile.getDataMap());
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportAnimation.hide();
	});

	btnHome->signal_clicked().connect([this]() {
		enterDirectory(&rootDir);
	});
}

void AnimationDirectoryNavigator::clear() noexcept {
	rootDir.getPrimaryChild()->wipe();
	currentDir = &rootDir;
}

void AnimationDirectoryNavigator::extractData(
	const string& filePath,
	Storage::DirectoryEntry* parent
) {
	AnimationFile datafile(filePath, parent);
	auto da {DataDialogs::DialogAnimation::getInstance()};
	da->setOwner(parent->getPrimaryChild(), parent);
	da->setCurrentDirectory(parent);
	da->load(datafile.getDataMap());
}

void AnimationDirectoryNavigator::saveItem(
	Storage::Data* item,
	const string& filePath
) const noexcept {
	AnimationFile::save(*static_cast<Storage::Animation*>(item), filePath);
}

void AnimationDirectoryNavigator::wireDialogs() noexcept {
	auto da {DataDialogs::DialogAnimation::getInstance()};
	da->setOwner(currentDir->getPrimaryChild(), currentDir);
	da->setCurrentDirectory(currentDir);
	da->refreshItems();

	// Update navigation buttons.
	btnHome->set_sensitive(not isAtRoot());

	// Rebuild breadcrumb.
	for (auto child : boxBreadcrumb->get_children()) boxBreadcrumb->remove(*child);

	if (not currentDir->isAtRoot()) {
		auto node {static_cast<Storage::DirectoryEntry*>(currentDir->getParent())};
		while (not node->isAtRoot()) {
			auto btn {Gtk::make_managed<Gtk::Button>(node->getName())};
			btn->get_style_context()->add_class(CSS_BREADCRUMB_BUTTON);
			btn->signal_clicked().connect([this, node]() {
				enterDirectory(node);
			});
			auto sep {Gtk::make_managed<Gtk::Label>("/")};
			sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);

			boxBreadcrumb->pack_start(*btn, Gtk::PACK_SHRINK);
			boxBreadcrumb->pack_start(*sep, Gtk::PACK_SHRINK);
			boxBreadcrumb->reorder_child(*btn, 0);
			boxBreadcrumb->reorder_child(*sep, 1);

			node = static_cast<Storage::DirectoryEntry*>(node->getParent());
		}

		auto sep {Gtk::make_managed<Gtk::Label>("/")};
		sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);
		boxBreadcrumb->pack_end(*sep, Gtk::PACK_SHRINK);

		auto cur {Gtk::make_managed<Gtk::Label>(currentDir->getName())};
		cur->get_style_context()->add_class(CSS_BREADCRUMB_CURRENT);
		boxBreadcrumb->pack_end(*cur, Gtk::PACK_SHRINK);
	}

	boxBreadcrumb->show_all();
}
