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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirNode
 * Pure structural mixin for objects that live inside a directory tree.
 * Provides parent pointer and recursive path resolution only.
 */
class DirNode {

public:

	/**
	 * @param parent Parent node. nullptr = root level.
	 */
	DirNode(DirNode* parent) : parent(parent) {}

	virtual ~DirNode() = default;

	/**
	 * Returns this node's own name segment.
	 * @return Name string.
	 */
	virtual const string& getName() const noexcept abstract;

	/**
	 * Returns a stable app-wide identifier for this node.
	 * @return Stable id string, e.g. "d_1", "f_3".
	 */
	virtual const string& getFsId() const noexcept abstract;

	/**
	 * Returns the parent node pointer.
	 * @return Parent pointer, or nullptr if at root level.
	 */
	DirNode* getParent() const noexcept { return parent; }

	/**
	 * Returns the full path of the parent.
	 * Empty string if this node is at root level.
	 * @return Parent full path string.
	 */
	string getPath() const noexcept;

	/**
	 * Returns the full relative path including this node's own name.
	 * Resolves recursively via the parent chain.
	 * e.g. root → dir1 → file returns "dir1/file"
	 * @return Full path string.
	 */
	string getFullPath() const noexcept;

	/**
	 * @return true if this node is at root level (no parent).
	 */
	bool isAtRoot() const noexcept { return not parent; }

protected:

	/// Parent node. nullptr = root level.
	DirNode* parent;

};

} // namespace
