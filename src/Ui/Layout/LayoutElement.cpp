/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LayoutElement.cpp
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
#include "Defaults.hpp"

using namespace LEDSpicerUI::Ui::Layout;
using namespace LEDSpicerUI::Constants;

static constexpr const char* ICON_RESOURCE_PREFIX{"/org/ledspicer/ui/images/elements/"};

LayoutElement::LayoutElement(Storage::Element* el, LayoutTester* t) noexcept :
	Gtk::EventBox(),
	element {el},
	tester  {t}
{
	kind = categorize(element);
	get_style_context()->add_class(CSS_LAYOUT_ELEMENT);
	set_visible_window(true);
	add_events(
		Gdk::BUTTON_PRESS_MASK   |
		Gdk::BUTTON_RELEASE_MASK |
		Gdk::POINTER_MOTION_MASK
	);
	if (kind != Kind::Strip)
		set_size_request(ELEMENT_WIDTH_PX, -1);
	build();

	signal_button_press_event()  .connect(sigc::mem_fun(*this, &LayoutElement::onButtonPress));
	signal_button_release_event().connect(sigc::mem_fun(*this, &LayoutElement::onButtonRelease));
	signal_motion_notify_event() .connect(sigc::mem_fun(*this, &LayoutElement::onMotion));
}

LayoutElement::~LayoutElement() {
	layoutTimer.disconnect();
	lightTimer.disconnect();
}

LayoutElement::Kind LayoutElement::categorize(const Storage::Element* el) noexcept {
	const auto& v {*el};
	if (v.isSet(STRIPSIZE))
		return Kind::Strip;
	if (v.isSet(SOLENOID))
		return Kind::Solenoid;
	if (v.isSet(POSITION) or v.isSet(POSITIONS) or
	    (v.isSet(RED_PIN) and v.isSet(GREEN_PIN) and v.isSet(BLUE_PIN)))
		return Kind::Rgb;
	return Kind::Mono;
}

void LayoutElement::build() noexcept {
	body = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));
	add(*body);

	nameLabel = Gtk::manage(new Gtk::Label(element->getPrimaryValue()));
	nameLabel->set_ellipsize(Pango::ELLIPSIZE_END);
	nameLabel->set_max_width_chars(12);
	body->pack_start(*nameLabel, Gtk::PACK_SHRINK);

	iconImg = Gtk::manage(new Gtk::Image());
	iconImg->set(Gdk::Pixbuf::create_from_resource(
		ICON_RESOURCE_PREFIX + iconForType(element->getValue(TYPE)),
		ICON_PX, ICON_PX, true
	));
	iconImg->get_style_context()->add_class(CSS_LAYOUT_ELEMENT_ICON);

	if (kind == Kind::Rgb or kind == Kind::Strip) {
		rgbRow = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2));
		rgbRow->set_halign(Gtk::ALIGN_CENTER);
		auto makeTgl {[this](const char* cssClass) {
			auto* tgl {Gtk::manage(new Gtk::ToggleButton())};
			tgl->set_can_focus(false);
			tgl->set_size_request(RGB_TOGGLE_PX, RGB_TOGGLE_PX);
			tgl->get_style_context()->add_class(cssClass);
			tgl->signal_toggled().connect([this]() { touchActivity(); });
			rgbRow->pack_start(*tgl, Gtk::PACK_SHRINK);
			return tgl;
		}};
		rgbR = makeTgl(CSS_LED_R);
		rgbG = makeTgl(CSS_LED_G);
		rgbB = makeTgl(CSS_LED_B);
	}

	if (kind == Kind::Strip) {
		auto* iconCol {Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2))};
		iconCol->pack_start(*iconImg, Gtk::PACK_SHRINK);
		iconCol->pack_start(*rgbRow, Gtk::PACK_SHRINK);
		auto* iconStripRow {Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4))};
		iconStripRow->pack_start(*iconCol, Gtk::PACK_SHRINK);
		strip = Gtk::manage(new StripRenderer());
		strip->setCount(static_cast<uint16_t>(element->getInt(STRIPSIZE)));
		strip->signal_led_clicked().connect([this](uint16_t firstPhysicalIdx) {
			touchActivity();
			fireCell(firstPhysicalIdx);
		});
		iconStripRow->pack_start(*strip, Gtk::PACK_EXPAND_WIDGET);
		body->pack_start(*iconStripRow, Gtk::PACK_SHRINK);
	}
	else {
		body->pack_start(*iconImg, Gtk::PACK_SHRINK);
		if (rgbRow) body->pack_start(*rgbRow, Gtk::PACK_SHRINK);
	}

	show_all();
	if (rgbRow) rgbRow->hide();
	if (strip)  strip->hide();
}


