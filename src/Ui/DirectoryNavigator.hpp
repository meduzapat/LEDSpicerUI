/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryNavigator.hpp
 * @since     Feb 15, 2026
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

#include "XMLHelper.hpp"
#include "config/Settings.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DirectoryNavigator
 * Base controller for directory-style navigation over a DirNode tree.
 * Owns the root DirectoryEntry and tracks the currently active directory.
 * Manages directory creation, renaming, and deletion directly.
 * Subclasses wire their specific file dialogs via wireDialogs() and
 * load file data via extractData().
 */
class DirectoryNavigator {

public:

	virtual ~DirectoryNavigator() = default;

	/**
	 * Called when this navigator's panel becomes active.
	 * Re-wires all consumer dialogs to the remembered current directory.
	 */
	void onActivate() noexcept;

	/**
	 * Enters a directory, making it current and wiring dialogs to its contents.
	 * @param dir Directory to enter.
	 */
	void enterDirectory(Storage::DirectoryEntry* dir) noexcept;

	/**
	 * Navigates up to the parent directory. Stops silently at root.
	 */
	void navigateUp() noexcept;

	/**
	 * @return true if currently at root.
	 */
	bool isAtRoot() const noexcept;

	/**
	 * @return Currently active directory pointer.
	 */
	Storage::DirectoryEntry* getCurrentDir() const noexcept;

	/**
	 * Called to clean up all data.
	 */
	virtual void clear() noexcept abstract;

	/**
	 * Loads all items from disk under Settings::getProjectDir()/getSubDir() into the tree.
	 * Clears existing data first, then calls wireDialogs() at root on completion.
	 */
	void load() noexcept;

	/**
	 * Saves all items in the tree to disk under Settings::getProjectDir()/getSubDir().
	 */
	void save() const noexcept;

protected:

	/// Owned root directory entry for this navigator type.
	Storage::DirectoryEntry rootDir;

	/// Currently active directory. Always valid; starts at rootDir.
	Storage::DirectoryEntry* currentDir;

	/// Breadcrumb container. Subclasses should populate and manage this directly.
	Gtk::Box* boxBreadcrumb = nullptr;

	/// Home button. Subclasses should connect this to enterDirectory(&rootDir) and manage sensitivity.
	Gtk::Button* btnHome = nullptr;

	/// Display box for the contents at the active directory.
	OrdenableFlowBox* box = nullptr;

	DirectoryNavigator() noexcept;

	/**
	 * Wires all consumer dialogs to current directory contents and refreshes the view.
	 */
	void wireDialogs() noexcept;

	/**
	 * Setups the consume dialog while wiring.
	 */
	virtual void setupDialog() noexcept abstract;

	/**
	 * Recursively walks absPath, building the DirectoryEntry tree and loading
	 * file data into each directory's collection.
	 * @param parent  DirectoryEntry that owns this level of the tree.
	 * @param absPath Absolute filesystem path to scan.
	 */
	void process(Storage::DirectoryEntry* parent, const string& absPath) noexcept;

	/**
	 * Parses one .xml file and loads its data directly into parent's collection.
	 * @param filePath Absolute path to the .xml file.
	 * @param parent   DirectoryEntry that owns this file.
	 * @throws Message if the file is invalid or cannot be loaded.
	 */
	virtual void extractData(const string& filePath, Storage::DirectoryEntry* parent) abstract;

	/**
	 * Writes one item to filePath.
	 * @param item     Data pointer.
	 * @param filePath Absolute destination path including filename and extension.
	 */
	virtual void saveItem(Storage::Data* item, const string& filePath) const noexcept abstract;

	/**
	 * @return The subdirectory path suffix for this navigator (e.g. PATH_INPUT).
	 */
	virtual const string& getSubDir() const noexcept abstract;

	/**
	 * Prompts the user for a directory name. Pre-fills with current when renaming.
	 * @param current Existing name to pre-fill, or empty for a new directory.
	 * @return Sanitized name, or empty string if the user cancelled.
	 */
	string promptDirName(const string& current) noexcept;

	/**
	 * Shows the name prompt, validates uniqueness in currentDir, creates the
	 * child directory, and refreshes the display.
	 */
	void onNewDirClicked() noexcept;

	/**
	 * Attaches navigation, edit, and delete buttons to a DirectoryEntry BoxButton.
	 * @param bb BoxButton wrapping the DirectoryEntry.
	 */
	void wireDirButtons(Storage::BoxButton& bb) noexcept;

	/**
	 * Installs a sort function on the given box that places directories before
	 * files; entries within each group are sorted alphabetically by name.
	 * @param box The flow box to sort.
	 */
	static void sortDirectoriesFirst(OrdenableFlowBox* box) noexcept;

};

} // namespace
