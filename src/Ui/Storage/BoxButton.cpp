/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButton.cpp
 * @since     Feb 13, 2023
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

#include "BoxButton.hpp"

using namespace LEDSpicerUI::Ui::Storage;

BoxButton::BoxButton(Data* form) noexcept :
	Gtk::FlowBoxChild(),
	data{form},
	label{Gtk::make_managed<Gtk::Label>()},
	contentBox{Gtk::make_managed<Gtk::HBox>(false, 2)}
{
	set_valign(Gtk::ALIGN_START);
	set_vexpand(false);
	set_can_focus(false);

	Gtk::HBox* lbox{Gtk::make_managed<Gtk::HBox>()};
	lbox->pack_start(*label, Gtk::PACK_EXPAND_WIDGET);
	lbox->set_visible(true);
	label->set_margin_left(5);
	label->set_margin_right(5);
	label->set_halign(Gtk::ALIGN_START);
	label->set_visible(true);
	contentBox->pack_start(*lbox, Gtk::PACK_EXPAND_WIDGET);

	get_style_context()->add_class(CSS_BOX_BUTTON);
	const auto& cssClass{form->getCssClass()};
	if (not cssClass.empty())
		get_style_context()->add_class(cssClass);
	add(*contentBox);
	data->registerToCollection();
	sync();
	show_all();
}

BoxButton::~BoxButton() {
	if (data) {
		data->unregisterFromCollection();
		delete data;
		data = nullptr;
	}
}

void BoxButton::packButtonStart(Gtk::Button& button) noexcept {
	contentBox->pack_start(button, Gtk::PACK_SHRINK);
}

void BoxButton::sync() noexcept {
	auto text{data->createTooltip()};
	if (not text.empty())
		label->set_tooltip_text(text);
	label->set_text(data->createPrettyName());
}
