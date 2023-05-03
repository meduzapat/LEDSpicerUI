/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroupBoxButton.cpp
 * @since     Feb 13, 2023
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

#include "BoxButton.hpp"

using namespace LEDSpicerUI::Ui::Forms;

BoxButton::BoxButton(
	string type,
	Form* form
) : type(type),
	form(form),
	label(Gtk::make_managed<Gtk::Label>()),
	btnE(form->canEdit()   ? Gtk::make_managed<Gtk::Button>() : nullptr),
	btnD(form->canDelete() ? Gtk::make_managed<Gtk::Button>() : nullptr),
	Gtk::HBox(false, 2)
{
	set_valign(Gtk::ALIGN_START);
	set_vexpand(false);
	set_margin_top(2);
	set_margin_bottom(2);
	set_margin_left(2);
	set_margin_right(2);

	pack_start(*label, Gtk::PACK_EXPAND_WIDGET);

	if (form->canEdit()) {
		btnE->set_image_from_icon_name("applications-engineering", Gtk::ICON_SIZE_BUTTON);
		pack_start(*btnE, Gtk::PACK_SHRINK);
	}

	if (form->canDelete()) {
		btnD->set_image_from_icon_name("edit-delete", Gtk::ICON_SIZE_BUTTON);
		pack_start(*btnD, Gtk::PACK_SHRINK);
	}
	get_style_context()->add_class("BoxButton");
	if (not form->getCssClass().empty())
		get_style_context()->add_class(form->getCssClass());
	updateLabel();
	show_all();
}

BoxButton::~BoxButton() {
	if (form) {
		delete form;
		form = nullptr;
	}
}

void BoxButton::setEditFn(std::function<void()> ediCallback) {
	btnE->signal_clicked().connect(ediCallback);
}

void BoxButton::setDelFn(std::function<void()> delCallback) {
	btnD->signal_clicked().connect(delCallback);
}

string BoxButton::toXML(const string& contents) const {
	string r("<" + type + " " + form->toXML());
	if (contents.empty())
		r +=  "/>";
	else
		r += ">" + contents + "</" + type + ">";
	return r;
}

Form* BoxButton::getForm() {
	return form;
}

const Form* BoxButton::getForm() const {
	return form;
}

void BoxButton::updateLabel() {
	label->set_text(form->createPrettyName());
}
