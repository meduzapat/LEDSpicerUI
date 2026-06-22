/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StripRenderer.cpp
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

#include "StripRenderer.hpp"
#include "Defaults.hpp"
#include "config/Settings.hpp"

using namespace LEDSpicerUI::Ui::Layout;

StripRenderer::StripRenderer() noexcept : Gtk::FlowBox() {
	set_selection_mode(Gtk::SELECTION_NONE);
	set_column_spacing(1);
	set_row_spacing(1);
	set_valign(Gtk::ALIGN_CENTER);
	set_can_focus(false);
	get_style_context()->add_class(CSS_STRIP_RENDERER);
}

void StripRenderer::setCount(uint16_t newSize) noexcept {
	clearCells();
	stripSize = newSize;
	packCount = stripSize < DISPLAY_CAP ? stripSize : DISPLAY_CAP;
	base = packCount ? stripSize / packCount : 1;
	rem  = packCount ? stripSize % packCount : 0;

	const uint16_t perRow {packCount < MAX_CELLS_PER_ROW ? packCount : MAX_CELLS_PER_ROW};
	set_min_children_per_line(perRow);
	set_max_children_per_line(perRow);

	cells.reserve(packCount);
	cellTimers.assign(packCount, sigc::connection{});
	for (uint16_t i = 0; i < packCount; ++i) {
		auto cell {Gtk::manage(new Gtk::ToggleButton())};
		cell->set_size_request(12, 12);
		cell->set_can_focus(false);
		cell->set_relief(Gtk::RELIEF_NONE);
		cell->get_style_context()->add_class(CSS_STRIP_LED);
		cell->get_style_context()->add_class(cssClassFor(LedColor::Off));
		const uint16_t firstPhysical {firstPhysicalOf(i)};
		if (const uint16_t groupSize {static_cast<uint16_t>(i < rem ? base + 1 : base)}; groupSize > 1)
			cell->set_tooltip_text("LED group of " + std::to_string(groupSize));
		cell->signal_toggled().connect([this, i, firstPhysical]() {
			if (cells[i]->get_active()) {
				if (cellTimers[i].connected()) return;
				ledClicked.emit(firstPhysical);
			}
			else if (cellTimers[i].connected()) {
				cells[i]->set_active(true);
			}
		});
		add(*cell);
		if (auto child {dynamic_cast<Gtk::FlowBoxChild*>(cell->get_parent())})
			child->set_can_focus(false);
		cells.push_back(cell);
	}
	show_all_children();
}

uint16_t StripRenderer::firstPhysicalOf(uint16_t cell) const noexcept {
	const uint16_t boundary {static_cast<uint16_t>(rem * (base + 1))};
	return cell < rem
		? static_cast<uint16_t>(cell * (base + 1))
		: static_cast<uint16_t>(boundary + (cell - rem) * base);
}

uint16_t StripRenderer::cellOf(uint16_t physicalIdx) const noexcept {
	const uint16_t boundary {static_cast<uint16_t>(rem * (base + 1))};
	return physicalIdx < boundary
		? static_cast<uint16_t>(physicalIdx / (base + 1))
		: static_cast<uint16_t>(rem + (physicalIdx - boundary) / base);
}

uint16_t StripRenderer::getGroupSize(uint16_t physicalIdx) const noexcept {
	return cellOf(physicalIdx) < rem ? base + 1 : base;
}

void StripRenderer::scheduleCellOff(uint16_t physicalIdx) noexcept {
	const uint16_t i {cellOf(physicalIdx)};
	cellTimers[i] = Glib::signal_timeout().connect([this, i, physicalIdx]() {
		cellTimers[i].disconnect();
		cells[i]->set_active(false);
		setLedState(physicalIdx, LedColor::Off);
		cellExpired.emit(physicalIdx);
		return false;
	}, Config::Settings::get().getLayoutTestTimeout());
}

void StripRenderer::setLedState(uint16_t idx, LedColor color) noexcept {
	auto cell {cells.at(cellOf(idx))};
	auto ctx {cell->get_style_context()};
	for (auto c : {LedColor::Off, LedColor::R, LedColor::G, LedColor::B, LedColor::Y, LedColor::M, LedColor::C, LedColor::W})
		ctx->remove_class(cssClassFor(c));
	ctx->add_class(cssClassFor(color));
}

void StripRenderer::setAll(LedColor color) noexcept {
	for (uint16_t i = 0; i < cells.size(); ++i)
		setLedState(firstPhysicalOf(i), color);
}

void StripRenderer::setAllOff() noexcept {
	for (uint16_t i = 0; i < cells.size(); ++i) {
		cellTimers[i].disconnect();
		cells[i]->set_active(false);
		setLedState(firstPhysicalOf(i), LedColor::Off);
	}
}

void StripRenderer::clearCells() noexcept {
	for (auto& c : cellTimers) c.disconnect();
	cellTimers.clear();
	for (auto cell : cells)
		remove(*cell);
	cells.clear();
}

const char* StripRenderer::cssClassFor(LedColor color) noexcept {
	switch (color) {
	case LedColor::Off: return CSS_LED_OFF;
	case LedColor::R:   return CSS_LED_R;
	case LedColor::G:   return CSS_LED_G;
	case LedColor::B:   return CSS_LED_B;
	case LedColor::Y:   return CSS_LED_Y;
	case LedColor::M:   return CSS_LED_M;
	case LedColor::C:   return CSS_LED_C;
	case LedColor::W:   return CSS_LED_W;
	}
	return CSS_LED_OFF;
}