void LayoutElement::refresh() noexcept {
	const Kind newKind {categorize(element)};
	if (newKind != kind) {
		for (auto child : body->get_children())
			body->remove(*child);
		body = nullptr;
		kind = newKind;
		build();
		return;
	}
	nameLabel->set_text(element->getPrimaryValue());
	iconImg->set(Gdk::Pixbuf::create_from_resource(
		ICON_RESOURCE_PREFIX + iconForType(element->getValue(TYPE)),
		ICON_PX, ICON_PX, true
	));
	if (kind == Kind::Strip) {
		const uint16_t count {static_cast<uint16_t>(element->getInt(STRIPSIZE))};
		if (count != strip->getCount())
			strip->setCount(count);
	}
}

void LayoutElement::setActive(bool on) noexcept {
	if (active == on) return;
	active = on;
	auto ctx {get_style_context()};
	if (active) {
		ctx->add_class(CSS_LAYOUT_ELEMENT_ACTIVE);
		if (rgbR) rgbR->set_active(false);
		if (rgbG) rgbG->set_active(false);
		if (rgbB) rgbB->set_active(false);
		if (rgbRow) rgbRow->show();
		if (strip)  strip->show();
		if (auto win {get_window()}) win->raise();
		layoutTimer.disconnect();
		layoutTimer = Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &LayoutElement::onLayoutIdle),
			LAYOUT_TIMEOUT_MS
		);
		activated.emit(this);
	}
	else {
		ctx->remove_class(CSS_LAYOUT_ELEMENT_ACTIVE);
		if (rgbRow) rgbRow->hide();
		if (strip)  strip->hide();
		layoutTimer.disconnect();
	}
}

void LayoutElement::touchActivity() noexcept {
	layoutTimer.disconnect();
	layoutTimer = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &LayoutElement::onLayoutIdle),
		LAYOUT_TIMEOUT_MS
	);
}

bool LayoutElement::onLayoutIdle() noexcept {
	setActive(false);
	return false;
}

void LayoutElement::fire(Storage::Element* target) noexcept {
	if (lightLocked) return;
	const string color {resolveColorName()};
	lightLocked = true;
	lightTimer.disconnect();
	lightTimer = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &LayoutElement::onLightExpired),
		LIGHT_TIMEOUT_MS
	);
	tester->test(target, color);
}

void LayoutElement::fireAll() noexcept {
	if (lightLocked) return;
	const string& color {resolveColorName()};
	const string& colorClass {ledCssClass(color)};
	iconImg->get_style_context()->add_class(CSS_LAYOUT_ELEMENT_FIRED);
	iconImg->get_style_context()->add_class(colorClass);
	if (strip) {
		const auto ledColor {colorEnumFor(color)};
		for (uint16_t i = 0; i < strip->getCount(); i += strip->getGroupRatio())
			strip->setLedState(i, ledColor);
	}
	pendingTintClass = colorClass;
	fire(element);
}

void LayoutElement::fireCell(uint16_t firstPhysicalIdx) noexcept {
	const string& color {resolveColorName()};
	strip->setLedState(firstPhysicalIdx, colorEnumFor(color));
	strip->scheduleCellOff(firstPhysicalIdx);
	tester->test(element->copyStripChildren()[firstPhysicalIdx], color);
}

bool LayoutElement::onLightExpired() noexcept {
	lightLocked = false;
	if (not pendingTintClass.empty()) {
		iconImg->get_style_context()->remove_class(CSS_LAYOUT_ELEMENT_FIRED);
		iconImg->get_style_context()->remove_class(pendingTintClass);
		pendingTintClass.clear();
	}
	if (strip) strip->setAllOff();
	return false;
}

const string& LayoutElement::resolveColorName() const noexcept {
	if (not rgbR) return COLOR_WHITE;
	const uint8_t mask {static_cast<uint8_t>(
		(rgbR->get_active() ? 4 : 0) |
		(rgbG->get_active() ? 2 : 0) |
		(rgbB->get_active() ? 1 : 0)
	)};
	switch (mask) {
	case 0b100: return COLOR_RED;
	case 0b010: return COLOR_GREEN;
	case 0b001: return COLOR_BLUE;
	case 0b110: return COLOR_YELLOW;
	case 0b101: return COLOR_MAGENTA;
	case 0b011: return COLOR_CYAN;
	default:    return COLOR_WHITE;
	}
}

