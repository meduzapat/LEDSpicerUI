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

using namespace LEDSpicerUI::Ui::Layout;
using namespace LEDSpicerUI::Constants;

const string LayoutElement::ICON_DIR {RESOURCE_PREFIX + "images/elements/"};

LayoutElement::LayoutElement(Storage::Element* el) noexcept :
	Gtk::EventBox(),
	element {el}
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
	lightTimer.disconnect();
}

LayoutElement::Kind LayoutElement::categorize(const Storage::Element* el) noexcept {
	const auto& v {*el};
	if (v.isSet(STRIPSIZE))
		return Kind::Strip;
	if (v.isSet(SOLENOID))
		return Kind::Solenoid;
	if (v.isSet(POSITION) or v.isSet(POSITIONS) or (v.isSet(RED_PIN) and v.isSet(GREEN_PIN) and v.isSet(BLUE_PIN)))
		return Kind::Rgb;
	return Kind::Mono;
}

void LayoutElement::build() noexcept {
	body = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));
	add(*body);

	nameLabel = Gtk::manage(new Gtk::Label());
	setName(element->getPrimaryValue());
	body->pack_start(*nameLabel, Gtk::PACK_SHRINK);

	auto pix {getCachedIcon(element->getValue(TYPE))};

	iconImg = Gtk::manage(new Gtk::Image());
	iconImg->set(pix);
	iconImg->get_style_context()->add_class(CSS_LAYOUT_ELEMENT_ICON);

	iconBtnImg = Gtk::manage(new Gtk::Image());
	iconBtnImg->set(pix);
	iconBtn = Gtk::manage(new Gtk::ToggleButton());
	iconBtn->set_image(*iconBtnImg);
	iconBtn->set_relief(Gtk::RELIEF_NONE);
	iconBtn->set_can_focus(false);
	iconBtn->get_style_context()->add_class(CSS_LAYOUT_ELEMENT_ICON);
	iconBtn->signal_toggled().connect([this]() {
		if (not iconBtn->get_active()) return; // already toggled.
		fireAll();
	});

	rgbRow = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2));
	rgbRow->set_halign(Gtk::ALIGN_CENTER);
	if (kind == Kind::Rgb or kind == Kind::Strip) {
		auto makeTgl {[this](const char* cssClass) {
			auto tgl {Gtk::manage(new Gtk::ToggleButton())};
			tgl->set_can_focus(false);
			tgl->set_size_request(RGB_TOGGLE_PX, RGB_TOGGLE_PX);
			tgl->get_style_context()->add_class(cssClass);
			rgbRow->pack_start(*tgl, Gtk::PACK_SHRINK);
			return tgl;
		}};
		rgbR = makeTgl(CSS_LED_R);
		rgbG = makeTgl(CSS_LED_G);
		rgbB = makeTgl(CSS_LED_B);
	}

	auto btnEdit {Gtk::manage(new Gtk::Button())};
	btnEdit->set_image_from_icon_name(ICON_EDIT, Gtk::ICON_SIZE_BUTTON);
	btnEdit->set_can_focus(false);
	btnEdit->set_relief(Gtk::RELIEF_NONE);
	btnEdit->set_tooltip_text("Edit element");
	btnEdit->signal_clicked().connect([this]() { editRequested.emit(this); });
	rgbRow->pack_start(*btnEdit, Gtk::PACK_SHRINK);

	if (kind == Kind::Strip) {
		auto iconCol {Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2))};
		iconCol->pack_start(*iconImg, Gtk::PACK_SHRINK);
		iconCol->pack_start(*iconBtn, Gtk::PACK_SHRINK);
		iconCol->pack_start(*rgbRow, Gtk::PACK_SHRINK);

		auto iconStripRow {Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4))};
		iconStripRow->pack_start(*iconCol, Gtk::PACK_SHRINK);

		strip = Gtk::manage(new StripRenderer());
		strip->setCount(static_cast<uint16_t>(element->getInt(STRIPSIZE)));
		strip->signal_led_clicked().connect([this](uint16_t firstPhysicalIdx) {
			fireCell(firstPhysicalIdx);
		});
		strip->signal_cell_expired().connect([this](uint16_t firstPhysicalIdx) {
			clearCell(firstPhysicalIdx);
		});

		iconStripRow->pack_start(*strip, Gtk::PACK_EXPAND_WIDGET);
		body->pack_start(*iconStripRow, Gtk::PACK_SHRINK);
	}
	else {
		body->pack_start(*iconImg, Gtk::PACK_SHRINK);
		body->pack_start(*iconBtn, Gtk::PACK_SHRINK);
		body->pack_start(*rgbRow, Gtk::PACK_SHRINK);
	}

	show_all();
	iconBtn->hide();
	rgbRow->hide();
	if (strip)  strip->hide();
}

