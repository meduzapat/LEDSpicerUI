/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDirectoryNavigator.hpp
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

#include "DirectoryNavigator.hpp"
#include "DataDialogs/DialogInput.hpp"
#include "DataDialogs/DialogInputMap.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::InputDirectoryNavigator
 * Specialized navigator for Input files.
 * Owns the inputs collection, handles loading/saving, manages DialogInput.
 */
class InputDirectoryNavigator : public DirectoryNavigator {

public:

	/// Widget names for navigation controls.
	static constexpr const char
		* NAV_HOME_BUTTON    = "BtnInputHome",
		* NAV_BREADCRUMB_BOX = "BoxInputBreadcrumb",
		* NAV_NEW_DIR_BUTTON = "BtnNewInputFolder";

	InputDirectoryNavigator() = delete;

	/**
	 * Creates navigator and sets up DialogInput.
	 * @param builder Glade builder to fetch widgets.
	 */
	InputDirectoryNavigator(Glib::RefPtr<Gtk::Builder> const& builder);

	virtual ~InputDirectoryNavigator();

	/**
	 * Scans inputs/ folder and loads all .xml files into memory.
	 * Files are validated through DialogInput before storage.
	 */
	void loadFromDisk();

	/**
	 * Saves all inputs to disk using backup strategy.
	 * 1. Moves inputs/ → inputs_bk/
	 * 2. Creates fresh inputs/ and saves all items
	 * 3. Removes backup on success
	 */
	void saveToDisk();

	/**
	 * Clears all items from memory and display.
	 */
	void clear();

	void navigateInto(const string& dir) override;
	void navigateUp() override;
	void navigateHome() override;
	void navigateToLevel(size_t level) override;

protected:

	/// Owns the inputs collection.
	Storage::BoxButtonCollection inputs;

	/**
	 * Updates DialogInput with current path.
	 */
	void syncDialogPath();

	/**
	 * Returns the full inputs directory path.
	 * @return Full path to inputs directory.
	 */
	string getInputsPath() const;

	/**
	 * Recursively scans a directory for .xml files.
	 * @param dirPath Directory to scan.
	 * @param relativePath Path relative to inputs root.
	 * @param allDirs Set to collect all directories found.
	 */
	void scanDirectory(const string& dirPath, const string& relativePath, std::set<string>& allDirs);

	/**
	 * Creates directory structure for a file path.
	 * @param filePath Full file path.
	 */
	void ensureDirectoryExists(const string& filePath);

	/**
	 * Recursively removes a directory and its contents.
	 * @param dirPath Directory to remove.
	 */
	void removeDirectoryRecursive(const string& dirPath);

};

} // namespace
