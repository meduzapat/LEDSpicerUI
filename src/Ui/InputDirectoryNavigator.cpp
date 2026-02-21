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
#include "config/InputFile.hpp"

using namespace LEDSpicerUI::Ui;

InputDirectoryNavigator::InputDirectoryNavigator(
	Glib::RefPtr<Gtk::Builder> const& builder
) :
	DirectoryNavigator(
		builder,
		&inputs,
		NAV_HOME_BUTTON,
		NAV_BREADCRUMB_BOX,
		NAV_NEW_DIR_BUTTON
	)
{
	DataDialogs::DialogInputLinkMaps::initialize(builder);
	DataDialogs::DialogInputMap::initialize(builder);
	DataDialogs::DialogInputSource::initialize(builder);
	DataDialogs::DialogInput::initialize(builder);

	// Setup DialogInput with our collection
	displayBox = DataDialogs::DialogInput::getInstance()->getBox();
	DataDialogs::DialogInput::getInstance()->setOwner(&inputs);

	syncDialogPath();
	refresh();
}

InputDirectoryNavigator::~InputDirectoryNavigator() {
	delete DataDialogs::DialogInputLinkMaps::getInstance();
	delete DataDialogs::DialogInputMap::getInstance();
	delete DataDialogs::DialogInput::getInstance();
}

void InputDirectoryNavigator::loadFromDisk() {
	clear();

	string inputsPath = getInputsPath();

	if (not Glib::file_test(inputsPath, Glib::FILE_TEST_IS_DIR)) {
		return;
	}

	// Track all directories found during scan
	std::set<string> allDirs;
	scanDirectory(inputsPath, "", allDirs);

	// Determine which directories are empty (no items under them)
	for (const auto& dir : allDirs) {
		bool hasItems = false;
		string dirPrefix = dir + "/";
		for (auto* bb : inputs) {
			string itemPath = bb->getData()->createUniqueId();
			if (itemPath.find(dirPrefix) == 0 or itemPath == dir) {
				hasItems = true;
				break;
			}
		}
		// Also check if any item is directly in this directory
		if (not hasItems) {
			for (auto* bb : inputs) {
				string itemPath = bb->getData()->createUniqueId();
				auto lastSlash = itemPath.rfind('/');
				string itemDir = (lastSlash == string::npos) ? "" : itemPath.substr(0, lastSlash);
				if (itemDir == dir) {
					hasItems = true;
					break;
				}
			}
		}
		if (not hasItems) {
			emptyDirs.insert(dir);
		}
	}

	refresh();
}

void InputDirectoryNavigator::saveToDisk() {
	string inputsPath = getInputsPath();
	string backupPath = inputsPath.substr(0, inputsPath.length() - 1) + "_bk/";

	// Remove old backup if exists
	if (Glib::file_test(backupPath, Glib::FILE_TEST_IS_DIR)) {
		removeDirectoryRecursive(backupPath);
	}

	// Move current → backup
	if (Glib::file_test(inputsPath, Glib::FILE_TEST_IS_DIR)) {
		if (std::rename(inputsPath.c_str(), backupPath.c_str()) != 0) {
			Message::displayError("Failed to create backup of inputs directory");
			return;
		}
	}

	// Create fresh directory
	g_mkdir_with_parents(inputsPath.c_str(), 0755);

	// Create empty directories first
	for (const auto& emptyDir : emptyDirs) {
		string fullPath = inputsPath + emptyDir;
		g_mkdir_with_parents(fullPath.c_str(), 0755);
	}

	// Save all items
	bool success = true;
	for (auto* bb : inputs) {
		auto* data = bb->getData();
		string filename = data->createUniqueId();
		string fullPath = inputsPath + filename + ".xml";

		ensureDirectoryExists(fullPath);

		try {
			string xml = data->toXML();
			Glib::file_set_contents(fullPath, xml);
		}
		catch (const Glib::Error& e) {
			Message::displayError("Failed to save " + filename + ": " + e.what());
			success = false;
		}
	}

	// Cleanup backup on success, restore on failure
	if (success) {
		removeDirectoryRecursive(backupPath);
	}
	else {
		removeDirectoryRecursive(inputsPath);
		std::rename(backupPath.c_str(), inputsPath.c_str());
		Message::displayError("Save failed, restored from backup");
	}
}

void InputDirectoryNavigator::clear() {
	inputs.wipe();
	displayBox->wipe();
	directoryEntries.wipe();
	emptyDirs.clear();
}

void InputDirectoryNavigator::navigateInto(const string& dir) {
	DirectoryNavigator::navigateInto(dir);
	syncDialogPath();
}

void InputDirectoryNavigator::navigateUp() {
	DirectoryNavigator::navigateUp();
	syncDialogPath();
}

void InputDirectoryNavigator::navigateHome() {
	DirectoryNavigator::navigateHome();
	syncDialogPath();
}

void InputDirectoryNavigator::navigateToLevel(size_t level) {
	DirectoryNavigator::navigateToLevel(level);
	syncDialogPath();
}

void InputDirectoryNavigator::syncDialogPath() {
	DataDialogs::DialogInput::getInstance()->setPath(getCurrentPath());
}

string InputDirectoryNavigator::getInputsPath() const {
	return Defaults::getProjectsDir() + INPUT_PATH;
}

void InputDirectoryNavigator::scanDirectory(const string& dirPath, const string& relativePath, std::set<string>& allDirs) {
	auto directory = Gio::File::create_for_path(dirPath);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;

	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error& e) {
		Message::displayError("Failed to scan directory: " + e.what());
		return;
	}

	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		string name = fileInfo->get_name();

		// Skip hidden files
		if (name.empty() or name[0] == '.') {
			continue;
		}

		string fullPath = dirPath + "/" + name;

		if (fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY) {
			string newRelativePath = relativePath.empty() ? name : relativePath + "/" + name;
			allDirs.insert(newRelativePath);
			scanDirectory(fullPath, newRelativePath, allDirs);
		}
		else if (fileInfo->get_file_type() == Gio::FILE_TYPE_REGULAR) {
			// Check for .xml extension
			if (name.length() > 4 and name.substr(name.length() - 4) == ".xml") {
				try {
					/*
					 * Load through DialogInput for validation.
					 * DialogInput::load() validates, creates Data, adds to collection and box.
					 */
					InputFile inputFile(fullPath);
					DataDialogs::DialogInput::getInstance()->load(&inputFile);
				}
				catch (const std::exception& e) {
					Message::displayError("Failed to load " + fullPath + ": " + e.what());
				}
			}
		}
	}
}

void InputDirectoryNavigator::ensureDirectoryExists(const string& filePath) {
	string dirPath = Glib::path_get_dirname(filePath);
	if (not Glib::file_test(dirPath, Glib::FILE_TEST_IS_DIR)) {
		g_mkdir_with_parents(dirPath.c_str(), 0755);
	}
}

void InputDirectoryNavigator::removeDirectoryRecursive(const string& dirPath) {
	if (not Glib::file_test(dirPath, Glib::FILE_TEST_IS_DIR)) {
		return;
	}

	auto directory = Gio::File::create_for_path(dirPath);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;

	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error&) {
		return;
	}

	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		string name = fileInfo->get_name();
		string fullPath = dirPath + "/" + name;

		if (fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY) {
			removeDirectoryRecursive(fullPath);
		}
		else {
			std::remove(fullPath.c_str());
		}
	}

	std::remove(dirPath.c_str());
}
