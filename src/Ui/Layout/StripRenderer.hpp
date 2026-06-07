/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StripRenderer.hpp
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

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <cstdint>
#include <vector>

#pragma once

namespace LEDSpicerUI::Ui::Layout {

/**
 * LEDSpicerUI::Ui::Layout::StripRenderer
 *
 * FlowBox of toggle cells representing an addressable LED strip.
 * When stripSize exceeds DISPLAY_CAP, cells are grouped: each visible
 * toggle represents ceil(stripSize / DISPLAY_CAP) physical LEDs.
 */
class StripRenderer: public Gtk::FlowBox {

public:

	/// Maximum number of toggles displayed. Real strips longer than this are grouped.
	static constexpr uint16_t DISPLAY_CAP       = 50;

	/// Soft cap for cells per visual row (forces 3–5 row pile-up on long strips).
	static constexpr uint16_t MAX_CELLS_PER_ROW = 12;

	/// Logical color of a single LED (or group). CSS classes match these names.
	enum class LedColor : uint8_t { Off, R, G, B, Y, M, C, W };

	StripRenderer() noexcept;

	virtual ~StripRenderer() = default;

	StripRenderer(const StripRenderer&)            = delete;
	StripRenderer& operator=(const StripRenderer&) = delete;

	void setCount(uint16_t stripSize) noexcept;

	/** Physical idx is mapped onto its display cell when grouping is active. */
	void setLedState(uint16_t idx, LedColor color) noexcept;

	void setAllOff() noexcept;

	/** Schedules the cell containing physicalIdx to revert to Off after LIGHT_TIMEOUT_MS. */
	void scheduleCellOff(uint16_t physicalIdx) noexcept;

	/** Argument = index of the first physical LED in the clicked group.
	 *  Emitted only when a cell transitions OFF → ON via user click;
	 *  rejected re-clicks during a live cell timer do not emit. */
	sigc::signal<void, uint16_t>& signal_led_clicked() noexcept { return ledClicked; }

	uint16_t getCount() const noexcept { return stripSize; }

	uint16_t getGroupRatio() const noexcept { return groupRatio; }

private:

	uint16_t
		stripSize  = 0,
		groupRatio = 1;

	std::vector<Gtk::ToggleButton*> cells;

	std::vector<sigc::connection> cellTimers;

	sigc::signal<void, uint16_t> ledClicked;

	void clearCells() noexcept;

	static const char* cssClassFor(LedColor color) noexcept;
};

} // namespace
