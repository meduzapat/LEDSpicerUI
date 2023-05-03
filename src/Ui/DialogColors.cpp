/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogColors.cpp
 * @since     Feb 22, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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
#include "XMLHelper.hpp"

using namespace LEDSpicerUI::Ui;

vector<Gtk::Button*> DialogColors::colorButtons;
vector<Gtk::FlowBox*> DialogColors::colorBoxes;
DialogColors* DialogColors::dc = nullptr;

DialogColors* DialogColors::getInstance() {
	return dc ? dc : throw Message("Dialog Colors not initialized.");
}

DialogColors* DialogColors::getInstance(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not dc)
		builder->get_widget_derived("DialogColors", dc);
	return getInstance();
}

DialogColors::DialogColors(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::Dialog(obj)
{
	Gtk::Entry* filterEntry;
	Gtk::Button* removefilter;
	builder->get_widget("BtnColorOn",           BtnColorOn);
	builder->get_widget("BtnColorOff",          BtnColorOff);
	builder->get_widget("BtnColorRandom",       BtnColorRandom);
	builder->get_widget("BtnClearColor",        BtnClearColor);
	builder->get_widget("ContainerColorPicker", ContainerColorPicker);
	// Filter.
	builder->get_widget("InputFilterColor",     filterEntry);
	builder->get_widget("BntRemoveColorFilter", removefilter);

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
			Message::displayInfo("Only default colors are available.\nSelect a color file in configuration to get more colores", this);
		}
	});

	removefilter->signal_clicked().connect([filterEntry]() {
		filterEntry->set_text("");
	});
	filterEntry->signal_changed().connect([&, filterEntry]() {
		auto filterText = filterEntry->get_text().lowercase();
		for (auto child : ContainerColorPicker->get_children()) {
			auto c = dynamic_cast<Gtk::FlowBoxChild*>(child);
			auto b = dynamic_cast<Gtk::Button*>(c->get_child());
			auto t = b->get_tooltip_text().lowercase();
			if (t.find(filterText) != std::string::npos)
				child->show();
			else
				child->hide();
		}
	});
}

void DialogColors::setColorsFromFile(const string& path) {

	resetColorButtons();

	// Gather colors from file.
	unordered_map<string, string> colors;
	try {
		XMLHelper colorsXML(path, "Colors");
		tinyxml2::XMLElement* xmlElement = colorsXML.getRoot()->FirstChildElement("color");

		// Extract Colors.
		if (xmlElement) {
			for (; xmlElement; xmlElement = xmlElement->NextSiblingElement()) {
				auto colorAttr = colorsXML.processNode(xmlElement);
				XMLHelper::checkAttributes({NAME, "color"}, colorAttr, "color");
				if (colorAttr[NAME] == "Random" || colors.count(colorAttr[NAME]))
					continue;
				colors[colorAttr[NAME]] = colorAttr["color"];
			}
		}
	}
	catch (...) {}

	// Remove current CSS data (if any).
	auto styleContext = Gtk::StyleContext::create();
	if (currentProvider.get() != nullptr) {
		styleContext->remove_provider_for_screen(
			Gdk::Screen::get_default(),
			currentProvider
		);
	}

	// Set new CSS data (if colors > 0).
	string c = setColors(colors);
	if (c.size()) {
		currentProvider = Gtk::CssProvider::create();
		currentProvider->load_from_data(c);
		styleContext->add_provider_for_screen(
			Gdk::Screen::get_default(),
			currentProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
		);
	}
}

void DialogColors::colorizeButton(Gtk::Button* button, const string& colorName) {
	string
		oldClassName = button->get_tooltip_text(),
		newClassName = isValidColor(colorName) ? colorName : "";
	button->set_tooltip_text(newClassName);
	button->set_label(newClassName);
	auto sc = button->get_style_context();
	if (not oldClassName.empty() and sc->has_class(oldClassName))
		sc->remove_class(oldClassName);
	if (not newClassName.empty())
		sc->add_class(newClassName);
}

bool DialogColors::isValidColor(const string& colorName) {
	if (colorName == "On" or colorName == "Off" or colorName == "Random")
		return true;
	for (auto child : ContainerColorPicker->get_children()) {
		auto c = dynamic_cast<Gtk::FlowBoxChild*>(child);
		auto b = dynamic_cast<Gtk::Button*>(c->get_child());
		string t(b->get_tooltip_text());
		if (colorName == t)
			return true;
	}
	return false;
}

void DialogColors::activateColorButton(Gtk::Button* button) {
	colorButtons.push_back(button);
	button->signal_clicked().connect([&, button]() {
		if (run() == Gtk::ResponseType::RESPONSE_OK) {
			colorizeButton(button, selectedColor);
		}
		hide();
	});
}

void DialogColors::activateColorPicker(Gtk::Button* button, Gtk::FlowBox* destination) {
	colorBoxes.push_back(destination);
	button->signal_clicked().connect([&, button, destination]() {
		// Random cannot be used for list.
		BtnColorRandom->hide();
		BtnClearColor->hide();
		if (run() == Gtk::ResponseType::RESPONSE_OK) {
			auto box = Gtk::make_managed<Gtk::HBox>(false, 2);
			box->set_valign(Gtk::ALIGN_START);
			box->set_vexpand(false);
			box->set_margin_top(2);
			box->set_margin_bottom(2);
			box->set_margin_left(2);
			box->set_margin_right(2);
			box->get_style_context()->add_class("ColorButton");
			box->set_tooltip_text(selectedColor);
			auto l = Gtk::make_managed<Gtk::Label>(selectedColor);
			l->get_style_context()->add_class(selectedColor);
			auto b = Gtk::make_managed<Gtk::Button>();
			b->set_image_from_icon_name("edit-delete", Gtk::ICON_SIZE_BUTTON);
			b->signal_clicked().connect([box, destination]() {
				destination->remove(*box);
			});
			box->pack_start(*l, Gtk::PACK_EXPAND_WIDGET);
			box->pack_start(*b, Gtk::PACK_SHRINK);
			destination->add(*box);
			box->show_all();
		}
		BtnColorRandom->show();
		BtnClearColor->show();
		hide();
	});
}

void DialogColors::resetColorButtons() {
	for (auto button : colorButtons)
		if (not button->get_tooltip_text().empty())
			colorizeButton(button, "");
	for (auto box : colorBoxes)
		for (auto c : box->get_children())
			box->remove(*c);
}

void DialogColors::onColorSelected(Gtk::Button* button) {
	selectedColor = button->get_tooltip_text();
	response(Gtk::RESPONSE_OK);
}

string DialogColors::setColors(unordered_map<string, string>& colors) {
	// Remove previous buttons.
	for (auto c : ContainerColorPicker->get_children())
		ContainerColorPicker->remove(*c);

	string cssData;
	for (auto c : colors) {
		if (c.first == "On" or c.first == "Off" or c.first == "Random")
			// Discard any special color.
			continue;
		cssData += '.' + c.first + "{background:#" + c.second + ';';
		if (Message::getLiminance(c.second) > 0.5)
			cssData += "color:black;";
		cssData += '}';
		// Create Button.
		Gtk::Button* b = Gtk::make_managed<Gtk::Button>(c.first);
		b->set_tooltip_text(c.first);
		b->set_label(c.first);
		b->get_style_context()->add_class(c.first);
		ContainerColorPicker->add(*b);
		b->signal_clicked().connect(sigc::bind(
			sigc::mem_fun(*this, &DialogColors::onColorSelected),
			b
		));
	}
	ContainerColorPicker->show_all();
	return cssData;
}
