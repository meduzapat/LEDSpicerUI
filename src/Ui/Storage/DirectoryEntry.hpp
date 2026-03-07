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
 * Holds a pointer to its parent so full paths are resolved recursively
 * without storing or copying path strings.
 * NAME stores the directory's own segment name only.
 */
class DirectoryEntry : public DirNode {

public:

	/**
	 * @param data  Must contain NAME (directory segment name).
	 * @param parent Parent directory node, or nullptr for root-level.
	 */
	DirectoryEntry(StringUMap& data, const DirectoryEntry* parent) :
		DirNode(data, parent),
		fsId("dir_" + std::to_string(++dirCounter))
	{}

	virtual ~DirectoryEntry() = default;

	const string createUniqueId() const override;
	const string createPrettyName() const override;
	const string createTooltip() const override;
	const string getCssClass() const override;

	string getName() const override;
	string getFsId() const override;

	bool isEmpty() const;

	BoxButtonCollection& getContents();

	DirNode* getParent();

protected:

	/// Stable app-wide identifier, set once at construction.
	const string fsId;

	/// Counter for stable code generation.
	inline static size_t dirCounter = 0;

	/// Stored files in this directory.
	Storage::BoxButtonCollection contents;

};

} // namespace
