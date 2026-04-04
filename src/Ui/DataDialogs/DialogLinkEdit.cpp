/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogLinkEdit.cpp
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

#include "DialogLinkEdit.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogLinkEdit::DialogLinkEdit(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	GladeDialog(obj, builder)
{
	builder->get_widget("BtnApplyLinkEdit",   btnApply);
	builder->get_widget("BoxLinkEditColor",   boxColor);
	builder->get_widget("LabelLinkEditColor", labelColor);
	builder->get_widget("BtnLinkEditColor",   btnColor);
	builder->get_widget("BoxLinkEditCombo",   boxCombo);
	builder->get_widget("LabelLinkEditCombo", labelCombo);
	builder->get_widget("ComboLinkEditCombo", comboFilter);

	/* Wire the color button once — the same button is reused across all opens. */
	DialogColors::getInstance()->activateColorButton(btnColor);

//	btnApply->signal_clicked().connect([this]() {
//		response(Gtk::ResponseType::RESPONSE_APPLY);
//	});
}

void DialogLinkEdit::open(Storage::Link* link, const vector<LinkField>& fields) {

	/* Hide all slots — show only what this link type needs. */
	boxColor->hide();
	boxCombo->hide();

	for (const auto& field : fields) {
		switch (field.widgetType) {

			case LinkField::Widget::COLOR:
				labelColor->set_text(field.label);
				DialogColors::getInstance()->colorizeButton(
					btnColor,
					link->getValue(field.key, field.defaultValue)
				);
				boxColor->show();
				break;

			case LinkField::Widget::COMBO:
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

	/* Write widget values back into the Link's fieldsData. */
	for (const auto& field : fields) {
		switch (field.widgetType) {
			case LinkField::Widget::COLOR:
				link->setValue(field.key, string(btnColor->get_label()));
				break;
			case LinkField::Widget::COMBO:
				link->setValue(field.key, string(comboFilter->get_active_id()));
				break;
		}
	}

	Defaults::markDirty();
	hide();
}
