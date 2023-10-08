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
CollectionHandler* Element::elementHandler = nullptr;

Element::Element(unordered_map<string, string>& data) : Form(data) {
	if (not fieldsData.empty()) {
		retrieveData(Modes::LOAD);
		isValid(Modes::LOAD);
		storeData(Modes::LOAD);
		return;
	}
	resetForm(Modes::ADD);
}

Element::~Element() {
	if (fieldsData.count(NAME) and not fieldsData.at(NAME).empty())
		elementHandler->remove(fieldsData.at(NAME));
	if (pinLayout)
		unmarkMyPins();
}

void Element::initialize(ElementFields* fields) {
	Element::fields = fields;
	fields->solenoid->signal_toggled().connect([fields]() {
		fields->timeOn->set_sensitive(fields->solenoid->get_active());
		fields->timeOn->set_text("");
	});
	elementHandler = CollectionHandler::getInstance("elements");
	pinLayout      = PinHandler::getInstance();
}

void Element::resetForm(Modes mode) {
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

void Element::isValid(Modes mode) {
	// Check invalid name
	string name(fields->InputElementName->get_text());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			fields->InputElementName->grab_focus();
		throw Message("Missing element name\n");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (elementHandler->isUsed(name)) {
		if (mode != Modes::EDIT or fieldsData[NAME] != name) {
			if (mode != Modes::LOAD)
				fields->InputElementName->grab_focus();
			throw Message("Element with name " + name + " already exist\n");
		}
	}

	// Check missing type.
	/*if (fields->InputElementType->get_active_id() == "0")
		throw Message("Missing element type");*/

	std::function<void(Gtk::Entry*)> checkPin = [&](Gtk::Entry* pin) {
		string p(pin->get_text());
		string name(pin->get_placeholder_text());
		// Check empty.
		if (p.empty()) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("Enter a valid pin number for " + name + '\n');
		}

		// Numeric check and range.
		if (not Defaults::isBetween(pin->get_text(), 1, pinLayout->getSize())) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("The " + name + " must be a number from 1 and " + std::to_string(pinLayout->getSize()) + '\n');
		}
		// Check others elements for used pins.
		if (pinLayout->isUsed(p)) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("Pin " + p + " Is already in use\n");
		}

		if (pin == fields->pin)
			return;

		// Check local form for duplicated pins in RGB, ignore single pin.
		int times = 0;
		for (auto e: {fields->pinR, fields->pinG, fields->pinB})
			if (e->get_text() == p)
				++times;
		if (times > 1) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("Pin " + p + " is set more than once\n");
		}
	};
	// remove marks
	if (mode != Modes::ADD)
		unmarkMyPins();
	try {
		if (fields->pin->get_text().empty()) {
			// Check empty
			if (
				fields->pinR->get_text().empty() and
				fields->pinG->get_text().empty() and
				fields->pinB->get_text().empty()
			) {
				throw Message("Missing element connection information\n");
			}
			// RGB checks
			for (auto e: {fields->pinR, fields->pinG, fields->pinB}) {
				// check for changes.
				if (mode != Modes::EDIT or e->get_text() != fieldsData[e->get_name()])
					checkPin(e);
			}
		}
		// check single pin numeric.
		else {
			// check for changes.
			if (mode != Modes::EDIT or fields->pin->get_text() != (fieldsData.count(SOLENOID) ? fieldsData[SOLENOID] : fieldsData[PIN]))
				checkPin(fields->pin);
			// check solenoids fields.
			if (
					fields->solenoid->get_active() and
					not fields->timeOn->get_text().empty() and
					not Defaults::isNumber(fields->timeOn->get_text())
			) {
				if (mode != Modes::LOAD)
					fields->timeOn->grab_focus();
				throw Message("Enter a number for the milliseconds.\n");
			}
		}
	}
	catch (Message& e) {
		// set the pins back
		if (mode != Modes::ADD)
			markMyPins();
		throw e;
	}
	if (mode != Modes::ADD)
		markMyPins();
}

void Element::storeData(Modes mode) {

	const string name(fields->InputElementName->get_text());

	if (mode == Modes::EDIT)
		elementHandler->replace(fieldsData.at(NAME), name);
	else
		elementHandler->add(name);

	unmarkMyPins();
	// This will clean any anomaly.
	fieldsData.clear();

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
	resetForm(mode);
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
	fields->InputElementType->set_active_id(fieldsData.count(E_TYPE) ? fieldsData[E_TYPE] : DEFAULT_ELEMENT_TYPE);
}

const string Element::createPrettyName() const {
	return fieldsData.at(NAME);
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
