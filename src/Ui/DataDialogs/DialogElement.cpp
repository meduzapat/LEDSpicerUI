/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.cpp
 * @since     Mar 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

	// Connections layout.
	builder->get_widget("FlowBoxPinLayout", pinsBox);

	// Scattered RGB.
	builder->get_widget("EntryPinR", pinR);
	builder->get_widget("EntryPinG", pinG);
	builder->get_widget("EntryPinB", pinB);

	// RGB.
	builder->get_widget("ComboBoxRGBRGB",   comboBoxRGBRGB);
	builder->get_widget("EntryPositionRGB", positionRGB);

	// RGB Strip.
	builder->get_widget("ComboBoxRGBStrip",   comboBoxRGBStrip);
	builder->get_widget("EntrySizeStrip",     sizeStrip);
	builder->get_widget("EntryPositionStrip", positionStrip);

	// Single Pin
	builder->get_widget("EntryPin",              pin);
	builder->get_widget("ToggleElementSolenoid", solenoid);
	builder->get_widget("EntryElementOnTime",    timeOn);

	// Multi RGB.
	builder->get_widget("EntryPositionsMRGB", positionsMRGB);
	builder->get_widget("ComboBoxRGBMRGB",    comboBoxRGBMRGB);

	// General.
	builder->get_widget("EntryElementName",       elementName);
	builder->get_widget("BtnDefaultColor",        btnDefaultColor);
	builder->get_widget("ComboBoxElementType",    elementType);
	builder->get_widget("ScaleElementBrightness", brightness);

	builder->get_widget("NotebookDeviceConnections", notebookDeviceConnections);

	DialogColors::getInstance()->activateColorButton(btnDefaultColor);

	pinsBox->signal_selected_children_changed().connect([&]() {

		auto selected(pinsBox->get_selected_children());
		// unselect.
		if (selected.empty()) {
			return;
		}

		// Extracts the position number.
		std::function<string(const Gtk::FlowBoxChild*)> getPosition([&](const Gtk::FlowBoxChild* sel) {
			return Defaults::explode(sel->get_child()->get_name(), '_')[1];
		});

		switch (static_cast<tabIndex>(notebookDeviceConnections->get_current_page())) {
			case tabIndex::RGB:
				positionRGB->set_text(getPosition(selected.at(0)));
				break;
			case tabIndex::Strip: {
				const auto pos(getPosition(selected.front()));
				positionStrip->set_text(pos);
				sizeStrip->set_text(std::to_string(1 + std::stoi(getPosition(selected.back())) - std::stoi(pos)));
				break;
			}
			case tabIndex::sRGB: {
				// Assuming all hardware is sequential.
				int first(Storage::Element::findFirstConnectorIndexByPosition(getPosition(selected.at(0))));
				pinR->set_text(std::to_string(first++));
				pinG->set_text(std::to_string(first++));
				pinB->set_text(std::to_string(first));
				break;
			}
			case tabIndex::mRGB: {
				vector<string> csvs;
				for (auto s : selected) {
					csvs.push_back(getPosition(s));
				}
				positionsMRGB->set_text(Defaults::implode(csvs, ','));
				break;
			}
		}
	});

	solenoid->signal_toggled().connect([&]() {
		bool active(solenoid->get_active());
		timeOn->set_sensitive(active);
		timeOn->set_text("");
		brightness->set_sensitive(active and not isMono);
		brightness->set_value(100);
	});

	// When the notebook page changes, remove the values on the other page.
	notebookDeviceConnections->signal_switch_page().connect(sigc::mem_fun(*this, &DialogElement::onSwitchPage));

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
				elementName->set_text(name);
				elementType->set_active_id(Defaults::detectElementType(name));
				elementName->grab_focus();
			}
		}
		dialogGenerateElementName->hide();
	});
}

void DialogElement::load(XMLHelper* values) {
	createItems(values->getData(Defaults::createCommonUniqueId({owner->createUniqueId(), COLLECTION_ELEMENT})), values);
}

void DialogElement::clearForm() {
	clearFormConditinal(false);
}