void LayoutElement::setName(const Glib::ustring& full) noexcept {
	const bool over {full.length() > NAME_MAX_CHARS};
	nameLabel->set_text(over ? full.substr(0, NAME_MAX_CHARS - 1) + "…" : full);
	if (over) nameLabel->set_tooltip_text(full);
	else      nameLabel->set_has_tooltip(false);
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
	setName(element->getPrimaryValue());
	auto pix {getCachedIcon(element->getValue(TYPE))};
	iconImg->set(pix);
	iconBtnImg->set(pix);
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
		iconImg->hide();
		iconBtn->show();
		rgbRow->show();
		if (strip)  strip->show();
		get_window()->raise();
		activated.emit(this);
	}
	else {
		ctx->remove_class(CSS_LAYOUT_ELEMENT_ACTIVE);
		iconBtn->hide();
		iconImg->show();
		rgbRow->hide();
		if (strip) strip->hide();
	}
}

void LayoutElement::stopTesting() noexcept {
	lightTimer.disconnect();
	if (not pendingTintClass.empty()) {
		auto ctx {iconBtn->get_style_context()};
		ctx->remove_class(CSS_LAYOUT_ELEMENT_FIRED);
		ctx->remove_class(pendingTintClass);
		pendingTintClass.clear();
	}
	iconBtn->set_active(false);
	if (strip) strip->setAllOff();
}

void LayoutElement::fire(Storage::Element* target) noexcept {
	if (lightTimer.connected()) return;
	lightTimer = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &LayoutElement::onLightExpired),
		Config::Settings::get().getLayoutTestTimeout()
	);
	lightTarget(target, resolveColorName());
}

void LayoutElement::fireAll() noexcept {
	if (lightTimer.connected()) return;
	const string
		& color {resolveColorName()},
		& colorClass {ledCssClass(color)};
	iconBtn->get_style_context()->add_class(CSS_LAYOUT_ELEMENT_FIRED);
	iconBtn->get_style_context()->add_class(colorClass);
	if (strip)
		strip->setAll(colorEnumFor(color));
	pendingTintClass = colorClass;
	debugHardwareTest((strip ? "Lighting strip " : "Lighting ") + element->getPrimaryValue() + ' ' + color);
	fire(element);
}

void LayoutElement::fireCell(uint16_t firstPhysicalIdx) noexcept {
	const string& color {resolveColorName()};
	strip->setLedState(firstPhysicalIdx, colorEnumFor(color));
	strip->scheduleCellOff(firstPhysicalIdx);
	// Light every element packed into the clicked segment; stop if a send is
	// dropped (e.g. testing paused mid-group for a daemon refresh).
	const auto children {element->copyStripChildren()};
	const uint16_t groupSize {strip->getGroupSize(firstPhysicalIdx)};
	debugHardwareTest("Lighting " + std::to_string(groupSize) + " LEDs " + color);
	for (uint16_t i {0}; i < groupSize; ++i)
		if (not lightTarget(children[firstPhysicalIdx + i], color))
			break;
}

