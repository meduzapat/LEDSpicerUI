/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroupDialog.cpp
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

#include "DialogGroup.hpp"

using namespace LEDSpicerUI::Ui;

DialogGroup::DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	// Connect Groups Box and button.
	builder->get_widget_derived("BoxGroups", box);
	builder->get_widget("BtnAddGroup",       btnAdd);
	builder->get_widget("BtnApplyGroup",     btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogGroup::onAddClicked));
	setSignalApply();

	Forms::Group::GroupFields groupFields;
	builder->get_widget("InputGroupName",       groupFields.InputGroupName);
	builder->get_widget("BtnGroupDefaultColor", groupFields.BtnGroupDefaultColor);
	DialogColors::getInstance()->activateColorButton(groupFields.BtnGroupDefaultColor);

	// Name Generator.
	Gtk::Dialog* dialogGenerateGroupName = nullptr;
	Gtk::Button* btnGenerateGroupName    = nullptr;
	Gtk::ComboBoxText
		* ComboBoxGN1 = nullptr,
		* ComboBoxGN2 = nullptr;
	builder->get_widget("DialogGenerateGroupName", dialogGenerateGroupName);
	builder->get_widget("BtnGenerateGroupName",    btnGenerateGroupName);
	builder->get_widget("ComboBoxGN1", ComboBoxGN1);
	builder->get_widget("ComboBoxGN2", ComboBoxGN2);

	btnGenerateGroupName->signal_clicked().connect([=]() {
		ComboBoxGN1->set_active(-1);
		ComboBoxGN2->set_active(-1);
		if (dialogGenerateGroupName->run() == Gtk::ResponseType::RESPONSE_APPLY) {
			groupFields.InputGroupName->set_text(ComboBoxGN1->get_active_id() + ComboBoxGN2->get_active_id());
			groupFields.InputGroupName->grab_focus();
		}
		dialogGenerateGroupName->hide();
	});

	Forms::Group::initialize(groupFields);

	signal_show().connect([&]() {
		// Create group for add.
		if (mode == Forms::Form::Modes::ADD) {
			// create an empty group.
			unordered_map<string, string> rawData;
			currentForm = getForm(rawData);
		}
	});
}

string DialogGroup::getType() {
	return "group";
}

Forms::Form* DialogGroup::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Group(rawData);
}