void DialogElement::clearFormConditinal(uint8_t flags) {

	pinsBox->unselect_all();

	// Single pin.
	if (not flags or (flags & tabBit::RGB)) {
		pin->set_text("");
		solenoid->set_active(false);
		timeOn->set_text("");
		timeOn->set_sensitive(false);
	}

	// Scattered RGB.
	if (not flags or (flags & tabBit::sRGB)) {
		pinR->set_text("");
		pinG->set_text("");
		pinB->set_text("");
	}

	// RGB.
	if (not flags or (flags & tabBit::Strip)) {
		positionRGB->set_text("");
		comboBoxRGBRGB->set_active_id(defaultRGBFormat);
	}

	// RGB Strip.
	if (not flags or (flags & tabBit::Single)) {
		positionStrip->set_text("");
		sizeStrip->set_text("");
		comboBoxRGBStrip->set_active_id(defaultRGBFormat);
	}

	// Multi RGB.
	if (not flags or (flags & tabBit::mRGB)) {
		positionsMRGB->set_text("");
		comboBoxRGBMRGB->set_active_id(defaultRGBFormat);
	}

	if (not flags) {
		// Common fields to always reset.
		elementName->set_text("");
		elementName->grab_focus();
		elementType->set_active_id("0");
		brightness->set_value(100);
		brightness->set_sensitive(not isMono);
		DialogColors::getInstance()->colorizeButton(btnDefaultColor, NO_COLOR);
		// refresh page.

		Gtk::Widget* page_widget = nullptr;
		page_widget = notebookDeviceConnections->get_nth_page(static_cast<uint8_t>(tabIndex::RGB));
		page_widget->set_visible(canRGB);
		page_widget = notebookDeviceConnections->get_nth_page(static_cast<uint8_t>(tabIndex::Strip));
		page_widget->set_visible(canStrip);
		page_widget = notebookDeviceConnections->get_nth_page(static_cast<uint8_t>(tabIndex::mRGB));
		page_widget->set_visible(canRGB);
		int idx(notebookDeviceConnections->get_current_page());
		Gtk::Widget* page = notebookDeviceConnections->get_nth_page(idx);
		onSwitchPage(page, idx);
		drawPins();
	}
}

