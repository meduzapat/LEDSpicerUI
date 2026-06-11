/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LayoutElement.hpp
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
#include "LayoutTester.hpp"
#include "Storage/Element.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Layout {

/**
 * LEDSpicerUI::Ui::Layout::LayoutElement
 *
 * Free-positioned board tile holding a non-owning Storage::Element*.
 * Idle: name + icon. Active: also shows the RGB color picker and,
 * for strip elements, the per-LED strip view. The icon doubles as
 * the "All" fire button while active.
 */
class LayoutElement: public Gtk::EventBox {

public:

	enum class Kind : uint8_t { Mono, Solenoid, Rgb, Strip };

	static constexpr int
		ICON_PX           = 48,
		ELEMENT_WIDTH_PX  = 96,
		RGB_TOGGLE_PX     = 18,
		DRAG_THRESHOLD_PX = 5;

	LayoutElement(Storage::Element* element, LayoutTester* tester) noexcept;

	virtual ~LayoutElement();

	LayoutElement(const LayoutElement&)            = delete;
	LayoutElement& operator=(const LayoutElement&) = delete;

	void refresh() noexcept;

	void setActive(bool on) noexcept;

	bool isActive() const noexcept { return active; }

	Storage::Element* getElement() const noexcept { return element; }

	Kind getKind() const noexcept { return kind; }

	/** Fired when the tile becomes active so the board can deactivate the previous one. */
	sigc::signal<void, LayoutElement*>& signal_activated() noexcept { return activated; }

	/** Fired after a drag persists a new position, so the board can recompute its canvas. */
	sigc::signal<void>& signal_moved() noexcept { return moved; }

	static Kind categorize(const Storage::Element* element) noexcept;

private:

	static const string ICON_DIR;

	Storage::Element* element = nullptr;
	LayoutTester*     tester  = nullptr;

	Kind kind = Kind::Mono;

	Gtk::Box*      body      = nullptr;
	Gtk::Label*    nameLabel = nullptr;
	Gtk::Image*    iconImg   = nullptr;
	Gtk::Box*      rgbRow    = nullptr;
	StripRenderer* strip     = nullptr;

	Gtk::ToggleButton
		* rgbR = nullptr,
		* rgbG = nullptr,
		* rgbB = nullptr;

	bool
		active        = false,
		dragging      = false,
		dragMoved     = false,
		lightLocked   = false,
		pressedOnIcon = false;

	double
		dragOffsetX = 0.0,
		dragOffsetY = 0.0;

	sigc::connection
		layoutTimer,
		lightTimer;

	sigc::signal<void, LayoutElement*> activated;
	sigc::signal<void>                 moved;

	string pendingTintClass;

	void build() noexcept;
	void persistPosition() noexcept;
	void touchActivity() noexcept;
	bool onLayoutIdle() noexcept;
	bool onLightExpired() noexcept;
	void fire(Storage::Element* target) noexcept;
	void fireAll() noexcept;
	void fireCell(uint16_t firstPhysicalIdx) noexcept;
	const string& resolveColorName() const noexcept;

	bool onButtonPress(GdkEventButton* ev)   noexcept;
	bool onButtonRelease(GdkEventButton* ev) noexcept;
	bool onMotion(GdkEventMotion* ev)        noexcept;

	bool isInsideIcon(double x, double y) noexcept;

	static string iconForType(const string& typeId) noexcept;
	static const string& ledCssClass(const string& colorName) noexcept;
	static StripRenderer::LedColor colorEnumFor(const string& colorName) noexcept;
};

} // namespace
