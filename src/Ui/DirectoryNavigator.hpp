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

#include "Storage/BoxButtonCollection.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DirectoryNavigator
 * Manages directory-style navigation over a flat BoxButtonCollection.
 * Items with paths like "folder/subfolder/item" are grouped by directory.
 * Reusable for Inputs, Profiles, and Animations.
 */
class DirectoryNavigator {

public:

	DirectoryNavigator() = delete;

	/**
	 * Creates a navigator for a collection.
	 * @param builder Glade builder to fetch widgets.
	 * @param items The flat collection to navigate.
	 * @param homeButtonName Name of home button widget.
	 * @param breadcrumbBoxName Name of breadcrumb container widget.
	 * @param newDirButtonName Name of new folder button widget.
	 */
	DirectoryNavigator(
		Glib::RefPtr<Gtk::Builder> const& builder,
		Storage::BoxButtonCollection*     items,
		const string&                     homeButtonName,
		const string&                     breadcrumbBoxName,
		const string&                     newDirButtonName
	);

	virtual ~DirectoryNavigator() = default;

	/**
	 * Rebuilds the view cache and refreshes display.
	 */
	void refresh();

	/**
	 * Navigates into a subdirectory.
	 * @param dir Directory name to enter.
	 */
	virtual void navigateInto(const string& dir);

	/**
	 * Goes up one directory level.
	 */
	virtual void navigateUp();

	/**
	 * Returns to root.
	 */
	virtual void navigateHome();

	/**
	 * Navigates to a specific level in the breadcrumb.
	 * @param level Breadcrumb level (0 = first segment).
	 */
	virtual void navigateToLevel(size_t level);

	/**
	 * Returns current path as string (segments joined with /).
	 * @return Path string.
	 */
	string getCurrentPath() const;

	/**
	 * Returns true if currently at root level.
	 * @return True if at root.
	 */
	bool isAtRoot() const;

protected:

	/// The flat collection being navigated (not owned).
	Storage::BoxButtonCollection* items;

	/// Temporary directory BoxButtons (recreated on refresh).
	Storage::BoxButtonCollection directoryEntries;

	/// Display area for items and directories.
	OrdenableFlowBox* displayBox = nullptr;

	Gtk::Button
		* homeButton   = nullptr,  /// Button to return to root.
		* newDirButton = nullptr;  /// Button to create new directory.

	/// Container for breadcrumb trail.
	Gtk::Box* breadcrumbBox = nullptr;

	/// Current navigation path as segments.
	vector<string> pathSegments;

	std::set<string>
		/// User-created empty directories (full paths, persisted until items added).
		emptyDirs,

		/// Cached subdirectory names at current level.
		cachedDirs;

	/// Cached items directly in current level.
	vector<Storage::BoxButton*> cachedItems;

	/// Breadcrumb widgets (recreated on path change).
	vector<unique_ptr<Gtk::Widget>> breadcrumbWidgets;

	/**
	 * Scans items and populates cachedDirs and cachedItems for current path.
	 */
	void rebuildCache();

	/**
	 * Populates displayBox from cache: directories first, then items.
	 */
	void populateDisplay();

	/**
	 * Rebuilds the breadcrumb trail.
	 */
	void updateBreadcrumb();

	/**
	 * Creates a BoxButton for a directory entry.
	 * @param dirName Directory name.
	 * @return Reference to created BoxButton.
	 */
	Storage::BoxButton& createDirectoryButton(const string& dirName);

	/**
	 * Adds edit and delete buttons to a directory BoxButton.
	 * @param boxButton BoxButton to modify.
	 * @param fullPath Full path of directory.
	 */
	void addDirectoryButtons(Storage::BoxButton& boxButton, const string& fullPath);

	/**
	 * Shows dialog to create new directory.
	 */
	void showNewDirectoryDialog();

	/**
	 * Shows dialog to rename a directory.
	 * @param path Current directory path.
	 */
	void showRenameDirectoryDialog(const string& path);

	/**
	 * Renames a directory, updating all item paths within it.
	 * @param oldPath Current path.
	 * @param newPath New path.
	 */
	void renameDirectory(const string& oldPath, const string& newPath);

	/**
	 * Confirms and executes directory deletion.
	 * @param path Directory path to delete.
	 */
	void confirmDeleteDirectory(const string& path);

	/**
	 * Deletes a directory and all its contents.
	 * @param path Directory path.
	 */
	void deleteDirectory(const string& path);

	/**
	 * Gets all items under a path (for delete/rename operations).
	 * @param path Directory path.
	 * @return Vector of BoxButton pointers.
	 */
	vector<Storage::BoxButton*> getItemsUnderPath(const string& path) const;

};

} // namespace