void DialogElement::isValid() const {
	// Check invalid name
	string name(createUniqueId());
	if (name.empty()) {
		if (mode != Modes::LOAD)
			elementName->grab_focus();
		throw Message("Missing element name");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (elementHandler->isUsed(name)) {
		if (mode != Modes::EDIT or currentData->createUniqueId() != name) {
			if (mode != Modes::LOAD)
				elementName->grab_focus();
			throw Message("Element with name " + name + " already exist");
		}
	}

	// Check for connectors errors.
	std::function<void(Gtk::Entry*)> checkPin = [&](Gtk::Entry* connector) {
		string
			conn(connector->get_text()),
			name(connector->get_placeholder_text());

		// Check empty.
		if (conn.empty()) {
			if (mode != Modes::LOAD)
				connector->grab_focus();
			throw Message("Enter a valid connector number for " + name);
		}

		// Numeric check and range.
		if (not Defaults::isBetween(conn, 1, numberOfPins)) {
			if (mode != Modes::LOAD)
				connector->grab_focus();
			throw Message("The connector for " + name + " must be a number from 1 and " + std::to_string(numberOfPins));
		}
	};

	switch (static_cast<tabIndex>(notebookDeviceConnections->get_current_page())) {
	case tabIndex::Single:

		// Check for changes, solenoid or pin.
		if (mode != Modes::EDIT or pin->get_text() != currentData->getValue(PIN) + currentData->getValue(SOLENOID)) {
			checkPin(pin);
		}

		// Check solenoid milliseconds.
		if (
			solenoid->get_active()         and
			not timeOn->get_text().empty() and
			not Defaults::isNumber(timeOn->get_text())
		) {
			if (mode != Modes::LOAD)
				timeOn->grab_focus();
			throw Message("Enter a valid number of milliseconds for the timer.");
		}
		break;

	// Scattered RGB.
	case tabIndex::sRGB: {
		// Check empty.
		if (
			pinR->get_text().empty() or
			pinG->get_text().empty() or
			pinB->get_text().empty()
		) {
			throw Message("Missing connection information for scattered RGB Element");
		}
		// RGB checks
		bool found = false;
		for (const auto e: {pinR, pinG, pinB}) {
			// Check local form for duplicated pins in RGB.
			for (const auto p: {pinR, pinG, pinB}) {
				if (e == p)
					continue;
				if (e->get_text() == p->get_text()) {
					if (found) {
						if (mode != Modes::LOAD)
							pin->grab_focus();
						throw Message("Connector " + p->get_text() + " is set more than once in " + p->get_name());
					}
					found = true;
				}
			}
			// Value is new or changed.
			if (not found) {
				checkPin(e);
			}
		}
		break;
	}
	// RGB.
	case tabIndex::RGB:
		// Check empty.
		if (positionRGB->get_text().empty()) {
			throw Message("Missing element position, select an connector.");
		}
		break;
	case tabIndex::Strip:
		if (positionStrip->get_text().empty()) {
			throw Message("Missing element position and size, select one or more connectors.");
		}
		break;
	case tabIndex::mRGB:
		if (std::count(positionsMRGB->get_text().begin(), positionsMRGB->get_text().end(), ',') < 2) {
			throw Message("Missing element positions, select at least two connectors.");
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

	switch (static_cast<tabIndex>(notebookDeviceConnections->get_current_page())) {
	case tabIndex::Single:
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
		break;
	case tabIndex::sRGB:
		currentData->setValue(RED_PIN, pinR->get_text());
		currentData->setValue(GREEN_PIN, pinG->get_text());
		currentData->setValue(BLUE_PIN, pinB->get_text());
		break;
	case tabIndex::RGB:
		currentData->setValue(POSITION, positionRGB->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBRGB->get_active_id());
		break;
	case tabIndex::Strip:
		currentData->setValue(POSITION, positionStrip->get_text());
		currentData->setValue(STRIPSIZE, sizeStrip->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBStrip->get_active_id());
		break;
	case tabIndex::mRGB:
		currentData->setValue(POSITIONS, positionsMRGB->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBMRGB->get_active_id());
		break;
	}

	if (not btnDefaultColor->get_tooltip_text().empty()) {
		currentData->setValue(DEFAULT_COLOR, btnDefaultColor->get_tooltip_text());
	}
	currentData->setValue(TYPE, elementType->get_active_id() == "0" ? DEFAULT_ELEMENT_TYPE : elementType->get_active_id());
	currentData->setValue(BRIGHTNESS, std::to_string(static_cast<uint>(brightness->get_value())));
}

void DialogElement::retrieveData() {

	// Gets the string position and selects the connection, returns the index.
	std::function<const int(const string&)> setSelectedConnectors = [&](const string& connector) {
		int idx;
		try {
			idx = std::stoi(connector) -1;
		}
		catch (...) {
			return -1;
		}
		Gtk::FlowBoxChild* child(pinsBox->get_child_at_index(idx));
		if (not child)
			return -1;
		pinsBox->select_child(*child);
		return idx;
	};

	elementName->set_text(currentData->getValue(NAME));

	// Multi RGB.
	if (not currentData->getValue(POSITIONS).empty()) {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::mRGB));
		for (const auto position : Defaults::explode(currentData->getValue(POSITIONS), ',')) {
			setSelectedConnectors(position);
		}
		comboBoxRGBMRGB->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// LED strip.
	else if (not currentData->getValue(STRIPSIZE).empty()) {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::Strip));
		int
			idx(setSelectedConnectors(currentData->getValue(POSITION))),
			siz;
		if (idx != -1) {
			try {
				siz = std::stoi(currentData->getValue(STRIPSIZE));
			}
			catch (...) {}
			setSelectedConnectors(std::to_string(idx + siz));
		}
		comboBoxRGBStrip->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// RGB.
	else if (not currentData->getValue(POSITION).empty()) {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::RGB));
		setSelectedConnectors(currentData->getValue(POSITION));
		comboBoxRGBRGB->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// Single.
	else if (not currentData->getValue(PIN).empty()) {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::Single));
		pin->set_text(currentData->getValue(PIN));
		solenoid->set_active(false);
	}
	// Solenoid.
	else if (not currentData->getValue(SOLENOID).empty()) {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::Single));
		pin->set_text(currentData->getValue(SOLENOID));
		solenoid->set_active(true);
		if (not currentData->getValue(TIME_ON).empty())
			timeOn->set_text(currentData->getValue(TIME_ON));
	}
	// Scattered RGB
	else {
		notebookDeviceConnections->set_current_page(static_cast<uint8_t>(tabIndex::sRGB));
		pinR->set_text(currentData->getValue(RED_PIN));
		pinG->set_text(currentData->getValue(GREEN_PIN));
		pinB->set_text(currentData->getValue(BLUE_PIN));
	}
	DialogColors::getInstance()->colorizeButton(
		btnDefaultColor,
		currentData->getValue(DEFAULT_COLOR, NO_COLOR)
	);
	elementType->set_active_id(currentData->getValue(TYPE, DEFAULT_ELEMENT_TYPE));
	brightness->set_value(std::stoi(currentData->getValue(BRIGHTNESS, DEFAULT_BRIGHTNESS)) ?: 100);
}

