/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButton.cpp
 * @since     Feb 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "BoxButton.hpp"

using namespace LEDSpicerUI::Ui::Storage;

BoxButton::BoxButton(Data* form) :
	data(form),
	label(Gtk::make_managed<Gtk::Label>()),
	Gtk::HBox(false, 2)
{
	set_valign(Gtk::ALIGN_START);
	set_vexpand(false);
	set_margin_top(2);
	set_margin_bottom(2);
	set_margin_left(2);
	set_margin_right(2);

	Gtk::HBox* lbox(Gtk::make_managed<Gtk::HBox>());
	label->set_margin_left(5);
	label->set_margin_right(5);
	label->set_halign(Gtk::ALIGN_START);
	lbox->pack_start(*label, Gtk::PACK_EXPAND_WIDGET);
	pack_start(*lbox, Gtk::PACK_EXPAND_WIDGET);

	get_style_context()->add_class("BoxButton");
	if (not form->getCssClass().empty())
		get_style_context()->add_class(form->getCssClass());
	updateLabel();
	show_all();
}

BoxButton::~BoxButton() {
	if (data) {
		delete data;
		data = nullptr;
	}
}

void BoxButton::packButtonStart(Gtk::Button& button) {
	pack_start(button, Gtk::PACK_SHRINK);
}

Data* BoxButton::getData() {
	return data;
}

const Data* BoxButton::getData() const {
	return data;
}

void BoxButton::updateLabel() {
	string text(data->createTooltip());
	if (not text.empty()) {
		label->set_tooltip_text(text);
	}
	text = data->createPrettyName();
	label->set_text(text);
}
