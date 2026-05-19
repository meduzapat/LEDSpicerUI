/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDirectoryNavigator.cpp
 * @since     Feb 16, 2026
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

#include "InputDirectoryNavigator.hpp"

using namespace LEDSpicerUI::Ui;
using namespace LEDSpicerUI::Config;

InputDirectoryNavigator::InputDirectoryNavigator(
	const Glib::RefPtr<Gtk::Builder>& builder,
	Gtk::Window* parentWindow
) noexcept :
	DirectoryNavigator(),
	dialogImportInput(DialogImport::Types::INPUT, parentWindow)
{
	DataDialogs::DialogInput::buildInstance(builder, "DialogInput");

	Gtk::Button
		* btnNewInputFolder = nullptr,
		* btnAddInput       = nullptr,
		* btnImportInput    = nullptr;

	builder->get_widget_derived("BoxInputs",  boxInputs);
	builder->get_widget("BtnInputHome",       btnHome);
	builder->get_widget("BtnNewInputFolder",  btnNewInputFolder);
	builder->get_widget("BoxInputBreadcrumb", boxBreadcrumb);
	builder->get_widget("BtnAddInput",        btnAddInput);
	builder->get_widget("BtnImportInput",     btnImportInput);

	DataDialogs::DialogInput::getInstance()->setBox(boxInputs);

	// Sort directories first, then files.
	boxInputs->set_sort_func([](Gtk::FlowBoxChild* a, Gtk::FlowBoxChild* b) -> int {
		auto
			dA {static_cast<Storage::BoxButton*>(a->get_child())->getData()},
			dB {static_cast<Storage::BoxButton*>(b->get_child())->getData()};
		const bool
			aIsDir {dynamic_cast<Storage::DirectoryEntry*>(dA) != nullptr},
			bIsDir {dynamic_cast<Storage::DirectoryEntry*>(dB) != nullptr};
		if (aIsDir != bIsDir)
			return aIsDir ? -1 : 1;
		return dynamic_cast<Storage::DirNode*>(dA)->getName().compare(dynamic_cast<Storage::DirNode*>(dB)->getName());
	});

	// Register automatic buttons.
	auto chEl {Storage::CollectionHandler::getInstance(COLLECTION_ELEMENTS)};
	chEl->registerSensitivity(btnAddInput);
	chEl->registerSensitivity(btnImportInput);

	btnNewInputFolder->signal_clicked().connect([this]() { onNewDirClicked(); });

	// Import button.
	btnImportInput->signal_clicked().connect([this]() {
		if (dialogImportInput.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportInput.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					InputFile datafile(selectedFile, currentDir);
					DataDialogs::DialogInput::getInstance()->load(datafile.getDataMap());
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportInput.hide();
	});

	btnHome->signal_clicked().connect([this]() {
		enterDirectory(&rootDir);
	});
}

void InputDirectoryNavigator::clear() noexcept {
	rootDir.getPrimaryChild()->wipe();
	currentDir = &rootDir;
}

void InputDirectoryNavigator::extractData(const string& filePath, Storage::DirectoryEntry* parent) noexcept {

	InputFile datafile(filePath, parent);
	auto di {DataDialogs::DialogInput::getInstance()};
	di->setOwner(parent->getPrimaryChild(), parent);
	di->setCurrentDirectory(parent);
	di->load(datafile.getDataMap());
}

void InputDirectoryNavigator::saveItem(Storage::Data* item, const string& filePath) const noexcept {
	InputFile::save(*static_cast<Storage::Input*>(item), filePath);
}

void InputDirectoryNavigator::wireDialogs() noexcept {

	auto di {DataDialogs::DialogInput::getInstance()};
	di->setOwner(currentDir->getPrimaryChild(), currentDir);
	di->setCurrentDirectory(currentDir);
	di->refreshItems();

	// Update navigation buttons.
	btnHome->set_sensitive(not isAtRoot());

	// Rebuild breadcrumb.
	for (auto child : boxBreadcrumb->get_children()) boxBreadcrumb->remove(*child);

	if (not currentDir->isAtRoot()) {
		// Walk the parent chain bottom → up, collecting ancestor buttons.
		auto node {static_cast<Storage::DirectoryEntry*>(currentDir->getParent())};
		while (not node->isAtRoot()) {
			auto btn {Gtk::make_managed<Gtk::Button>(node->getName())};
			btn->get_style_context()->add_class(CSS_BREADCRUMB_BUTTON);
			btn->signal_clicked().connect([this, node]() {
				enterDirectory(node);
			});
			auto sep {Gtk::make_managed<Gtk::Label>("/")};
			sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);

			// Reorder so each ancestor goes to the front, keeping correct left-to-right order.
			boxBreadcrumb->pack_start(*btn, Gtk::PACK_SHRINK);
			boxBreadcrumb->pack_start(*sep, Gtk::PACK_SHRINK);
			boxBreadcrumb->reorder_child(*btn, 0);
			boxBreadcrumb->reorder_child(*sep, 1);

			node = static_cast<Storage::DirectoryEntry*>(node->getParent());
		}

		// Separator before the current (non-clickable) label.
		auto sep {Gtk::make_managed<Gtk::Label>("/")};
		sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);
		boxBreadcrumb->pack_end(*sep, Gtk::PACK_SHRINK);

		// Current directory label at the end.
		auto cur {Gtk::make_managed<Gtk::Label>(currentDir->getName())};
		cur->get_style_context()->add_class(CSS_BREADCRUMB_CURRENT);
		boxBreadcrumb->pack_end(*cur, Gtk::PACK_SHRINK);
	}

	boxBreadcrumb->show_all();
}