string const DialogElement::createUniqueId() const {
	return Defaults::createCommonUniqueId({elementName->get_text()});
}

void DialogElement::changeNumberOfPins(const uint16_t newSize) {

	if (not newSize) {
		numberOfPins = 0;
		drawPins();
	}

	if (numberOfPins == newSize)
		return;

	// resize.
	if (newSize < numberOfPins) {
		uint16_t pinsToSearch(numberOfPins - newSize);
		unordered_set<Storage::BoxButton*> elementsToDelete;
		for (uint16_t c(numberOfPins - pinsToSearch); c < numberOfPins; ++c)
			findElementByPin(c, elementsToDelete);
		// delete elements
		string deleted;
		for(auto boxButton : elementsToDelete) {
			deleted += boxButton->getData()->getValue(NAME) + "\n";
			// elements doesn't need activation.
			box->remove(*boxButton);
			items->remove(boxButton);
		}
		if (not deleted.empty())
			Message::displayInfo("Elements being deleted due to resizing:\n" +deleted);
	}

	numberOfPins = newSize;
	drawPins();
}

void DialogElement::setRules(bool isMono, bool canRGB, bool canStrip) {
	this->isMono   = isMono;
	this->canRGB   = canRGB;
	this->canStrip = canStrip;
}

void DialogElement::drawPins() {

	// Remove pins layout.
	for (auto child : pinsBox->get_children())
		pinsBox->remove(*child);

	if (not numberOfPins)
		return;

	std::vector<std::pair<string, string>> pinsUsage(numberOfPins, {NO_COLOR, ""});
	findConnectorTypes(pinsUsage);
	// Create pin labels.
	vector<Gtk::Label *> labels;
	labels.reserve(numberOfPins);
	for (auto c = 1; c <= numberOfPins; ++c) {
		Gtk::Label* label(Gtk::make_managed<Gtk::Label>(std::to_string(c)));
		label->get_style_context()->add_class(PIN_LABEL);
		auto& pinUsage(pinsUsage[c - 1]);
		string labelTxt("Connector " + std::to_string(c));
		if (pinUsage.first.empty()) {
			labelTxt += " is unused";
		}
		else {
			labelTxt += string(" is used by element") + (pinUsage.first == COLOR_MULTIPLE ? "s " : " ");
			label->get_style_context()->add_class(std::move(pinUsage.first));
		}
		label->set_tooltip_text(labelTxt + std::move(pinUsage.second));
		labels.push_back(label);
	}
	// call hardware function
	if (canRGB) {
		drawPinsRGB(labels);
	}
	else {
		drawPins(labels);
	}
	pinsBox->show_all();
}

void DialogElement::drawPinsRGB(vector<Gtk::Label*>& labels) {
	uint16_t totalLeds(numberOfPins / 3), pin(0);
	// Get the best size for the box.
	findLargestDivisor(totalLeds);
	for (uint led = 1; led <= totalLeds; ++led) {
		// Create box.
		Gtk::VBox* vboxConnector = Gtk::manage(new Gtk::VBox(true));
		pinsBox->add(*vboxConnector);
		vboxConnector->set_name("boxRGB_" + std::to_string(led));
		vboxConnector->get_style_context()->add_class(CONNECTOR_BOX);
		vboxConnector->set_hexpand(false);
		vboxConnector->set_vexpand(false);
		vboxConnector->set_valign(Gtk::ALIGN_START);
		// Create box Label
		Gtk::Label* connectorLabel(Gtk::make_managed<Gtk::Label>("LED " + std::to_string(led)));
		vboxConnector->pack_start(*connectorLabel, Gtk::PACK_SHRINK);
		// Create container for pins
		Gtk::HBox* pinsBox = Gtk::manage(new Gtk::HBox(true, 1));
		vboxConnector->pack_start(*pinsBox, Gtk::PACK_SHRINK);
		for (uint l = 0; l < 3; ++l) {
			pinsBox->pack_start(*labels[pin++]);
		}
	}
	if (numberOfPins % 3 == 0)
		return;

	// Create box for extra pins.
	Gtk::VBox* vboxConnector = Gtk::manage(new Gtk::VBox(true));
	pinsBox->add(*vboxConnector);
	vboxConnector->get_style_context()->add_class(CONNECTOR_BOX);
	vboxConnector->set_hexpand(false);
	vboxConnector->set_vexpand(false);
	vboxConnector->set_valign(Gtk::ALIGN_START);
	// Create box Label
	Gtk::Label* connectorLabel(Gtk::make_managed<Gtk::Label>("Extra"));
	vboxConnector->pack_start(*connectorLabel, Gtk::PACK_SHRINK);
	// Create container for pins
	Gtk::HBox* pinsBox = Gtk::manage(new Gtk::HBox(true, 1));
	vboxConnector->pack_start(*pinsBox, Gtk::PACK_SHRINK);
	for (; pin < numberOfPins; ++pin) {
		pinsBox->pack_start(*labels[pin]);
	}
}

