/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirNode.hpp
 * @since     Feb 28, 2026
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

#include "Revertible.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirNode
 * Base for any object that lives inside a directory tree.
 * Provides parent pointer, path resolution, and name abstraction.
 */
class DirNode : public Revertible {

public:

	/**
	 * @param parent Parent node. nullptr = root level.
	 */
	DirNode(StringUMap& data, const DirNode* parent) :
	Revertible(data),
	parent(parent)
	{}

	virtual ~DirNode() = default;

	/**
	 * Returns this node's own name segment.
	 * DirectoryEntry returns its directory name.
	 * FileData returns its filename (no extension).
	 * @return Name string.
	 */
	virtual string getName() const = 0;

	/**
	 * @return Returns a unique identifier for this node
	 */
	virtual string getFsId() const = 0;

	/**
	 * Returns the parent node pointer.
	 * @return Parent pointer, or nullptr if at root level.
	 */
	const DirNode* getParent() const;

	/**
	 * Returns the full path of the parent.
	 * Empty string if this node is at root level.
	 * @return Parent full path string.
	 */
	string getPath() const;

	/**
	 * Returns the full relative path including this node's own name.
	 * Resolves recursively via the parent chain.
	 * e.g. root → dir1 → dir2 returns "dir1/dir2"
	 * @return Full path string.
	 */
	string getFullPath() const;

	/**
	 * @return true if this node is at root level (no parent).
	 */
	bool isRoot() const;

protected:

	/// Parent node. nullptr = root level.
	const DirNode* const parent;

};

} // namespace
