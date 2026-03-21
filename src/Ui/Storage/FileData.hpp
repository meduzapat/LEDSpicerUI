/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileData.hpp
 * @since     Feb 23, 2026
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
#include "DirNode.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::FileData
 * Base for file-based items (Inputs, Animations, Profiles).
 * Inherits Revertible for serializable field storage and snap/restore support.
 * Inherits DirNode for parent pointer and recursive path resolution.
 * FILENAME is stored as a runtime property — never serialized to XML.
 * Subclasses add their own fields, collections, and visual decoration.
 */
class FileData : public Revertible, public DirNode {

public:

	/**
	 * @param data Field data.
	 * FILENAME is rescued into properties before being erased from fieldsData so it is never serialized.
	 * @param parent Owning directory node. nullptr = root of the type's tree.
	 */
	FileData(StringUMap& data, const DirNode* parent);

	virtual ~FileData() = default;

	const string createUniqueId() const override;

	string getName() const override;
	string getFsId() const override;

protected:

	/// Counter for stable id generation.
	inline static size_t fileCounter = 0;

};

} // namespace
