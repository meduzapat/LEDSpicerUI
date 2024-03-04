/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.cpp
 * @since     Mar 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogElement* DialogElement::instance = nullptr;

void DialogElement::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogElement", instance);
	}
}

DialogElement* DialogElement::getInstance() {
	return instance;
}

DialogElement::DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Element Box and buttons.
	builder->get_widget_derived("BoxElements", box, "BtnDeviceElementUp", "BtnDeviceElementDn");
	builder->get_widget("BtnAddElement",       btnAdd);
	builder->get_widget("BtnApplyElement",     btnApply);
	setSignalAdd();
	setSignalApply();

	// Pins layout.
	builder->get_widget("FlowBoxPinLayout", pinsBox);

	// Fields.
	builder->get_widget("pin",  pin);
	builder->get_widget("pinR", pinR);
	builder->get_widget("pinG", pinG);
	builder->get_widget("pinB", pinB);

	builder->get_widget("InputElementAutoOff", solenoid);
	builder->get_widget("InputElementOnTime",  timeOn);

	builder->get_widget("InputElementName",  inputElementName);
	builder->get_widget("InputDefaultColor", inputDefaultColor);
	builder->get_widget("InputElementType",  inputElementType);

	builder->get_widget("NotebookDevice", notebookDevice);

	DialogColors::getInstance()->activateColorButton(inputDefaultColor);

	solenoid->signal_toggled().connect([&]() {
		timeOn->set_sensitive(solenoid->get_active());
		timeOn->set_text("");
	});

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

	ComboBoxEN1->signal_changed().connect([ComboBoxEN1, ComboBoxEN2]() {
		ComboBoxEN2->set_sensitive(not ComboBoxEN1->get_active_text().empty());
	});
	ComboBoxEN2->signal_changed().connect([ComboBoxEN2, ComboBoxEN3]() {
		ComboBoxEN3->set_sensitive(not ComboBoxEN2->get_active_text().empty());
	});
	ComboBoxEN3->signal_changed().connect([ComboBoxEN3, ComboBoxEN4]() {
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
				inputElementName->set_text(name);
				inputElementType->set_active_id(Defaults::detectElementType(name));
				inputElementName->grab_focus();
			}
		}
		dialogGenerateElementName->hide();
	});

	// When the notebook page changes, remove the values on the other page.
	notebookDevice->signal_switch_page().connect([&](Gtk::Widget* page, guint pageNum) {
		if (pageNum) {
			pinR->set_text("");
			pinG->set_text("");
			pinB->set_text("");
		}
		else {
			pin->set_text("");
			solenoid->set_active(true);
			solenoid->set_active(false);
		}
	});
}

void DialogElement::load(XMLHelper* values) {
	createItems(values->getData(Defaults::createCommonUniqueId({owner->createUniqueId(), COLLECTION_ELEMENT})), values);
}

void DialogElement::clearForm() {
	pin->set_text("");
	pinR->set_text("");
	pinG->set_text("");
	pinB->set_text("");
	timeOn->set_text("");
	solenoid->set_active(false);
	timeOn->set_sensitive(false);
	inputElementName->set_text("");
	inputElementName->grab_focus();
	inputElementType->set_active_id("0");
	DialogColors::getInstance()->colorizeButton(inputDefaultColor, NO_COLOR);
	drawPins();
}

