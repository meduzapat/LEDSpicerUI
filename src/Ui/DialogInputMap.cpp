/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.cpp
 * @since     Sep 30, 2023
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

#include "DialogInputMap.hpp"

using namespace LEDSpicerUI::Ui;

DialogInputMap* DialogInputMap::instance = nullptr;

void DialogInputMap::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance)
		builder->get_widget_derived("DialogInputMap", instance);
}

DialogInputMap* DialogInputMap::getInstance() {
	return instance;
}

DialogInputMap::DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
DialogForm(obj, builder)
{
	// Connect Input map fields.
	builder->get_widget_derived("BoxInputMap", box);
	builder->get_widget("BtnAddInputMap",      btnAdd);
	builder->get_widget("BtnApplyInputMap",    btnApply);
	setSignalAdd();
	setSignalApply();

	// Color and Filter.
	Forms::InputMap::InputMapFields inputMapFields;
	builder->get_widget("BtnInputMapDefaultColor", inputMapFields.inputMapDefaultColor);
	builder->get_widget("ComboBoxInputMapFilter",  inputMapFields.comboBoxInputMapFilter);
	// Others
	builder->get_widget("ComboBoxInputMapElement", inputMapFields.comboBoxInputMapElement);
	builder->get_widget("ComboBoxInputMapGroup",   inputMapFields.comboBoxInputMapGroup);
	builder->get_widget("InputInputMapTrigger",    inputMapFields.inputInputMapTrigger);

	Gtk::Box* inputTypeElement = nullptr;
	Gtk::Box* inputTypeGroup   = nullptr;

	builder->get_widget("StackElementAndGroup", inputMapFields.stackElementAndGroup);

	// Activate color button.
	DialogColors::getInstance()->activateColorButton(inputMapFields.inputMapDefaultColor);

	// Populate selects.
/*	signal_show().connect([inputMapFields]() {
		Forms::CollectionHandler::getInstance("elements")->refreshComboBox(inputMapFields.comboBoxInputMapElement);
		Forms::CollectionHandler::getInstance("groups")->refreshComboBox(inputMapFields.comboBoxInputMapGroup);
	});
*/
	// When the page change on the stack remove other selection.
	inputMapFields.stackElementAndGroup->connect_property_changed("visible-child", [&, inputMapFields]() {
		if (inputMapFields.stackElementAndGroup->get_visible_child_name() == "InputTypeElement") {
			Forms::CollectionHandler::getInstance("elements")->refreshComboBox(inputMapFields.comboBoxInputMapElement);
			inputMapFields.comboBoxInputMapGroup->set_active(-1);
		}
		else {
			Forms::CollectionHandler::getInstance("groups")->refreshComboBox(inputMapFields.comboBoxInputMapGroup);
			inputMapFields.comboBoxInputMapElement->set_active(-1);
		}
	});

	Forms::InputMap::initialize(inputMapFields);
}

Forms::Form* DialogInputMap::getForm(unordered_map<string, string>& rawData) {
	return new Forms::InputMap(rawData);
}

string DialogInputMap::getType() {
	return "Input Map";
}
