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

#include "Storage/DirectoryEntry.hpp"
#include "DataDialogs/DialogDirectory.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DirectoryNavigator
 * Base controller for directory-style navigation over a DirNode tree.
 * Owns the root DirectoryEntry and tracks the currently active directory.
 * Subclasses wire their specific consumer dialogs via wireDialogs().
 * Declaration order is critical — rootData must be initialized before rootDir.
 */
class DirectoryNavigator {

public:

	virtual ~DirectoryNavigator();

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

	/// @return true if currently at root.
	bool isAtRoot() const noexcept;

	/// @return Currently active directory pointer.
	Storage::DirectoryEntry* getCurrentDir() const noexcept;

	/// Called to clean up all data.
	virtual void clear() noexcept abstract;

	/**
	 * Saves all items in the tree to disk under projectDir/specialized.
	 * @param baseDir Root project directory plus the specialized sub directory.
	 */
	void save(const string& baseDir) noexcept;

protected:

	/// Backing data for rootDir — must be declared before rootDir.
	Values rootData;

	/// Owned root directory entry for this navigator type.
	Storage::DirectoryEntry rootDir;

	/// Currently active directory. Always valid; starts at rootDir.
	Storage::DirectoryEntry* currentDir;

	/// Flat scan accumulator — populated by process(), consumed by DDir::load().
	DataMap scanData;

	DirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/**
	 * Wires all consumer dialogs to the given directory's contents and refreshes the view.
	 * Each specialized navigator implements this for its own dialog set.
	 * @param dir The directory to wire. Always a valid pointer (root or child).
	 */
	virtual void wireDialogs(Storage::DirectoryEntry* dir) noexcept abstract;

	/**
	 * Recursively walks absPath, populating scanData with directory entries and
	 * file data. Directories before files within each level (sorted order).
	 * @param absPath Absolute filesystem path to scan.
	 * @param relPath Navigator-relative path prefix for this level (empty = root).
	 */
	void process(const string& absPath, const string& relPath) noexcept;

	/**
	 * Parses one .xml file and returns its extracted DataMap.
	 * @param filePath Absolute path to the .xml file.
	 * @param relPath  Navigator-relative directory path containing the file.
	 * @return DataMap with all data extracted from the file.
	 */
	virtual DataMap extractData(const string& filePath, const string& relPath) noexcept abstract;

	/**
	 * Writes one item to filePath.
	 * @param item Data pointer.
	 * @param filePath Absolute destination path including filename and extension.
	 */
	virtual void saveItem(Storage::Data* item, const string& filePath) noexcept abstract;

	/**
	 * @return The subdirectory path suffix for this navigator.
	 */
	virtual const string& getSubDir() const noexcept abstract;

};

} // namespace
