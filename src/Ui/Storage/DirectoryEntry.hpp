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

#include "Parent.hpp"
#include "DirNode.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirectoryEntry
 * Represents a single directory node in the navigator tree.
 * Never serialized — exists only at runtime for navigation.
 * Inherits Parent for child item storage (dirs + inputs share COLLECTION_DIRECTORIES).
 * Inherits DirNode for node identity and path resolution.
 * TODO: add move support — requires reseating parent pointer on move.
 */
class DirectoryEntry : public Parent, public DirNode {

public:

	DirectoryEntry(
		StringUMap&     data,
		DirectoryEntry* parent
	) noexcept;

	virtual ~DirectoryEntry() = default;

	string createUniqueId()   const noexcept override;
	string createPrettyName() const noexcept override;
	string createTooltip()    const noexcept override;

	string_view getCssClass() const noexcept override { return CSS_DIRECTORY_BOX_BUTTON; }
	string_view getXmlTag()   const noexcept override { return ""; }

	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance(COLLECTION_DIRECTORIES);
	}

};

} // namespace