void DialogElement::isValid() const {
	// Check invalid name
	string name(createUniqueId());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			inputElementName->grab_focus();
		throw Message("Missing element name");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (elementHandler->isUsed(name)) {
		if (mode != Modes::EDIT or currentData->createUniqueId() != name) {
			if (mode != Modes::LOAD)
				inputElementName->grab_focus();
			throw Message("Element with name " + name + " already exist");
		}
	}

	// Check missing type.
	/*if (inputElementType->get_active_id() == "0")
		throw Message("Missing element type");*/


	// Check for changes against old values.
	std::function<bool(Gtk::Entry*)> checkOld = [&](Gtk::Entry* pin) {
		for (auto& old: {currentData->getValue(PIN), currentData->getValue(SOLENOID), currentData->getValue(RED_PIN), currentData->getValue(GREEN_PIN), currentData->getValue(BLUE_PIN)}) {
			if (not old.empty() and old == pin->get_text()) {
				return true;
			}
		}
		return false;
	};

	// Check for pin errors.
	std::function<void(Gtk::Entry*)> checkPin = [&](Gtk::Entry* pin) {
		string p(pin->get_text());
		string name(pin->get_placeholder_text());
		// Check empty.
		if (p.empty()) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("Enter a valid pin number for " + name);
		}

		// Numeric check and range.
		if (not Defaults::isBetween(p, 1, numberOfPins)) {
			if (mode != Modes::LOAD)
				pin->grab_focus();
			throw Message("The " + name + " must be a number from 1 and " + std::to_string(numberOfPins));
		}
	};

	if (pin->get_text().empty()) {
		// Check empty
		if (
			pinR->get_text().empty() and
			pinG->get_text().empty() and
			pinB->get_text().empty()
		) {
			throw Message("Missing element pin out information");
		}
		// RGB checks
		for (auto e: {pinR, pinG, pinB}) {
			// Check local form for duplicated pins in RGB.
			bool found = false;
			for (auto p: {pinR, pinG, pinB}) {
				if (e->get_text() == p->get_text()) {
					if (found) {
						if (mode != Modes::LOAD)
							pin->grab_focus();
						throw Message("Pin " + p->get_text() + " is set more than once");
					}
					found = true;
				}
			}
			found = false;
			// if edit, check if the value changed or not.
			if (mode == Modes::EDIT) {
				found = checkOld(e);
			}
			// Value is new or changed.
			if (not found) {
				checkPin(e);
			}
		}
	}
	else {
		// Check for changes, solenoid or pin.
		if (mode != Modes::EDIT or not checkOld(pin)) {
			checkPin(pin);
		}

		// Check solenoid field.
		if (
			solenoid->get_active() and
			not timeOn->get_text().empty() and
			not Defaults::isNumber(timeOn->get_text())
		) {
			if (mode != Modes::LOAD)
				timeOn->grab_focus();
			throw Message("Enter a number for the milliseconds.");
		}
	}
}

void DialogElement::storeData() {

	const string name(createUniqueId());

	if (mode == Modes::EDIT)
		elementHandler->replace(currentData->createUniqueId(), name);
	else
		elementHandler->add(name);

	// This will clean any anomaly.
	currentData->wipe();

	currentData->setValue(NAME, name);

	// Single pins.
	if (not pin->get_text().empty()) {
		// Solenoid.
		if (solenoid->get_active()) {
			currentData->setValue(SOLENOID, pin->get_text());
			if (not timeOn->get_text().empty()) {
				currentData->setValue(TIME_ON, timeOn->get_text());
			}
		}
		// LED.
		else {
			currentData->setValue(PIN, pin->get_text());
		}
	}
	// RGB.
	else {
		currentData->setValue(RED_PIN, pinR->get_text());
		currentData->setValue(GREEN_PIN, pinG->get_text());
		currentData->setValue(BLUE_PIN, pinB->get_text());
	}
	if (not inputDefaultColor->get_tooltip_text().empty()) {
		currentData->setValue(DEFAULT_COLOR, inputDefaultColor->get_tooltip_text());
	}
	currentData->setValue(TYPE, inputElementType->get_active_id() == "0" ? DEFAULT_ELEMENT_TYPE : inputElementType->get_active_id());
}

void DialogElement::retrieveData() {
	inputElementName->set_text(currentData->getValue(NAME));
	if (not currentData->getValue(PIN).empty()) {
		pin->set_text(currentData->getValue(PIN));
		solenoid->set_active(false);
	}
	else if (not currentData->getValue(SOLENOID).empty()) {
		pin->set_text(currentData->getValue(SOLENOID));
		solenoid->set_active(true);
		if (not currentData->getValue(TIME_ON).empty())
			timeOn->set_text(currentData->getValue(TIME_ON));
	}
	else {
		pinR->set_text(currentData->getValue(RED_PIN));
		pinG->set_text(currentData->getValue(GREEN_PIN));
		pinB->set_text(currentData->getValue(BLUE_PIN));
	}
	DialogColors::getInstance()->colorizeButton(
		inputDefaultColor,
		currentData->getValue(DEFAULT_COLOR, NO_COLOR)
	);
	inputElementType->set_active_id(currentData->getValue(TYPE, DEFAULT_ELEMENT_TYPE));
	// If led or solenoid is set, set page 1, 0 for RGB.
	notebookDevice->set_current_page(not currentData->getValue(PIN).empty() or not currentData->getValue(SOLENOID).empty());
}

