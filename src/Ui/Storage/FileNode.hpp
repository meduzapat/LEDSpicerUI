/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileNode.hpp
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

#include "Parent.hpp"
#include "DirNode.hpp"
#include "XMLHelper.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::FileNode
 * Base for file-based items (Inputs, Animations, Profiles).
 * Inherits Parent for field storage and child collection ownership.
 * Inherits DirNode for parent pointer and recursive path resolution.
 * FILENAME is stored as a runtime property — never serialized to XML.
 */
class FileNode : public Parent, public DirNode {

public:

	FileNode(
		StringUMap& data,
		DirNode* parent,
		const string& collectionId,
		const vector<string>& childIds
	) noexcept;

	virtual ~FileNode() = default;

	const string createUniqueId() const noexcept override;

	const string& getName() const noexcept override;
	const string& getFsId() const noexcept override;

	/**
	 * Serializes this file-based item using XMLHelper::xmlHeader/Footer.
	 * Subclasses provide xmlBody() for inner content.
	 * @return Full XML file string.
	 */
	const string toXML() const noexcept override;

protected:

	/// Counter for stable id generation.
	inline static size_t fileCounter = 0;

};

} // namespace
