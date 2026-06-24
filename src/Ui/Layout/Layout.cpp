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
#include "DataDialogs/DialogDevice.hpp"
#include "Storage/Device.hpp"

using namespace LEDSpicerUI::Ui::Layout;
using namespace LEDSpicerUI::Constants;

Layout::Layout(
	const Glib::RefPtr<Gtk::Builder>& builder,
	Storage::BoxButtonCollection* devices
) noexcept :
	devicesCollection {devices}
{
	builder->get_widget_derived("LayoutBoard", board);
	board->add_events(Gdk::BUTTON_PRESS_MASK);
	board->signal_button_press_event().connect([this](GdkEventButton*) {
		if (current) {
			current->setActive(false);
			current = nullptr;
		}
		return false;
	});
}

Layout::~Layout() {
	for (auto& t : tiles)
		board->remove(*t.second);
	tiles.clear();
}

void Layout::onAdded(Storage::Element* element) noexcept {
	if (element->getProperties().isSet(PROP_STRIP)) return;

	auto tile {Gtk::manage(new LayoutElement(element))};
	tiles.emplace(element, tile);
	placeNew(tile);
	tile->signal_activated().connect(sigc::mem_fun(*this, &Layout::onTileActivated));
	tile->signal_moved().connect(sigc::mem_fun(*this, &Layout::recomputeBoardSize));
	tile->signal_editRequested().connect(sigc::mem_fun(*this, &Layout::onEditRequested));
	tile->setTestGate([this] { return testing; });
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

void Layout::setTesting(bool on) noexcept {
	testing = on;
	if (on)
		return;
	// Disabling testing closes the active tile and cancels its in-flight light.
	if (current) {
		current->stopTesting();
		current->setActive(false);
		current = nullptr;
	}
}

void Layout::onEditRequested(LayoutElement* tile) noexcept {
	auto element {tile->getElement()};
	auto [device, child] {resolveButtons(element)};
	DataDialogs::DialogDevice::getInstance()->editChild(*device, *child);
}

std::pair<LEDSpicerUI::Ui::Storage::BoxButton*, LEDSpicerUI::Ui::Storage::BoxButton*>
Layout::resolveButtons(Storage::Element* element) const noexcept {
	for (auto device : *devicesCollection) {
		auto elements {static_cast<Storage::Device*>(device->getData())->getChild(COLLECTION_ELEMENTS)};
		for (auto child : *elements)
			if (child->getData() == element)
				return std::make_pair(device, child);
	}
	// Unreachable: a tile only exists for an Element owned by a Device in the collection.
	return std::make_pair(nullptr, nullptr);
}
