/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryAware.hpp
 * @since     Apr 17, 2026
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

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DirectoryAware
 * Pure mixin that  tracks the active directory for file-based dialogs.
 * Mix into any DialogForm subclass that manages FileNode Data objects
 * (Inputs, Animations, Profiles) stored inside project directories.
 * The navigator sets currentDirectory before opening the dialog.
 */
class DirectoryAware {

public:

	virtual ~DirectoryAware() = default;

	/**
	 * Sets the active directory for new item creation.
	 * @param directory The current DirectoryEntry. nullptr = root.
	 */
	void setCurrentDirectory(Storage::DirectoryEntry* directory) noexcept;

	/**
	 * @return The active directory, or nullptr if at root.
	 */
	Storage::DirectoryEntry* getCurrentDirectory() const noexcept;

protected:

	/// Active directory for new items. nullptr = root.
	Storage::DirectoryEntry* currentDirectory = nullptr;

};

} // namespace
