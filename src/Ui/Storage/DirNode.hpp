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
#include "Values.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirNode
 * Structural mixin for objects that live inside a directory tree.
 * Owns the node identity (UID, PID, FILENAME) written into the
 * provided Values destination at construction.
 * Provides parent pointer and recursive path resolution.
 */
class DirNode {

public:

	/**
	 * @param dest     Values destination for UID, PID, FILENAME (values or properties).
	 * @param parent   Parent node. nullptr = root level.
	 * @param filename This node's name segment.
	 */
	DirNode(
		Values&        dest,
		DirNode*       parent,
		const string&  filename
	) noexcept;

	virtual ~DirNode() = default;

	/// Returns this node's name segment.
	const string& getName() const noexcept;

	/// Returns this node's stable runtime id.
	const string& getFsId() const noexcept;

	/// Returns the parent node pointer.
	DirNode* getParent() const noexcept { return parent; }

	/// Returns the full path of the parent. Empty if at root.
	string getPath() const noexcept;

	/// Returns the full relative path including this node's name.
	string getFullPath() const noexcept;

	/// @return true if this node is at root level (no parent).
	bool isAtRoot() const noexcept { return not parent; }

protected:

	/// Parent node. nullptr = root level.
	DirNode* parent;

private:

	/// Shared counter for all node types.
	inline static size_t nodeCounter = 0;

	/// Destination storage — values or properties of the owning Data.
	Values& dest;

};

} // namespace
