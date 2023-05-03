/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.cpp
 * @since     Mar 16, 2023
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

#include "Element.hpp"

using namespace LEDSpicerUI::Ui::Forms;

PinHandler* Element::pinLayout             = nullptr;
Element::ElementFields* Element::fields    = nullptr;
CollectionHandler* Element::ElementHandler = nullptr;

Element::Element(unordered_map<string, string>& data) : Form(data) {
	if (not fieldsData.empty()) {
		retrieveData(Forms::Form::Modes::LOAD);
		isValid(Forms::Form::Modes::LOAD);
		storeData(Forms::Form::Modes::LOAD);
		ElementHandler->add(fieldsData[NAME]);
		return;
	}
	cancelData(Forms::Form::Modes::ADD);
}

Element::~Element() {
	if (fieldsData.count(NAME) and not fieldsData.at(NAME).empty())
		ElementHandler->remove(fieldsData.at(NAME));
	if (pinLayout)
		unmarkMyPins();
}

void Element::initialize(ElementFields* fields) {
	Element::fields = fields;
	fields->solenoid->signal_toggled().connect([fields]() {
		fields->timeOn->set_sensitive(fields->solenoid->get_active());
		fields->timeOn->set_text("");
	});
	ElementHandler = CollectionHandler::getInstance("elements");
	pinLayout      = PinHandler::getInstance();
}

const string Element::createPrettyName() const {
	return fieldsData.at(NAME);
}

void Element::isValid(Modes mode) {
	// Check invalid name
	if (fields->InputElementName->get_text().empty()) {
		fields->InputElementName->grab_focus();
		throw Message("Missing element name");
	}

	auto name = fields->InputElementName->get_text();

	// Check element dupes.
	if (
		(mode == Forms::Form::Modes::EDIT and name != fieldsData.at(NAME) and ElementHandler->isUsed(name)) or
		(mode != Forms::Form::Modes::EDIT and ElementHandler->isUsed(name))
	) {
		fields->InputElementName->grab_focus();
		throw Message("Element with name " + name + " already exist");
	}

	// Check missing type.
	/*if (fields->InputElementType->get_active_id() == "0")
		throw Message("Missing element type");*/

	std::function<void(Gtk::Entry*)> checkPin = [&](Gtk::Entry* pin) {
		// Check empty.
		if (pin->get_text().empty()) {
			pin->grab_focus();
			throw Message("Enter a valid pin number for " + pin->get_placeholder_text());
		}

		// Numeric check and range.
		if (not Message::isBetween(pin->get_text(), 1, pinLayout->getSize())) {
			pin->grab_focus();
			throw Message("The " + pin->get_placeholder_text() + " must be a number from 1 and " + std::to_string(pinLayout->getSize()));
		}
		// Check others elements for used pins.
		if (pinLayout->isUsed(pin->get_text())) {
			pin->grab_focus();
			throw Message("Pin " + pin->get_text() + " Is already in use");
		}

		if (pin == fields->pin)
			return;

		// Check local form for duplicated pins in RGB, ignore single pin.
		int times = 0;
		for (auto e: {fields->pinR, fields->pinG, fields->pinB})
			if (e->get_text() == pin->get_text())
				++times;
		if (times > 1) {
			pin->grab_focus();
			throw Message("Pin " + pin->get_text() + " is set more than once ");
		}
	};
	// remove marks
	if (mode == Forms::Form::Modes::EDIT)
		unmarkMyPins();
	try {
		if (fields->pin->get_text().empty()) {
			// Check empty
			if (
				fields->pinR->get_text().empty() and
				fields->pinG->get_text().empty() and
				fields->pinB->get_text().empty()
			) {
				throw Message("Missing element pin information");
			}
			// RGB checks
			for (auto e: {fields->pinR, fields->pinG, fields->pinB}) {
				// check pin if changed (the field name is stored  in the entry name).
				if (e->get_text() != fieldsData[e->get_name()])
					checkPin(e);
			}
		}
		// check single pin numeric.
		else {
			// check for no changes.
			if (fields->pin->get_text() != (fieldsData.count(SOLENOID) ? fieldsData[SOLENOID] : fieldsData[PIN]))
				checkPin(fields->pin);
			// check solenoids fields.
			if (
					fields->solenoid->get_active() and
					not fields->timeOn->get_text().empty() and
					not Message::isBetween(fields->timeOn->get_text(), 1, -1)
			) {
				fields->timeOn->grab_focus();
				throw Message("Enter a number bigger than 0 for the milliseconds.");
			}
		}
	}
	catch (Message& e) {
		// set the pins back
		if (mode == Forms::Form::Modes::EDIT)
			markMyPins();
		throw e;
	}
	if (mode == Forms::Form::Modes::EDIT)
		markMyPins();
}