void DialogElement::drawPins(vector<Gtk::Label *>& labels) {
	findLargestDivisor(numberOfPins);
	for (uint16_t pin = 0; pin < numberOfPins; ++pin) {
		pinsBox->add(*labels[pin]);
	}
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

void DialogElement::findConnectorTypes(vector<std::pair<string, string>>& pinsUsage) {

	std::function<void(std::pair<string, string>&, const string&)> storePins = [&](std::pair<string, string>& pair, const string& color) {
		if (pair.first == color or pair.first == NO_COLOR)
			pair.first = color;
		else
			pair.first = COLOR_MULTIPLE;
	};

	std::function<void(
		vector<std::pair<string, string>>&,
		const string&,
		const Storage::Element::Data*
	)> storePinsRGB = [&](
		vector<std::pair<string, string>>& pinsList,
		const string& position,
		const Storage::Element::Data* data
	) {
		uint16_t index(Storage::Element::findFirstConnectorIndexByPosition(position));
		for (const auto& c : data->getValue(COLORFORMAT)) {
			auto& pair(pinsList[index++]);
			if (pair.second.empty())
				pair.second = data->getValue(NAME);
			else
				pair.second = pair.second + " - " + data->getValue(NAME);
			switch (c) {
			case 'R':
				storePins(pair, COLOR_RED);
				break;
			case 'G':
				storePins(pair, COLOR_GREEN);
				break;
			case 'B':
				storePins(pair, COLOR_BLUE);
			}
		}
	};

	for (const auto boxButton : *items) {
		// Multiple RGB.
		if (not boxButton->getData()->getValue(POSITIONS).empty()) {
			for (const auto& position : Defaults::explode(boxButton->getData()->getValue(POSITIONS), ',')) {
				storePinsRGB(pinsUsage, position, boxButton->getData());
			}
		}
		// LED strip.
		else if (not boxButton->getData()->getValue(STRIPSIZE).empty()) {
			const uint16_t
				s(std::stoi(boxButton->getData()->getValue(POSITION))),
				t(s + std::stoi(boxButton->getData()->getValue(STRIPSIZE)));
			for (uint16_t c(s); c < t; ++c) {
				storePinsRGB(pinsUsage, std::to_string(c), boxButton->getData());
			}
		}
		// RGB.
		else if (not boxButton->getData()->getValue(POSITION).empty()) {
			storePinsRGB(pinsUsage, boxButton->getData()->getValue(POSITION), boxButton->getData());
		}
		// Single.
		else if (not boxButton->getData()->getValue(PIN).empty()) {
			const uint16_t index(std::stoi(boxButton->getData()->getValue(PIN)) - 1);
			storePins(pinsUsage[index], COLOR_PIN);
			pinsUsage[index].second = boxButton->getData()->getValue(NAME);
		}
		// Solenoid.
		else if (not boxButton->getData()->getValue(SOLENOID).empty()) {
			const uint16_t index(std::stoi(boxButton->getData()->getValue(SOLENOID)) - 1);
			storePins(pinsUsage[index], COLOR_SOLENOID);
			pinsUsage[index].second = boxButton->getData()->getValue(NAME);
		}
		// Scattered RGB
		else {
			// Red.
			uint16_t index(std::stoi(boxButton->getData()->getValue(RED_PIN)) - 1);
			storePins(pinsUsage[index], COLOR_RED);
			pinsUsage[index].second = boxButton->getData()->getValue(NAME);
			// Green.
			index = std::stoi(boxButton->getData()->getValue(GREEN_PIN)) - 1;
			storePins(pinsUsage[index], COLOR_GREEN);
			pinsUsage[index].second = boxButton->getData()->getValue(NAME);
			// Blue.
			index = std::stoi(boxButton->getData()->getValue(BLUE_PIN)) - 1;
			storePins(pinsUsage[index], COLOR_BLUE);
			pinsUsage[index].second = boxButton->getData()->getValue(NAME);
		}
	}
}

void DialogElement::findLargestDivisor(uint16_t size) {
	int best_divisor = MAX_COLUMNS;
	int best_rows = 1;

	// Look for the best divisor
	for (int c = MAX_COLUMNS; c >= MIN_COLUMNS; --c) {
		if (size % c == 0) { // Perfect division
			best_divisor = c;
			best_rows = size / c;
			break; // We've found a perfect divisor, no need to continue
		} else if (size > c && size % c < best_divisor % c) { // Not perfect but better than previous
			best_divisor = c;
			best_rows = (size + c - 1) / c; // Ceiling division to ensure we have enough rows
		}
	}

	// If no good divisor was found, use MAX_COLUMNS, but adjust min
	if (best_divisor == MAX_COLUMNS) {
		pinsBox->set_max_children_per_line(MAX_COLUMNS);
		pinsBox->set_min_children_per_line(MIN_COLUMNS);
	} else {
		// Ensure min_children_per_line is at least MIN_COLUMNS but not more than half of best_divisor
		int min_per_line = std::max(MIN_COLUMNS, best_divisor / 2);
		pinsBox->set_max_children_per_line(best_divisor);
		pinsBox->set_min_children_per_line(min_per_line);
	}
}

void DialogElement::onSwitchPage(Gtk::Widget* page, guint pageNum) {
	switch (static_cast<tabIndex>(pageNum)) {
	case tabIndex::RGB:
	case tabIndex::sRGB:
		pinsBox->set_selection_mode(Gtk::SELECTION_SINGLE);
		break;
	case tabIndex::Strip:
	case tabIndex::mRGB:
		pinsBox->set_selection_mode(Gtk::SELECTION_MULTIPLE);
		break;
	default:
		pinsBox->set_selection_mode(Gtk::SELECTION_NONE);
	}
	uint8_t exclude = 1 << pageNum;
	clearFormConditinal(0b11111 & ~exclude);
}

void DialogElement::findElementByPin(uint16_t finder, unordered_set<Storage::BoxButton*>& elementsFound) {
	const string connector(std::to_string(finder));
	for (const auto boxButton : *items) {

		// Single LED.
		string subject(boxButton->getData()->getValue(PIN, boxButton->getData()->getValue(SOLENOID)));
		if (not subject.empty()) {
			if (connector == subject)
				elementsFound.insert(boxButton);
			continue;
		}
		// Multi LED.
		subject = boxButton->getData()->getValue(POSITIONS);
		if (not subject.empty()) {
			bool found = false;
			for (const auto& position : Defaults::explode(subject, ',')) {
				auto pinNum(Storage::Element::findFirstConnectorIndexByPosition(position));
				if (finder >= pinNum and finder <= pinNum +2) {
					found = true;
					break;
				}
			}
			if (found) {
				elementsFound.insert(boxButton);
				continue;
			}
		}
		// RGB Strip.
		subject = boxButton->getData()->getValue(POSITION);
		if (not boxButton->getData()->getValue(STRIPSIZE).empty()) {
			uint16_t
				lastPin(Storage::Element::findFirstConnectorIndexByPosition(boxButton->getData()->getValue(STRIPSIZE)) + 2),
				firstPin(Storage::Element::findFirstConnectorIndexByPosition(subject));

			if (firstPin >= finder and firstPin <= lastPin) {
				elementsFound.insert(boxButton);
				continue;
			}
		}
		// RGB.
		if (not subject.empty()) {
			auto pinNum(Storage::Element::findFirstConnectorIndexByPosition(subject));
			if (finder >= pinNum and finder <= pinNum +2) {
				elementsFound.insert(boxButton);
				continue;
			}
		}
		// Scattered RGB.
		subject = boxButton->getData()->getValue(RED_PIN);
		if (not subject.empty()) {
			uint16_t pinNum(std::stoi(subject));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
			pinNum = std::stoi(boxButton->getData()->getValue(GREEN_PIN));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
			pinNum = std::stoi(boxButton->getData()->getValue(BLUE_PIN));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
		}
	}
}
