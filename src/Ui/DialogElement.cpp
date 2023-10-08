/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.cpp
 * @since     Mar 13, 2023
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

#include "DialogElement.hpp"

using namespace LEDSpicerUI::Ui;

DialogElement* DialogElement::instance = nullptr;

DialogElement* DialogElement::getInstance() {
	return instance;
}

void DialogElement::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance)
		builder->get_widget_derived("DialogElement", instance);
}

DialogElement::DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Element Box and buttons.
	builder->get_widget_derived("BoxElements", box, "BtnDeviceElementUp", "BtnDeviceElementDn");
	builder->get_widget("BtnAddElement",   btnAdd);
	builder->get_widget("BtnApplyElement", btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogElement::onAddClicked));
	setSignalApply();

	// Fields
	builder->get_widget("pin",  fields.pin);
	builder->get_widget("pinR", fields.pinR);
	builder->get_widget("pinG", fields.pinG);
	builder->get_widget("pinB", fields.pinB);

	builder->get_widget("InputElementAutoOff", fields.solenoid);
	builder->get_widget("InputElementOnTime",  fields.timeOn);

	builder->get_widget("InputElementName",  fields.InputElementName);
	builder->get_widget("InputDefaultColor", fields.InputDefaultColor);
	builder->get_widget("InputElementType",  fields.InputElementType);

	DialogColors::getInstance()->activateColorButton(fields.InputDefaultColor);

	Forms::Element::initialize(&fields);

	// Name Generator.
	Gtk::Dialog* dialogGenerateElementName = nullptr;
	Gtk::Button* btnGenerateElementName    = nullptr;
	Gtk::ComboBoxText
		* ComboBoxEN1 = nullptr,
		* ComboBoxEN2 = nullptr,
		* ComboBoxEN3 = nullptr,
		* ComboBoxEN4 = nullptr;
	builder->get_widget("DialogGenerateElementName", dialogGenerateElementName);
	builder->get_widget("BtnGenerateElementName",    btnGenerateElementName);
	builder->get_widget("ComboBoxEN1", ComboBoxEN1);
	builder->get_widget("ComboBoxEN2", ComboBoxEN2);
	builder->get_widget("ComboBoxEN3", ComboBoxEN3);
	builder->get_widget("ComboBoxEN4", ComboBoxEN4);

	ComboBoxEN1->signal_changed().connect([=]() {
		ComboBoxEN2->set_sensitive(not ComboBoxEN1->get_active_text().empty());
	});
	ComboBoxEN2->signal_changed().connect([=]() {
		ComboBoxEN3->set_sensitive(not ComboBoxEN2->get_active_text().empty());
	});
	ComboBoxEN3->signal_changed().connect([=]() {
		ComboBoxEN4->set_sensitive(not ComboBoxEN3->get_active_text().empty());
	});

	btnGenerateElementName->signal_clicked().connect([=]() {
		ComboBoxEN1->set_active(-1);
		ComboBoxEN2->set_active(-1);
		ComboBoxEN3->set_active(-1);
		ComboBoxEN4->set_active(-1);
		ComboBoxEN2->set_sensitive(false);
		ComboBoxEN3->set_sensitive(false);
		ComboBoxEN4->set_sensitive(false);
		if (dialogGenerateElementName->run() == Gtk::ResponseType::RESPONSE_APPLY) {
			string name(ComboBoxEN1->get_active_id() + ComboBoxEN2->get_active_id());
			if (not name.empty()) {
				if (not ComboBoxEN3->get_active_id().empty())
					name +=  '_' + ComboBoxEN3->get_active_id() + ComboBoxEN4->get_active_id();
				fields.InputElementName->set_text(name);
				fields.InputElementType->set_active_id(Defaults::detectElementType(name));
				fields.InputElementName->grab_focus();
			}
		}
		dialogGenerateElementName->hide();
	});

	// When the notebook page changes, remove the values on the other page.
	Gtk::Notebook* notebookDevice;
	builder->get_widget("NotebookDevice", notebookDevice);
	notebookDevice->signal_switch_page().connect([&](Gtk::Widget* page, guint pageNum) {
		if (pageNum) {
			fields.pinR->set_text("");
			fields.pinG->set_text("");
			fields.pinB->set_text("");
		}
		else {
			fields.pin->set_text("");
			fields.solenoid->set_active(true);
			fields.solenoid->set_active(false);
		}
	});

	// When show up reset the notebook to RGB.
	/*
	notebookDevice->signal_show().connect([&, notebookDevice]() {
		if (not editMode)
			notebookDevice->set_current_page(0);
	});
	*/

	signal_show().connect([&, notebookDevice]() {
		// PinHandler is not accepting signal show for some reason do reset here.
		Forms::PinHandler::getInstance()->reset();
		if (mode == Forms::Form::Modes::EDIT) {
			// If led or solenoid is set, set page 1, 0 for RGB.
			notebookDevice->set_current_page(not currentForm->getValue(PIN).empty() or not currentForm->getValue(SOLENOID).empty());
		}
	});
}

string DialogElement::toXml() {
	Defaults::increaseTab();
	string r;
	for (auto b : items) {
		r += b->toXML("");
	}
	Defaults::reduceTab();
	return r;
}

string DialogElement::getType() {
	return "element";
}

Forms::Form* DialogElement::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Element(rawData);
}
