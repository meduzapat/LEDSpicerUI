/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSource.hpp
 * @since     Feb 20, 2026
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

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::InputSource
 * Represents a single input source inside an Input.
 * Owns the maps belonging to this source.
 * Collection ID is scoped to its parent Input via ownerId.
 */
class InputSource : public Parent {

public:

	InputSource(StringUMap& data, const string& ownerId) noexcept;

	virtual ~InputSource() = default;

	string createUniqueId()   const noexcept override;
	string createPrettyName() const noexcept override;
	string createTooltip()    const noexcept override;
	string_view getCssClass() const noexcept override { return "InputSourceBoxButton"; }
	string_view getXmlTag()   const noexcept override { return "maps"; }

	CollectionHandler* getCollectionHandler() const noexcept override;

protected:

	string getPrimaryKey() const noexcept override { return SOURCE; }

	bool shouldSerialize(const string& key, const string& value) const noexcept override;

private:

	/// Counter for stable UID generation.
	inline static size_t sourceCounter = 0;

};

} // namespace
