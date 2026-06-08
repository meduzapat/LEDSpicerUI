/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Layout.cpp
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

#include "Layout.hpp"
#include "Defaults.hpp"

using namespace LEDSpicerUI::Ui::Layout;
using namespace LEDSpicerUI::Constants;

Layout::Layout(const Glib::RefPtr<Gtk::Builder>& builder, LayoutTester* t) noexcept :
	tester {t}
{
	builder->get_widget("LayoutBoard", board);
}

Layout::~Layout() {
	for (auto& t : tiles)
		board->remove(*t.second);
	tiles.clear();
}

void Layout::onAdded(Storage::Element* element) noexcept {
	if (element->getProperties().isSet(PROP_STRIP)) return;

	auto tile {Gtk::manage(new LayoutElement(element, tester))};
	tiles.emplace(element, tile);
	placeNew(tile);
	tile->signal_activated().connect(sigc::mem_fun(*this, &Layout::onTileActivated));
	tile->signal_moved().connect(sigc::mem_fun(*this, &Layout::recomputeBoardSize));
}

void Layout::onRemoved(Storage::Element* element) noexcept {
	auto it {tiles.find(element)};
	if (it == tiles.end()) return;
	if (current == it->second) current = nullptr;
	board->remove(*it->second);
	tiles.erase(it);
}

void Layout::onChanged(Storage::Element* element) noexcept {
	auto it {tiles.find(element)};
	if (it == tiles.end()) return;
	it->second->refresh();
}

void Layout::placeNew(LayoutElement* tile) noexcept {
	auto element {tile->getElement()};
	int x {0}, y {0};
	if (element->isSet(LAYOUT_X) and element->isSet(LAYOUT_Y)) {
		x = element->getInt(LAYOUT_X);
		y = element->getInt(LAYOUT_Y);
	}
	else {
		x = PLACE_MARGIN_X + static_cast<int>(placementCursor % PLACE_COLUMNS) * PLACE_STEP_X;
		y = PLACE_MARGIN_Y + static_cast<int>(placementCursor / PLACE_COLUMNS) * PLACE_STEP_Y;
		++placementCursor;
	}
	board->put(*tile, x, y);
	tile->show();
	recomputeBoardSize();
}

void Layout::recomputeBoardSize() noexcept {
	guint w {0}, h {0};
	for (auto& t : tiles) {
		auto tile {t.second};
		const int tx {board->child_property_x(*tile).get_value()};
		const int ty {board->child_property_y(*tile).get_value()};
		const int tw {tile->get_allocated_width()};
		const int th {tile->get_allocated_height()};
		const guint right  {static_cast<guint>(tx + (tw > 0 ? tw : TILE_FALLBACK_PX) + BOARD_PAD)};
		const guint bottom {static_cast<guint>(ty + (th > 0 ? th : TILE_FALLBACK_PX) + BOARD_PAD)};
		if (right  > w) w = right;
		if (bottom > h) h = bottom;
	}
	board->set_size(w, h);
}

void Layout::onTileActivated(LayoutElement* tile) noexcept {
	if (current == tile) return;
	if (current) current->setActive(false);
	current = tile;
}
