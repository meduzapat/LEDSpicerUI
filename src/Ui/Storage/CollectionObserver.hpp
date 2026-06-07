/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionObserver.hpp
 * @since     Jun 7, 2026
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

#include "Data.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::CollectionObserver
 *
 * Receives lifecycle events from a CollectionHandler.
 * A handler holds at most one observer, set during bootstrap and never cleared.
 */
class CollectionObserver {

public:

	virtual ~CollectionObserver() = default;

	/**
	 * Fired after item has been added to the collection.
	 */
	virtual void onAdded(Data* item) noexcept abstract;

	/**
	 * Fired before item is erased from the collection.
	 * The pointer is still valid and still resolvable in the collection at this point.
	 */
	virtual void onRemoved(Data* item) noexcept abstract;

	/**
	 * Fired when an item's values change. Identity (pointer) is unchanged.
	 */
	virtual void onChanged(Data* item) noexcept abstract;
};

} // namespace
