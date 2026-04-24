/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMap.hpp
 * @since     Sep 30, 2023
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

#include "Link.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::InputMap
 *
 * A single input map entry. Extends Link to wrap the target Element or Group
 * so renames and deletes propagate automatically via CollectionHandler.
 * Carries its own extra values: trigger, color, filter, type.
 * Unique ID: sourceID + "_" + trigger (scoped to the owning InputSource).
 * Source ID is stored as a property by DialogInputMap at creation time.
 */
class InputMap : public Link {

public:

	InputMap(StringUMap& data, Data* link) noexcept :
		Link(data, TARGET, TYPE_MAP, {}, link) {}

	virtual ~InputMap() = default;

	string_view getCssClass() const noexcept override { return "InputMapBoxButton"; }
	string createUniqueId()   const noexcept override;
	string createPrettyName() const noexcept override;
	string createTooltip()    const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override;

};

} // namespace