string const DialogElement::createUniqueId() const {
	return Defaults::createCommonUniqueId({inputElementName->get_text()});
}

void DialogElement::changeNumberOfPins(const uint8_t newSize) {

	if (not newSize)
		numberOfPins = 0;

	if (numberOfPins == newSize)
		return;

	// resize.
	if (newSize < numberOfPins) {
		uint8_t pinsToSearch(numberOfPins - newSize);
		vector<Storage::BoxButton*> elementsToDelete;
		for (uint8_t c = 1; c <= pinsToSearch; ++c)
			findElementsByPin(std::to_string(c), elementsToDelete);
		// delete elements
		string deleted;
		for(auto boxButton : elementsToDelete) {
			deleted += boxButton->getData()->getValue(NAME) + " ";
			// elements doesn't need activation.
			box->remove(*boxButton);
			items->remove(boxButton);
		}
		if (not deleted.empty())
			Message::displayInfo("Element(s) " + deleted + "have been deleted due to resizing.");
	}
	pinsBox->set_max_children_per_line(findLargestDivisor(newSize));
	numberOfPins = newSize;
	drawPins();
}

void DialogElement::drawPins() {

	for (auto& child : pinsBox->get_children())
		pinsBox->remove(*child);

	// Fill pins.
	for (auto c = 1; c <= numberOfPins; ++c) {
		auto label = Gtk::make_managed<Gtk::Label>(std::to_string(c));
		vector<Storage::BoxButton*> elementsUse;
		findElementsByPin(std::to_string(c), elementsUse);
		string labelTxt("Pin " + std::to_string(c) + " is used by element ");
		vector<string> elementsTxt;
		for (auto bb : elementsUse)
			elementsTxt.push_back(bb->getData()->getValue(NAME));
		if (elementsUse.size() > 1) {
			label->get_style_context()->add_class(COLOR_MULTIPLE);
			std::string lastElement = std::move(elementsTxt.back());
			elementsTxt.pop_back();
			label->set_tooltip_text(labelTxt + Defaults::implode(elementsTxt, ", ") + " and " + lastElement);
		}
		else if (elementsUse.size()) {
			label->get_style_context()->add_class(Storage::Element::getPinCssByData(c, elementsUse[0]->getData()));
			label->set_tooltip_text(labelTxt + elementsTxt[0]);
		}
		else {
			label->get_style_context()->add_class(NO_COLOR);
			label->set_tooltip_text("Pin " + std::to_string(c) + " is not used");
		}
		pinsBox->add(*label);
	}
	pinsBox->show_all();
}

const string DialogElement::getType() const {
	return "element";
}

LEDSpicerUI::Ui::Storage::Data* DialogElement::getData(unordered_map<string, string>& rawData) {
	return new Storage::Element(rawData);
}

void DialogElement::addButtons(Storage::BoxButton* boxButton) {
	createEditButton(boxButton);
	createCloneButton(boxButton);
	createDeleteButton(boxButton);
	boxButton->show_all();
}

void DialogElement::findElementsByPin(const string& pin, vector<Storage::BoxButton*>& elements) {
	for (auto boxButton : *items) {
		for (auto v : {PIN, SOLENOID, RED_PIN, GREEN_PIN, BLUE_PIN}) {
			if (boxButton->getData()->getValue(v) == pin) {
				// already in the list?
				if (std::find(elements.begin(), elements.end(), boxButton) != elements.end())
					break;
				elements.push_back(boxButton);
				break;
			}
		}
	}
}

const uint8_t DialogElement::findLargestDivisor(uint8_t size) {
	for (uint8_t c = MAX_COLUMNS; c > MIN_COLUMNS; --c) {
		if (size % c == 0) {
			return c;
		}
	}
	return MAX_COLUMNS;
}
