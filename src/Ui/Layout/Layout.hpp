/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Layout.hpp
 * @since     Jun 8, 2026
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

#include "LayoutElement.hpp"
#include "LayoutBoard.hpp"
#include "Storage/ElementObserver.hpp"
#include "Storage/BoxButtonCollection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Layout {

/**
 * LEDSpicerUI::Ui::Layout::Layout
 *
 * Owns the visual board: subscribes to Storage::Element lifecycle events
 * and maintains one LayoutElement tile per real Element (strip children
 * are filtered). Enforces single-active selection across tiles.
 */
class Layout: public Storage::ElementObserver {

public:

	static constexpr int
		PLACE_MARGIN_X = 10,
		PLACE_MARGIN_Y = 10,
		PLACE_STEP_X   = 110,
		PLACE_STEP_Y   = 120,
		PLACE_COLUMNS  = 8,
		BOARD_PAD      = 10,
		TILE_FALLBACK_PX = 100;

	Layout(
		const Glib::RefPtr<Gtk::Builder>& builder,
		Storage::BoxButtonCollection* devicesCollection
	) noexcept;

	virtual ~Layout();

	Layout(const Layout&)            = delete;
	Layout& operator=(const Layout&) = delete;

	void onAdded(Storage::Element* element)   noexcept override;
	void onRemoved(Storage::Element* element) noexcept override;
	void onChanged(Storage::Element* element) noexcept override;

	/**
	 * Closes the active element, if any (e.g. when daemon testing stops).
	 */
	void deactivate() noexcept;

private:

	LayoutBoard*  board  = nullptr;

	/// Points to Devices Collection to lookup the Device and Element on edit.
	Storage::BoxButtonCollection* devicesCollection = nullptr;

	std::unordered_map<Storage::Element*, LayoutElement*> tiles;

	LayoutElement* current = nullptr;

	size_t placementCursor = 0;

	void placeNew(LayoutElement* tile) noexcept;
	void onTileActivated(LayoutElement* tile) noexcept;
	void onEditRequested(LayoutElement* tile) noexcept;
	void recomputeBoardSize() noexcept;

	/**
	 * Walks the Devices collection to find the owning Device and the Element's BoxButton.
	 * @param element the Element to look up.
	 * @return pair (Device BoxButton, Element BoxButton).
	 */
	std::pair<Storage::BoxButton*, Storage::BoxButton*>
		resolveButtons(Storage::Element* element) const noexcept;
};

} // namespace