void LayoutElement::persistPosition() noexcept {
	auto parent {dynamic_cast<Gtk::Layout*>(get_parent())};
	if (not parent) return;
	const int
		x {parent->child_property_x(*this).get_value()},
		y {parent->child_property_y(*this).get_value()};
	element->setValue(LAYOUT_X, x);
	element->setValue(LAYOUT_Y, y);
	Defaults::markDirty();
	moved.emit();
}

bool LayoutElement::onButtonPress(GdkEventButton* ev) noexcept {
	if (ev->button != 1) return false;
	pressedOnIcon = isInsideIcon(ev->x, ev->y);
	if (active) return false;
	dragging    = true;
	dragMoved   = false;
	dragOffsetX = ev->x;
	dragOffsetY = ev->y;
	return false;
}

bool LayoutElement::isInsideIcon(double x, double y) noexcept {
	int icon_x, icon_y;
	iconImg->translate_coordinates(*this, 0, 0, icon_x, icon_y);
	const int
		iw {iconImg->get_allocated_width()},
		ih {iconImg->get_allocated_height()};
	return x >= icon_x and x < icon_x + iw and y >= icon_y and y < icon_y + ih;
}

bool LayoutElement::onButtonRelease(GdkEventButton* ev) noexcept {
	if (ev->button != 1) return false;
	if (dragging) {
		dragging = false;
		if (dragMoved) {
			if (auto win {get_window()}) win->set_cursor();
			persistPosition();
			touchActivity();
			return false;
		}
	}
	if (not active) {
		setActive(true);
		return false;
	}
	touchActivity();
	if (pressedOnIcon and isInsideIcon(ev->x, ev->y))
		fireAll();
	return false;
}

bool LayoutElement::onMotion(GdkEventMotion* ev) noexcept {
	if (not dragging) return false;
	const double
		dx {ev->x - dragOffsetX},
		dy {ev->y - dragOffsetY};
	if (not dragMoved and dx * dx + dy * dy < DRAG_THRESHOLD_PX * DRAG_THRESHOLD_PX)
		return false;
	if (not dragMoved) {
		if (auto win {get_window()}) win->set_cursor(Gdk::Cursor::create(Gdk::FLEUR));
	}
	auto parent {dynamic_cast<Gtk::Layout*>(get_parent())};
	if (not parent) return false;
	const int
		x  {parent->child_property_x(*this).get_value()},
		y  {parent->child_property_y(*this).get_value()},
		nx {static_cast<int>(x + dx)},
		ny {static_cast<int>(y + dy)};
	const int cx {nx < 0 ? 0 : nx}, cy {ny < 0 ? 0 : ny};
	parent->move(*this, cx, cy);
	guint cw, ch;
	parent->get_size(cw, ch);
	const guint
		want_w {static_cast<guint>(cx + 150)},
		want_h {static_cast<guint>(cy + 200)};
	if (want_w > cw or want_h > ch)
		parent->set_size(want_w > cw ? want_w : cw, want_h > ch ? want_h : ch);
	dragMoved = true;
	return true;
}

const string& LayoutElement::ledCssClass(const string& colorName) noexcept {
	static const string
		r {CSS_LED_R},
		g {CSS_LED_G},
		b {CSS_LED_B},
		y {CSS_LED_Y},
		m {CSS_LED_M},
		c {CSS_LED_C},
		w {CSS_LED_W};
	if (colorName == COLOR_RED)     return r;
	if (colorName == COLOR_GREEN)   return g;
	if (colorName == COLOR_BLUE)    return b;
	if (colorName == COLOR_YELLOW)  return y;
	if (colorName == COLOR_MAGENTA) return m;
	if (colorName == COLOR_CYAN)    return c;
	return w;
}

StripRenderer::LedColor LayoutElement::colorEnumFor(const string& colorName) noexcept {
	if (colorName == COLOR_RED)     return StripRenderer::LedColor::R;
	if (colorName == COLOR_GREEN)   return StripRenderer::LedColor::G;
	if (colorName == COLOR_BLUE)    return StripRenderer::LedColor::B;
	if (colorName == COLOR_YELLOW)  return StripRenderer::LedColor::Y;
	if (colorName == COLOR_MAGENTA) return StripRenderer::LedColor::M;
	if (colorName == COLOR_CYAN)    return StripRenderer::LedColor::C;
	return StripRenderer::LedColor::W;
}

string LayoutElement::iconForType(const string& typeId) noexcept {
	// Type id is guaranteed valid by retrieveData() repair before this is reached.
	return "element-" + typeId + ".png";
}
