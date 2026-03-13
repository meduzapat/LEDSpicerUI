/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogFileForm.hpp
 * @since     Feb 25, 2026
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

#include "DialogFormHost.hpp"
#include "Storage/DirectoryEntry.hpp"
#include "Storage/FileData.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogFileForm
 * Intermediate base for dialogs that manage file-based Data objects
 * (Inputs, Animations, Profiles) stored inside project directories.
 * Provides directory tracking and per-directory filename uniqueness checks,
 */
class DialogFileForm : public DialogFormHost {

public:

	virtual ~DialogFileForm() = default;

	/**
	 * Sets the active directory for new file creation.
	 * Pass nullptr to indicate the root of the type's directory tree.
	 * @param directory Pointer to the current DirectoryEntry, or nullptr for root.
	 */
	void setCurrentDirectory(Storage::DirectoryEntry* directory);

	/**
	 * Returns the current directory, or nullptr if at root.
	 * @return Current DirectoryEntry pointer.
	 */
	Storage::DirectoryEntry* getCurrentDirectory() const;

	/**
	 * Resolves the full relative path for a given filename inside the current directory.
	 * @param filename Bare filename without extension.
	 * @return Relative path string, e.g. "subdir/name" or just "name" at root.
	 */
	string getFullPath(const string& filename) const;

	/**
	 * Checks whether a filename is already used by another FileData item
	 * residing in the same directory as currentDirectory.
	 * Ignores the item currently being edited (currentData).
	 * @param filename Bare filename to check.
	 * @return true if the filename is already taken in this directory.
	 */
	bool isUniqueFilename(const string& filename) const;

protected:

	using DialogFormHost::DialogFormHost;

	/// Active directory for new items. nullptr = root.
	Storage::DirectoryEntry* currentDirectory = nullptr;

};

} // namespace
