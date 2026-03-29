/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryEntry.hpp
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

#include "BoxButtonCollection.hpp"
#include "DirNode.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirectoryEntry
 * Represents a single directory node in the navigator tree.
 * Never serialized — exists only at runtime for navigation.
 * Inherits Data for field storage (NAME) and BoxButton compatibility.
 * Inherits DirNode for parent pointer and path resolution.
 * Does not inherit Revertible — directories are never snap/restored.
 */
class DirectoryEntry : public Data, public DirNode {

public:

	/**
	 * @param data   Must contain NAME (directory segment name).
	 * @param parent Parent directory node, or nullptr for root-level.
	 */
	DirectoryEntry(StringUMap& data, const DirectoryEntry* parent);

	virtual ~DirectoryEntry() = default;

	const string createUniqueId() const noexcept override;
	const string createPrettyName() const noexcept override;
	const string createTooltip() const noexcept override;
	string_view getCssClass() const noexcept override { return "DirectoryBoxButton"; }

	string getName() const noexcept override;
	string getFsId() const noexcept override;

	/**
	 * @return true if this directory contains no items.
	 */
	bool isEmpty() const noexcept;

	/**
	 * @return The mutable contents collection for this directory.
	 */
	BoxButtonCollection& getContents() noexcept { return contents; }

	/**
	 * @return Non-const parent pointer for tree navigation.
	 */
	DirNode* getParent();

protected:

	/// Counter for stable id generation.
	inline static size_t dirCounter = 0;

	/// Items owned by this directory.
	BoxButtonCollection contents;

};

} // namespace
