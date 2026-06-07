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
	stripSize  = newSize;
	groupRatio = stripSize > DISPLAY_CAP
		? static_cast<uint16_t>((stripSize + DISPLAY_CAP - 1) / DISPLAY_CAP)
		: 1;

	const uint16_t visible = groupRatio == 1
		? stripSize
		: static_cast<uint16_t>((stripSize + groupRatio - 1) / groupRatio);

	const uint16_t perRow {visible < MAX_CELLS_PER_ROW ? visible : MAX_CELLS_PER_ROW};
	set_min_children_per_line(perRow);
	set_max_children_per_line(perRow);

	cells.reserve(visible);
	cellTimers.assign(visible, sigc::connection{});
	for (uint16_t i = 0; i < visible; ++i) {
		auto cell {Gtk::manage(new Gtk::ToggleButton())};
		cell->set_size_request(12, 12);
		cell->set_can_focus(false);
		cell->set_relief(Gtk::RELIEF_NONE);
		cell->get_style_context()->add_class(CSS_STRIP_LED);
		cell->get_style_context()->add_class(cssClassFor(LedColor::Off));
		if (groupRatio > 1) {
			cell->set_tooltip_text(
				"LED group of " + std::to_string(groupRatio) +
				" (display capped at " + std::to_string(DISPLAY_CAP) + ")"
			);
		}
		const uint16_t firstPhysical = static_cast<uint16_t>(i * groupRatio);
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

void StripRenderer::scheduleCellOff(uint16_t physicalIdx) noexcept {
	const uint16_t i {static_cast<uint16_t>(physicalIdx / groupRatio)};
	cellTimers[i] = Glib::signal_timeout().connect([this, i, physicalIdx]() {
		cellTimers[i].disconnect();
		cells[i]->set_active(false);
		setLedState(physicalIdx, LedColor::Off);
		return false;
	}, LIGHT_TIMEOUT_MS);
}

void StripRenderer::setLedState(uint16_t idx, LedColor color) noexcept {
	const uint16_t displayIdx = static_cast<uint16_t>(idx / groupRatio);
	auto cell {cells.at(displayIdx)};
	auto ctx {cell->get_style_context()};
	for (auto c : {LedColor::Off, LedColor::R, LedColor::G, LedColor::B,
	               LedColor::Y, LedColor::M, LedColor::C, LedColor::W})
		ctx->remove_class(cssClassFor(c));
	ctx->add_class(cssClassFor(color));
}

void StripRenderer::setAllOff() noexcept {
	for (uint16_t i = 0; i < cells.size(); ++i) {
		cellTimers[i].disconnect();
		cells[i]->set_active(false);
		setLedState(static_cast<uint16_t>(i * groupRatio), LedColor::Off);
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
