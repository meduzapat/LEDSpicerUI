/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogLinkEditor.cpp
 * @since     Apr 2026
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

#include "DialogLinkEditor.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogLinkEditor::DialogLinkEditor(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	GladeDialog(obj, builder)
{

	add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	btnApply = add_button("_Apply", Gtk::ResponseType::RESPONSE_APPLY);

	builder->get_widget("BoxLinkEditColor",   boxColor);
	builder->get_widget("LabelLinkEditColor", labelColor);
	builder->get_widget("BtnLinkEditColor",   btnColor);
	builder->get_widget("BoxLinkEditCombo",   boxCombo);
	builder->get_widget("LabelLinkEditCombo", labelCombo);
	builder->get_widget("ComboLinkEditCombo", comboFilter);

	DialogColors::getInstance()->activateColorButton(btnColor);
}

void DialogLinkEditor::open(Storage::Link* link) noexcept {

	boxColor->hide();
	boxCombo->hide();

	// Prepare
	for (const auto& field : link->getLinkFields()) {
		switch (field.widgetType) {
		case Storage::Link::LinkField::Widget::COLOR_PICKER:
			labelColor->set_text(field.label);
			DialogColors::getInstance()->colorizeButton(
				btnColor,
				link->getValue(field.key, field.defaultValue)
			);
			boxColor->show();
			break;

		case Storage::Link::LinkField::Widget::COMBOBOX:
			labelCombo->set_text(field.label);
			comboFilter->set_active_id(link->getValue(field.key, field.defaultValue));
			boxCombo->show();
			break;
		}
	}

	if (run() != Gtk::ResponseType::RESPONSE_APPLY) {
		hide();
		return;
	}

	for (const auto& field : link->getLinkFields()) {
		switch (field.widgetType) {
			case Storage::Link::LinkField::Widget::COLOR_PICKER:
				link->setValue(field.key, btnColor->get_label());
				break;
			case Storage::Link::LinkField::Widget::COMBOBOX:
				link->setValue(field.key, comboFilter->get_active_id());
				break;
		}
	}

	Defaults::markDirty();
	hide();
}
