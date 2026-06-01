/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogColors.cpp
 * @since     Feb 22, 2023
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

#include "DialogColors.hpp"

using namespace LEDSpicerUI::Ui;

vector<Gtk::Button*> DialogColors::colorButtons;
vector<Gtk::FlowBox*> DialogColors::colorBoxes;

DialogColors::DialogColors(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	GladeDialog(obj, builder)
{
	Gtk::SearchEntry* filterEntry;
	builder->get_widget("BtnColorOn",           BtnColorOn);
	builder->get_widget("BtnColorOff",          BtnColorOff);
	builder->get_widget("BtnColorRandom",       BtnColorRandom);
	builder->get_widget("BtnClearColor",        BtnClearColor);
	builder->get_widget("ContainerColorPicker", ContainerColorPicker);
	// Filter.
	builder->get_widget("InputFilterColor",     filterEntry);

	// Signals
	BtnColorOn->signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &DialogColors::onColorSelected),
		BtnColorOn
	));
	BtnColorOff->signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &DialogColors::onColorSelected),
		BtnColorOff
	));
	BtnColorRandom->signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &DialogColors::onColorSelected),
		BtnColorRandom
	));
	BtnClearColor->signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &DialogColors::onColorSelected),
		BtnClearColor
	));
	signal_show().connect([&, filterEntry]() {
		filterEntry->set_text("");
		if (not ContainerColorPicker->get_children().size()) {
			Message::displayInfo("Only default colors are available.\nSelect a color file in configuration to get more colors", this);
		}
	});

	Defaults::setFilter(filterEntry, ContainerColorPicker, this);
}

void DialogColors::setColorsFromFile(const string& path) noexcept {

	resetColorButtons();

	// Gather colors from file.
	Values colors;
	try {
		XMLHelper colorsXML(path, "Colors");
		tinyxml2::XMLElement* xmlElement = colorsXML.getRoot()->FirstChildElement("color");

		// Extract Colors.
		if (xmlElement) {
			for (; xmlElement; xmlElement = xmlElement->NextSiblingElement()) {
				const auto colorAttr {XMLHelper::processNode(xmlElement)};
				XMLHelper::checkAttributes({NAME, "color"}, colorAttr, "color");
				if (colorAttr.isA(NAME, HUMAN_RANDOM) || colors.isSet(colorAttr.getValue(NAME)))
					continue;
				colors.setValue(colorAttr.getValue(NAME), colorAttr.getValue(COLOR));
			}
		}
	}
	catch (...) {}

	// Remove current CSS data (if any).
	auto styleContext {Gtk::StyleContext::create()};
	if (currentProvider.get() != nullptr) {
		styleContext->remove_provider_for_screen(
			Gdk::Screen::get_default(),
			currentProvider
		);
	}

	// Set new CSS data (if colors > 0).
	if (const string c {setColors(colors)}; c.size()) {
		currentProvider = Gtk::CssProvider::create();
		currentProvider->load_from_data(c);
		styleContext->add_provider_for_screen(
			Gdk::Screen::get_default(),
			currentProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION - 1
		);
	}
}

void DialogColors::colorizeButton(Gtk::Button* button, const string& colorName) const noexcept {

	const string
		oldClassName {button->get_label()},
		newClassName {isValidColor(colorName) ? colorName : NO_COLOR};

	button->set_label(newClassName);

	auto sc {button->get_style_context()};
	if (not oldClassName.empty() and sc->has_class(oldClassName))
		sc->remove_class(oldClassName);

	if (not newClassName.empty())
		sc->add_class(newClassName);
}

bool DialogColors::isValidColor(const string& colorName) const noexcept {

	if (colorName == HUMAN_ON or colorName == HUMAN_OFF or colorName == HUMAN_RANDOM)
		return true;

	for (auto child : ContainerColorPicker->get_children()) {
		auto c {static_cast<Gtk::FlowBoxChild*>(child)};
		auto b {static_cast<Gtk::Button*>(c->get_child())};
		if (b->get_sensitive() and colorName == b->get_label())
			return true;
	}
	return false;
}

void DialogColors::activateColorButton(Gtk::Button* button) noexcept {
	colorButtons.push_back(button);
	button->signal_clicked().connect([this, button]() {
		if (run() == Gtk::ResponseType::RESPONSE_OK) {
			Defaults::markDirty();
			colorizeButton(button, selectedColor);
		}
		hide();
	});
}

