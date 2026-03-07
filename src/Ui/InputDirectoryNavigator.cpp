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

InputDirectoryNavigator::InputDirectoryNavigator(
	const Glib::RefPtr<Gtk::Builder>& builder,
	Gtk::Window* parentWindow
) :
	dialogImportInput(DialogImport::Types::INPUT, parentWindow)
{

	DataDialogs::DialogInput::buildInstance(builder, "DialogInput");
	DataDialogs::DialogDirectory::buildInstance(builder, "DialogDirectory");

	OrdenableFlowBox* box = nullptr;
	builder->get_widget_derived("BoxInputs",  box);
	builder->get_widget("BtnInputHome",       btnHome);
	builder->get_widget("BtnNewInputFolder",  btnNewInputFolder);
	builder->get_widget("BoxInputBreadcrumb", boxBreadcrumb);

	// Setup DialogDirectory with the collection
	DataDialogs::DialogForm::setSignalAddTo(btnNewInputFolder, DataDialogs::DialogDirectory::getInstance());

//	DataDialogs::DialogInput::getInstance()->setOwner(&currentDir->getContents());

	onActivate();

	// Import button.
	Gtk::Button* btnImportInput = nullptr;
	builder->get_widget("BtnImportInput", btnImportInput);
	btnImportInput->signal_clicked().connect([this]() {
		if (dialogImportInput.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportInput.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					InputFile datafile(selectedFile);
					DataDialogs::DialogInput::getInstance()->load(&datafile);
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportInput.hide();
	});
}

InputDirectoryNavigator::~InputDirectoryNavigator() {

	delete DataDialogs::DialogInput::getInstance();
}

void InputDirectoryNavigator::clear() {
	rootDir.reset();
	DataDialogs::DialogInput::getInstance();
}

//void InputDirectoryNavigator::saveToDisk() {
//	string inputsPath = getInputsPath();
//	string backupPath = inputsPath.substr(0, inputsPath.length() - 1) + "_bk/";
//
//	// Remove old backup if exists
//	if (Glib::file_test(backupPath, Glib::FILE_TEST_IS_DIR)) {
//		removeDirectoryRecursive(backupPath);
//	}
//
//	// Move current → backup
//	if (Glib::file_test(inputsPath, Glib::FILE_TEST_IS_DIR)) {
//		if (std::rename(inputsPath.c_str(), backupPath.c_str()) != 0) {
//			Message::displayError("Failed to create backup of inputs directory");
//			return;
//		}
//	}
//
//	// Create fresh directory
//	g_mkdir_with_parents(inputsPath.c_str(), 0755);
//
//	// Create empty directories first
//	for (const auto& emptyDir : emptyDirs) {
//		string fullPath = inputsPath + emptyDir;
//		g_mkdir_with_parents(fullPath.c_str(), 0755);
//	}
//
//	// Save all items
//	bool success = true;
//	for (auto* bb : inputs) {
//		auto* data = bb->getData();
//		string filename = data->createUniqueId();
//		string fullPath = inputsPath + filename + ".xml";
//
//		ensureDirectoryExists(fullPath);
//
//		try {
//			string xml = data->toXML();
//			Glib::file_set_contents(fullPath, xml);
//		}
//		catch (const Glib::Error& e) {
//			Message::displayError("Failed to save " + filename + ": " + e.what());
//			success = false;
//		}
//	}
//
//	// Cleanup backup on success, restore on failure
//	if (success) {
//		removeDirectoryRecursive(backupPath);
//	}
//	else {
//		removeDirectoryRecursive(inputsPath);
//		std::rename(backupPath.c_str(), inputsPath.c_str());
//		Message::displayError("Save failed, restored from backup");
//	}
//}

void InputDirectoryNavigator::wireDialogs(Storage::DirectoryEntry* dir) {
	Storage::BoxButtonCollection& contents{dir->getContents()};

	// Wire DialogDirectory and DialogInput to the current dir's contents.
	auto dirDialog(DataDialogs::DialogDirectory::getInstance());
	dirDialog->setCollectionName(string(COLLECTION_DIRECTORIES) + "_" + COLLECTION_INPUT);
	dirDialog->setOwner(&contents, dir);

	DataDialogs::DialogInput::getInstance()->setOwner(&contents, dir);
	DataDialogs::DialogInput::getInstance()->refreshBox();

	// Update navigation buttons.
	btnHome->set_sensitive(not isAtRoot());

	// Rebuild breadcrumb.
	for (auto* child : boxBreadcrumb->get_children())
		boxBreadcrumb->remove(*child);

	// Walk the parent chain to build segments bottom-up, then reverse.
	vector<Storage::DirectoryEntry*> crumbs;
	auto* node = dir;
	while (node and not node->isRoot()) {
		crumbs.push_back(node);
		node = static_cast<Storage::DirectoryEntry*>(node->getParent());
	}

	for (auto it = crumbs.rbegin(); it != crumbs.rend(); ++it) {
		auto* btn = Gtk::make_managed<Gtk::Button>((*it)->getName());
		btn->get_style_context()->add_class("BreadcrumbButton");
		btn->signal_clicked().connect([this, entry = *it]() {
			enterDirectory(entry);
		});
		boxBreadcrumb->pack_start(*btn, Gtk::PACK_SHRINK);

		auto* sep = Gtk::make_managed<Gtk::Label>("/");
		sep->get_style_context()->add_class("BreadcrumbSeparator");
		boxBreadcrumb->pack_start(*sep, Gtk::PACK_SHRINK);
	}

	boxBreadcrumb->show_all();
}

