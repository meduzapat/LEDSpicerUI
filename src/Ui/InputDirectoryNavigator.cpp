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
	DirectoryNavigator(builder),
	dialogImportInput(DialogImport::Types::INPUT, parentWindow),
	dirSetting {
		boxInputs,
		COLLECTION_INPUT_DIRECTORIES,
		TYPE_INPUT_DIR,
		[this](Storage::DirectoryEntry* dir) { enterDirectory(dir); }
	}
{

	DataDialogs::DialogInput::buildInstance(builder, "DialogInput");

	builder->get_widget_derived("BoxInputs",  boxInputs);
	builder->get_widget("BtnInputHome",       btnHome);
	builder->get_widget("BtnNewInputFolder",  btnNewInputFolder);
	builder->get_widget("BoxInputBreadcrumb", boxBreadcrumb);
	builder->get_widget("BtnAddInput",        btnAddInput);
	builder->get_widget("BtnImportInput",     btnImportInput);

	// Setup DialogDirectory.
	DataDialogs::DialogForm::setSignalAddTo(btnNewInputFolder, DataDialogs::DialogDirectory::getInstance());

	// Import button.
	btnImportInput->signal_clicked().connect([this]() {
		if (dialogImportInput.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportInput.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					InputFile datafile(selectedFile, currentDir);
					DataDialogs::DialogInput::getInstance()->load(&datafile);
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportInput.hide();
	});

	btnHome->signal_clicked().connect([this]() {
		currentDir = &rootDir;
		wireDialogs(currentDir);
	});
}

InputDirectoryNavigator::~InputDirectoryNavigator() {
	delete DataDialogs::DialogInput::getInstance();
}

void InputDirectoryNavigator::clear() noexcept {
	rootDir.wipe();
	currentDir = &rootDir; // FIX: was missing — currentDir dangled into wiped tree.
}

void InputDirectoryNavigator::wireDialogs(Storage::DirectoryEntry* dir) noexcept {

	const bool sensitive(Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)->getSize());
	btnImportInput->set_sensitive(sensitive);
	btnAddInput->set_sensitive(sensitive);

	Storage::BoxButtonCollection& contents{dir->getContents()};

	// Always configure the section before wiring — fires before any button can be pressed.
	DataDialogs::DialogDirectory::getInstance()->setSettings(dirSetting);
	DataDialogs::DialogDirectory::getInstance()->setOwner(&contents, dir);

	DataDialogs::DialogInput::getInstance()->setOwner(&contents, dir);

	// Update navigation buttons.
	btnHome->set_sensitive(not isAtRoot());

	// Rebuild breadcrumb.
	for (auto child : boxBreadcrumb->get_children()) boxBreadcrumb->remove(*child);

	if (not dir->isAtRoot()) {
		// Walk the parent chain bottom-up, collecting ancestor buttons.
		auto node = static_cast<Storage::DirectoryEntry*>(dir->getParent());
		while (not node->isAtRoot()) {
			auto* btn = Gtk::make_managed<Gtk::Button>(node->getName());
			btn->get_style_context()->add_class("BreadcrumbButton");
			btn->signal_clicked().connect([this, node]() {
				enterDirectory(node);
			});
			auto* sep = Gtk::make_managed<Gtk::Label>("/");
			sep->get_style_context()->add_class("BreadcrumbSeparator");

			// Reorder so each ancestor goes to the front, keeping correct left-to-right order.
			boxBreadcrumb->pack_start(*btn, Gtk::PACK_SHRINK);
			boxBreadcrumb->pack_start(*sep, Gtk::PACK_SHRINK);
			boxBreadcrumb->reorder_child(*btn, 0);
			boxBreadcrumb->reorder_child(*sep, 1);

			node = static_cast<Storage::DirectoryEntry*>(node->getParent());
		}

		// Separator before the current (non-clickable) label.
		auto* sep = Gtk::make_managed<Gtk::Label>("/"); // FIX: was missing.
		sep->get_style_context()->add_class("BreadcrumbSeparator");
		boxBreadcrumb->pack_end(*sep, Gtk::PACK_SHRINK);

		// Current directory label at the end. // FIX: was pack_start — placed it at position 0.
		auto* cur = Gtk::make_managed<Gtk::Label>(dir->getName());
		cur->get_style_context()->add_class("BreadcrumbCurrent");
		boxBreadcrumb->pack_end(*cur, Gtk::PACK_SHRINK);
	}

	boxBreadcrumb->show_all();
}