void LayoutElement::clearCell(uint16_t firstPhysicalIdx) noexcept {
	// Clear the same elements the pack lit.
	const auto children {element->copyStripChildren()};
	const uint16_t groupSize {strip->getGroupSize(firstPhysicalIdx)};
	debugHardwareTest("Clearing " + std::to_string(groupSize) + " LEDs");
	for (uint16_t i {0}; i < groupSize; ++i)
		if (not clearTarget(children[firstPhysicalIdx + i]))
			break;
}

bool LayoutElement::lightTarget(Storage::Element* target, const string& colorName) noexcept {
	const bool useGroup {target->isSet(STRIPSIZE) or target->getProperties().isSet(PROP_STRIP_UID)};
	return DaemonHandler::getInstance().command(
		useGroup ? DaemonHandler::Command::SetGroup : DaemonHandler::Command::SetElement,
		{target->getPrimaryValue(), colorName, FILTER_NORMAL}
	);
}

bool LayoutElement::clearTarget(Storage::Element* target) noexcept {
	const bool useGroup {target->isSet(STRIPSIZE) or target->getProperties().isSet(PROP_STRIP_UID)};
	return DaemonHandler::getInstance().command(
		useGroup ? DaemonHandler::Command::ClearGroup : DaemonHandler::Command::ClearElement,
		{target->getPrimaryValue()}
	);
}

bool LayoutElement::onLightExpired() noexcept {
	if (not pendingTintClass.empty()) {
		iconBtn->get_style_context()->remove_class(CSS_LAYOUT_ELEMENT_FIRED);
		iconBtn->get_style_context()->remove_class(pendingTintClass);
		pendingTintClass.clear();
	}
	iconBtn->set_active(false);
	if (strip) strip->setAllOff();
	debugHardwareTest((strip ? "Clearing strip " : "Clearing ") + element->getPrimaryValue());
	clearTarget(element);
	return false;
}

void LayoutElement::debugHardwareTest(const string& message) noexcept {
	if (Config::Settings::get().shouldDebugHardwareTest())
		StatusBar::getInstance().push(message, StatusBar::Severity::Debug);
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
	if (active) return true;
	// Read-only root config: position writes are suppressed, so never start a drag.
	if (not Config::Settings::get().isRootConfigWritable()) return true;
	dragging    = true;
	dragMoved   = false;
	dragOffsetX = ev->x;
	dragOffsetY = ev->y;
	return true;
}

bool LayoutElement::onButtonRelease(GdkEventButton* ev) noexcept {
	if (ev->button != 1) return false;
	if (dragging) {
		dragging = false;
		if (dragMoved) {
			if (auto win {get_window()}) win->set_cursor();
			persistPosition();
			return false;
		}
	}
	// Elements are only testable when the board allows it (daemon up, fresh).
	if (not active and testGate and testGate()) {
		setActive(true);
		return false;
	}
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

	auto parent {static_cast<Gtk::Layout*>(get_parent())};
	const int
		x  {parent->child_property_x(*this).get_value()},
		y  {parent->child_property_y(*this).get_value()},
		nx {static_cast<int>(x + dx)},
		ny {static_cast<int>(y + dy)};
	const auto snap {[](int v, int g) {
		if (g <= 0) return v;
		const int n {(v + g / 2) / g * g};
		return std::abs(v - n) <= g / 3 ? n : v;
	}};
	const int
		g  {Config::Settings::get().getLayoutGrid()},
		cx {snap(nx < 0 ? 0 : nx, g)},
		cy {snap(ny < 0 ? 0 : ny, g)};
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

Glib::RefPtr<Gdk::Pixbuf> LayoutElement::getCachedIcon(const string& typeId) noexcept {
	static std::unordered_map<string, Glib::RefPtr<Gdk::Pixbuf>> cache;
	auto it {cache.find(typeId)};
	if (it != cache.end()) return it->second;
	auto pix {Gdk::Pixbuf::create_from_resource(ICON_DIR + iconForType(typeId), ICON_PX, ICON_PX, true)};
	cache.emplace(typeId, pix);
	return pix;
}