void DialogColors::activateColorPicker(Gtk::Button* button, Gtk::FlowBox* destination) noexcept {
	colorBoxes.push_back(destination);
	button->signal_clicked().connect([this, destination]() {
		// Random cannot be used for list.
		BtnColorRandom->hide();
		BtnClearColor->hide();
		if (run() == Gtk::ResponseType::RESPONSE_OK) {
			createColorButton(destination, selectedColor);
			Defaults::markDirty();
		}
		BtnColorRandom->show();
		BtnClearColor->show();
		hide();
	});
}

void DialogColors::resetColorButtons() const noexcept {

	for (auto button : colorButtons)
		if (not button->get_label().empty())
			colorizeButton(button, "");

	for (auto box : colorBoxes)
		for (auto c : box->get_children())
			box->remove(*c);
}

void DialogColors::wipeColorPicker(Gtk::FlowBox* destination) noexcept {
	for (auto c : destination->get_children())
		destination->remove(*c);
}

void DialogColors::populateColorBox(Gtk::FlowBox* destination, const StringVector& colors) noexcept {
	for (const auto& c : colors)
		createColorButton(destination, c);
}

LEDSpicerUI::StringVector DialogColors::getColorBoxValues(Gtk::FlowBox* destination) const noexcept {
	StringVector r;
	for (auto child : destination->get_children()) {
		auto c {static_cast<Gtk::FlowBoxChild*>(child)};
		auto b {static_cast<Gtk::HBox*>(c->get_child())};
		// 1st child should be the label.
		r.push_back(static_cast<Gtk::Label*>(b->get_children()[0])->get_label());
	}
	return r;
}

void DialogColors::onColorSelected(Gtk::Button* button) noexcept {
	selectedColor = button->get_label();
	response(Gtk::ResponseType::RESPONSE_OK);
}

string DialogColors::setColors(const Values& colors) noexcept {
	const auto existing {ContainerColorPicker->get_children()};
	for (auto c : existing)
		ContainerColorPicker->remove(*c);

	string cssData;

	auto addButton = [&](const string& name, const string& hex, const string& tooltip, bool enabled) {
		cssData += '.' + name + "{background:#" + hex + ';';
		if (Defaults::getLuminance(hex) > 0.5) cssData += "color:black;";
		cssData += '}';
		Gtk::Button* b = Gtk::make_managed<Gtk::Button>(name);
		b->get_style_context()->add_class(name);
		b->get_style_context()->add_class("colorPickerButton");
		b->set_tooltip_text(tooltip);
		b->set_sensitive(enabled);
		ContainerColorPicker->add(*b);
		b->signal_clicked().connect(sigc::bind(
			sigc::mem_fun(*this, &DialogColors::onColorSelected),
			b
		));
	};

	// All 50 standard colors always rendered in family order.
	// File hex used when available; pure reference used as fallback.
	// Buttons not in the loaded file are shown but disabled.
	for (const auto& entry : Defaults::legalColors) {
		const bool calibrated = colors.isSet(entry.name);
		const string& hex     = calibrated ? colors.getValue(entry.name) : entry.pure;
		const string tooltip  = calibrated
			? entry.brief
			: entry.brief + "\n\nNot available in your color file.";
		addButton(entry.name, hex, tooltip, calibrated);
	}

	// Custom colors from the file not in the standard set — at the bottom.
	for (const auto& [name, hex] : colors) {
		if (name == HUMAN_ON or name == HUMAN_OFF or name == HUMAN_RANDOM) continue;
		if (Defaults::isLegalColor(name)) continue;
		addButton(name, hex, "Custom color.", true);
	}

	ContainerColorPicker->show_all();
	return cssData;
}

void DialogColors::createColorButton(Gtk::FlowBox* destination, const string& color) const noexcept {

	auto box {Gtk::make_managed<Gtk::HBox>(false, 2)};
	box->set_valign(Gtk::ALIGN_START);
	box->set_vexpand(false);
	box->set_margin_top(2);
	box->set_margin_bottom(2);
	box->set_margin_left(2);
	box->set_margin_right(2);
	box->get_style_context()->add_class(CSS_COLOR_BUTTON);

	auto l {Gtk::make_managed<Gtk::Label>(color)};
	l->get_style_context()->add_class(color);

	// Button delete.
	auto b {Gtk::make_managed<Gtk::Button>()};
	b->set_image_from_icon_name(ICON_DELETE, Gtk::ICON_SIZE_BUTTON);
	b->signal_clicked().connect([box, destination]() {
		Defaults::markDirty();
		destination->remove(*box);
	});

	box->pack_start(*l, Gtk::PACK_EXPAND_WIDGET);
	box->pack_start(*b, Gtk::PACK_SHRINK);
	destination->add(*box);
	box->show_all();
}
