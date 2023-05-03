/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.cpp
 * @since     Apr 30, 2023
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

#include "DialogRestrictor.hpp"

using namespace LEDSpicerUI::Ui;

DialogRestrictor::DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Restrictor Box and buttons.
	builder->get_widget_derived("BoxRestrictors", box);
	builder->get_widget("BtnAddRestrictor",       btnAdd);
	builder->get_widget("BtnApplyRestrictors",    btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogRestrictor::onAddClicked));
	setSignalApply();

	// Restrictor fields and models..
	builder->get_widget("ComboBoxRestrictors",  fields.comboBoxRestrictors);
	builder->get_widget("ComboBoxRestrictorId", fields.comboBoxId);
	builder->get_widget("inputPlayer",          fields.player);
	builder->get_widget("InputJoystick",        fields.joystick);
	builder->get_widget("InputHasRestrictor",   fields.hasRestrictor);
	builder->get_widget("InputHandleMouse",     fields.handleMouse);
	builder->get_widget("InputWilliamsMode",    fields.williamsMode);
	builder->get_widget("InputSpeedOn",         fields.speedOn);
	builder->get_widget("InputSpeedOff",        fields.speedOff);
	fields.restrictorsListstore = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictors").get());
	fields.idListstore          = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictorsId").get());
	Forms::Restrictor::initialize(&fields);

	// When the dialog shows, do some clean up for new restrictors.
	signal_show().connect([&]() {
		if (mode == Forms::Form::Modes::ADD) {
			fields.comboBoxRestrictors->set_sensitive(true);
			fields.comboBoxRestrictors->set_active_id("");
			btnApply->set_sensitive(false);
			fields.comboBoxId->set_sensitive(false);
			fields.player->set_sensitive(false);
			fields.joystick->set_sensitive(false);
		}
		else {
			fields.comboBoxRestrictors->set_sensitive(false);
			btnApply->set_sensitive(true);
			fields.comboBoxId->set_sensitive(true);
			fields.player->set_sensitive(true);
			fields.joystick->set_sensitive(true);
		}
	});

	fields.comboBoxRestrictors->signal_changed().connect([&]() {
		// Setup only, edit or load.
		if (mode != Forms::Form::Modes::ADD)
			return;
		// Add.
		if (currentForm) {
			delete currentForm;
			currentForm = nullptr;
		}
		if (fields.comboBoxRestrictors->get_active_id().empty()) {
			btnApply->set_sensitive(false);
		}
		else {
			btnApply->set_sensitive(true);
			unordered_map<string, string> rawData;
			currentForm = getForm(rawData);
		}
	});
}

Forms::Form* DialogRestrictor::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Restrictor(rawData);
}

string DialogRestrictor::getType() {
	return "restrictor";
}