void Element::storeData(Modes mode) {

	const auto& name = fields->InputElementName->get_text();
	// Add.
	if (mode == Forms::Form::Modes::ADD) {
		ElementHandler->add(name);
	}
	// Edit.
	else if (mode == Forms::Form::Modes::EDIT) {
		if (fieldsData[NAME] != name)
			ElementHandler->replace(fieldsData.at(NAME), name);
		// Clean up.
		unmarkMyPins();
		fieldsData.clear();
	}

	fieldsData[NAME] = name;

	if (not fields->pin->get_text().empty()) {
		if (fields->solenoid->get_active()) {
			fieldsData[SOLENOID] = fields->pin->get_text();
			if (not fields->timeOn->get_text().empty()) {
				fieldsData[TIME_ON] = fields->timeOn->get_text();
			}
		}
		else {
			fieldsData[PIN] = fields->pin->get_text();
		}
	}
	else {
		fieldsData[RED_PIN]   = fields->pinR->get_text();
		fieldsData[GREEN_PIN] = fields->pinG->get_text();
		fieldsData[BLUE_PIN]  = fields->pinB->get_text();
	}
	if (not fields->InputDefaultColor->get_tooltip_text().empty()) {
		fieldsData[DEFAULT_COLOR] = fields->InputDefaultColor->get_tooltip_text();
	}
	fieldsData[E_TYPE] = fields->InputElementType->get_active_id() == "0" ? DEFAULT_ELEMENT_TYPE : fields->InputElementType->get_active_id();
	markMyPins();
	cancelData(mode);
}

void Element::retrieveData(Modes mode) {
	fields->InputElementName->set_text(fieldsData[NAME]);
	if (fieldsData.count(PIN) and not fieldsData.at(PIN).empty()) {
		fields->pin->set_text(fieldsData[PIN]);
		fields->solenoid->set_active(false);
	}
	else if (fieldsData.count(SOLENOID) and not fieldsData.at(SOLENOID).empty()) {
		fields->pin->set_text(fieldsData[SOLENOID]);
		fields->solenoid->set_active(true);
		if (fieldsData.count(TIME_ON) and not fieldsData.at(TIME_ON).empty())
			fields->timeOn->set_text(fieldsData[TIME_ON]);
	}
	else {
		fields->pinR->set_text(fieldsData[RED_PIN]);
		fields->pinG->set_text(fieldsData[GREEN_PIN]);
		fields->pinB->set_text(fieldsData[BLUE_PIN]);
	}
	DialogColors::getInstance()->colorizeButton(
		fields->InputDefaultColor,
		fieldsData.count(DEFAULT_COLOR) ? fieldsData[DEFAULT_COLOR] : NO_COLOR
	);
	fields->InputElementType->set_active_id(fieldsData[E_TYPE]);
}

void Element::cancelData(Modes mode) {
	// if in the future cancelData is needed, this will be moved.
	fields->pin->set_text("");
	fields->pinR->set_text("");
	fields->pinG->set_text("");
	fields->pinB->set_text("");
	fields->timeOn->set_text("");
	fields->solenoid->set_active(false);
	fields->timeOn->set_sensitive(false);
	fields->InputElementName->set_text("");
	fields->InputElementName->grab_focus();
	fields->InputElementType->set_active_id("0");
	DialogColors::getInstance()->colorizeButton(fields->InputDefaultColor, NO_COLOR);
}

const string Element::getCssClass() const {
	return "ElementBoxButton";
}

void Element::unmarkMyPins() {
	if (fieldsData.count(PIN) and not fieldsData.at(PIN).empty())
		pinLayout->unmarkPin(fieldsData.at(PIN));
	if (fieldsData.count(SOLENOID) and not fieldsData.at(SOLENOID).empty())
		pinLayout->unmarkPin(fieldsData.at(SOLENOID));
	if (fieldsData.count(RED_PIN) and not fieldsData.at(RED_PIN).empty())
		pinLayout->unmarkPin(fieldsData.at(RED_PIN));
	if (fieldsData.count(GREEN_PIN) and not fieldsData.at(GREEN_PIN).empty())
		pinLayout->unmarkPin(fieldsData.at(GREEN_PIN));
	if (fieldsData.count(BLUE_PIN) and not fieldsData.at(BLUE_PIN).empty())
		pinLayout->unmarkPin(fieldsData.at(BLUE_PIN));
}

void Element::markMyPins() {
	if (fieldsData.count(PIN) and not fieldsData.at(PIN).empty()) {
		pinLayout->markPin(fieldsData[PIN], PIN_COLOR);
	}
	else if (fieldsData.count(SOLENOID) and not fieldsData.at(SOLENOID).empty()) {
		pinLayout->markPin(fieldsData[SOLENOID], SOLENOID_COLOR);
	}
	else {
		pinLayout->markPin(fieldsData[RED_PIN],   RED_COLOR);
		pinLayout->markPin(fieldsData[GREEN_PIN], GREEN_COLOR);
		pinLayout->markPin(fieldsData[BLUE_PIN],  BLUE_COLOR);
	}
}
