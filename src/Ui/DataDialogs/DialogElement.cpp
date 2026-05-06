/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.cpp
 * @since     Mar 13, 2023
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

#include "DialogElement.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogElement::DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	DialogForm(obj, builder)
{

	// Connect Element Box and buttons.
	builder->get_widget_derived(
		"BoxElements",
		box,
		"BtnDeviceElementUp",
		"BtnDeviceElementDn",
		"BtnDeviceElementFirst",
		"BtnDeviceElementLast"
	);
	builder->get_widget("BtnApplyElement",     btnApply);
	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddElement", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Parent device.
	builder->get_widget("ComboBoxDevices", comboBoxDevices);

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
		if (selected.empty()) return;

		// Extracts the position number from the flowbox child using the name.
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
				pinR->set_text(std::to_string(++first));
				pinG->set_text(std::to_string(++first));
				pinB->set_text(std::to_string(++first));
				break;
			}
			case tabIndex::mRGB: {
				StringVector csvs;
				for (auto s : selected) {
					csvs.push_back(getPosition(s));
				}
				positionsMRGB->set_text(Defaults::implode(csvs, ','));
				break;
			}
			default: break;
		}
	});

	solenoid->signal_toggled().connect([&]() {
		bool active(solenoid->get_active());
		timeOn->set_sensitive(active);
		timeOn->set_text("");
		brightness->set_sensitive(not active and not Defaults::isMonochrome(comboBoxDevices->get_active_id()));
		if (active) brightness->set_value(100);
	});

	// When the notebook page changes, remove the values on the other page.
	switchPageConnection = notebookDeviceConnections->signal_switch_page().connect(sigc::mem_fun(*this, &DialogElement::onSwitchPage));

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

void DialogElement::load(DataMap& values) noexcept {
	createItems(values.at(Defaults::createCommonUniqueId({ownerData->createUniqueId(), COLLECTION_ELEMENTS})), values);
}

void DialogElement::clearForm() noexcept {
	clearFormConditinal(0);
}

void DialogElement::clearFormConditinal(uint8_t flags) noexcept {

	pinsBox->unselect_all();

	// Single pin.
	if (not flags or (flags & (1 << tabIndex::Single))) {
		pin->set_text("");
		solenoid->set_active(false);
		timeOn->set_text("");
		timeOn->set_sensitive(false);
		if (flags)
			// restore it to the device-native sensitivity when switching pages.
			brightness->set_sensitive(not Defaults::isMonochrome(comboBoxDevices->get_active_id()));
	}

	// Scattered RGB.
	if (not flags or (flags & (1 << tabIndex::sRGB))) {
		pinR->set_text("");
		pinG->set_text("");
		pinB->set_text("");
	}

	// RGB.
	if (not flags or (flags & (1 << tabIndex::RGB))) {
		positionRGB->set_text("");
		comboBoxRGBRGB->set_active_id(defaultRGBFormat);
	}

	// RGB Strip.
	if (not flags or (flags & (1 << tabIndex::Strip))) {
		positionStrip->set_text("");
		sizeStrip->set_text("");
		comboBoxRGBStrip->set_active_id(defaultRGBFormat);
	}

	// Multi RGB.
	if (not flags or (flags & (1 << tabIndex::mRGB))) {
		positionsMRGB->set_text("");
		comboBoxRGBMRGB->set_active_id(defaultRGBFormat);
	}

	if (not flags) {
		auto dev(Defaults::devicesInfo.at(comboBoxDevices->get_active_id()));
		// Common fields to always reset.
		elementName->set_text("");
		elementName->grab_focus();
		elementType->set_active_id("0");
		brightness->set_value(100);
		brightness->set_sensitive(not dev.monochrome);
		DialogColors::getInstance()->colorizeButton(btnDefaultColor, NO_COLOR);
		Gtk::Widget* page_widget = nullptr;
		page_widget = notebookDeviceConnections->get_nth_page(tabIndex::RGB);
		page_widget->set_visible(dev.layoutRGB);
		page_widget = notebookDeviceConnections->get_nth_page(tabIndex::Strip);
		page_widget->set_visible(dev.supportStrip);
		page_widget = notebookDeviceConnections->get_nth_page(tabIndex::mRGB);
		page_widget->set_visible(dev.layoutRGB);
		int idx(dev.monochrome ? tabIndex::Single : (dev.layoutRGB ? tabIndex::RGB : tabIndex::sRGB));
		notebookDeviceConnections->set_current_page(idx);
		Gtk::Widget* page = notebookDeviceConnections->get_nth_page(idx);
		onSwitchPage(page, idx);
		drawPins();
	}
}

void DialogElement::isValid() const {
	// Check invalid name
	string name(createUniqueId());
	if (name.empty()) {
		if (action != Actions::LOAD)
			elementName->grab_focus();
		throw Message("Missing element name");
	}

	// If is not edit, or data is not the same, check for dupes.
	if (currentData->getCollectionHandler()->isIdSet(name)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != name) {
			if (action != Actions::LOAD)
				elementName->grab_focus();
			throw Message("Element with name " + name + " already exist");
		}
	}

	auto groupHandler = LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_GROUPS);
	auto existingGroup = groupHandler->get(name);
	if (existingGroup and not existingGroup->getProperties().isSet(PROP_SYSTEM)) {
		throw Message("Element name '" + name + "' conflicts with existing group.\nNote: Strip elements auto-create groups with the same name.");
	}

	// Check for connectors errors.
	std::function<void(Gtk::Entry*)> checkPin = [&](Gtk::Entry* connector) {
		string
			conn(connector->get_text()),
			name(connector->get_placeholder_text());

		// Check empty.
		if (conn.empty()) {
			if (action != Actions::LOAD)
				connector->grab_focus();
			throw Message("Enter a valid connector number for " + name);
		}

		// Numeric check and range.
		if (not Defaults::isBetween(conn, 1, numberOfPins)) {
			if (action != Actions::LOAD)
				connector->grab_focus();
			throw Message("The connector for " + name + " must be a number from 1 and " + std::to_string(numberOfPins));
		}
	};

	switch (static_cast<tabIndex>(notebookDeviceConnections->get_current_page())) {
	case tabIndex::Single:

		// Check for changes, solenoid or pin.
		if (action != Actions::EDIT or pin->get_text() != currentData->getValue(PIN) + currentData->getValue(SOLENOID)) {
			checkPin(pin);
		}

		// Check solenoid milliseconds.
		if (
			solenoid->get_active()         and
			not timeOn->get_text().empty() and
			not Defaults::isNumber(timeOn->get_text())
		) {
			if (action != Actions::LOAD)
				timeOn->grab_focus();
			throw Message("Enter a valid number of milliseconds for the timer for element " + name + ".");
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
			throw Message("Missing connection information for scattered RGB Element " + name + ".");
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
						if (action != Actions::LOAD)
							pin->grab_focus();
						throw Message("In element " + name + ", the connector " + p->get_text() + " is set more than once in " + p->get_name());
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
			throw Message("Missing element position for element " + name + ", select an connector.");
		}
		break;
	case tabIndex::Strip:
		if (positionStrip->get_text().empty()) {
			throw Message("Missing element position and size for element " + name + ", select one or more connectors.");
		}
		break;
	case tabIndex::mRGB:
		if (std::count(positionsMRGB->get_text().begin(), positionsMRGB->get_text().end(), ',') < 2) {
			throw Message("Missing element positions for element " + name + ", select at least two connectors.");
		}
		break;
	default:
		throw Message("Internal Error.");
	}
}

void DialogElement::storeData() noexcept {

	auto& props{currentData->getProperties()};
	string
		name(elementName->get_text()),
		oldName(currentData->getValue(NAME));

	bool
		isStrip  = false,
		wasStrip = not currentData->getValue(STRIPSIZE).empty();

	auto groupCollectionHandler(LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_GROUPS));

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
		currentData->setValue(RED_PIN,   pinR->get_text());
		currentData->setValue(GREEN_PIN, pinG->get_text());
		currentData->setValue(BLUE_PIN,  pinB->get_text());
		break;
	case tabIndex::RGB:
		currentData->setValue(POSITION,    positionRGB->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBRGB->get_active_id());
		break;
	case tabIndex::Strip: {

		isStrip = true;

		static uint16_t lastStripCode = 0;
		auto collection{currentData->getCollectionHandler()};

		const size_t
			position = std::stoi(positionStrip->get_text()),
			size     = std::stoi(sizeStrip->get_text());

		// Get or create strip code.
		string code{props.getValue(PROP_STRIP_UID)};
		if (code.empty()) {
			code = std::to_string(++lastStripCode);
			props.setValue(PROP_STRIP_UID, code);
		}
		// Store strip properties.
		props.setValue(PROP_EXPAND,    "1"); // Expandable into many elements.
		props.setValue(PROP_SYSTEM,    "1"); // System element with special handling.
		props.setValue(PROP_NO_SELECT, "1"); // Cannot be consumed in selectable end-points.

		// Upscale, copy children pointers.
		auto currentDataE = static_cast<Storage::Element*>(currentData);
		auto children     = currentDataE->copyStripChildren();

		// Loop size to create/reuse children, this avoids chain reactions.
		for (size_t i = 0; i < size; ++i) {

			const string childName{name + std::to_string(i + 1)};

			if (i < children.size()) {
				// Reuse existing child - update name and position
				string oldChildId = children[i]->createUniqueId();
				children[i]->setValue(NAME, childName);
				children[i]->setValue(POSITION, std::to_string(position + i));
				collection->replace(children[i], oldChildId);
			}
			else {
				// Create new child
				StringUMap childData;
				childData[NAME]     = childName;
				childData[POSITION] = std::to_string(position + i);
				auto child{new Storage::Element(childData)};
				child->getProperties().setValue(PROP_STRIP, code);
				currentDataE->addStripChild(child);
				collection->add(child);
			}
		}

		// Delete excess children (unregisters from collection and trims stripChildren)
		currentDataE->deleteExcessStripChildren(size);

		currentData->setValue(POSITION,    positionStrip->get_text());
		currentData->setValue(STRIPSIZE,   sizeStrip->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBStrip->get_active_id());

		// System group management.
		if (not wasStrip) {
			// Create new group.
			StringUMap groupData{{"name", name}};
			auto group = new Storage::Group(groupData);
			group->getProperties().setValue(PROP_SYSTEM, "1");
			groupCollectionHandler->add(group);
		}
		else if (oldName != name) {
			// Rename existing group.
			auto group{groupCollectionHandler->get(oldName)};
			if (group->getProperties().isSet(PROP_SYSTEM)) {
				group->setValue(NAME, name);
				groupCollectionHandler->replace(group, oldName);
			}
		}
		break;
	}
	case tabIndex::mRGB:
		currentData->setValue(POSITIONS,   positionsMRGB->get_text());
		currentData->setValue(COLORFORMAT, comboBoxRGBMRGB->get_active_id());
		break;
	default: return;
	}

	if (not btnDefaultColor->get_label().empty()) {
		currentData->setValue(DEFAULT_COLOR, btnDefaultColor->get_label());
	}
	currentData->setValue(NAME, name);
	currentData->setValue(TYPE, elementType->get_active_id() == "0" ? Glib::ustring(DEFAULT_ELEMENT_TYPE) : elementType->get_active_id());
	currentData->setValue(BRIGHTNESS, std::to_string(static_cast<uint>(brightness->get_value())));

	// Cleanup if changed from strip to non-strip
	if (wasStrip and not isStrip) {
		static_cast<Storage::Element*>(currentData)->clearStripChildren();
		props.unSet(PROP_STRIP_UID);
		props.unSet(PROP_EXPAND);
		props.unSet(PROP_SYSTEM);
		props.unSet(PROP_NO_SELECT);

		auto group{groupCollectionHandler->get(oldName)};
		if (group and group->getProperties().isSet(PROP_SYSTEM)) {
			groupCollectionHandler->remove(group);
		}
	}
}

void DialogElement::retrieveData() noexcept {

	// Gets the string position and selects the connection, returns the index.
	std::function<const int(const string&)> setConnectorSelected = [&](const string& connector) {
		int idx;
		try {
			idx = std::stoi(connector) -1;
		}
		catch (...) {
			return -1;
		}
		Gtk::FlowBoxChild* child(pinsBox->get_child_at_index(idx));
		if (not child) return -1;
		pinsBox->select_child(*child);
		return idx;
	};

	/*
	Element type is detected from which fieldsData keys are present.
	Priority order matches mutual exclusivity of the connection types:
		1. POSITIONS  → mRGB   (multiple positions, CSV)
		2. STRIPSIZE  → Strip  (position + count)
		3. POSITION   → RGB    (single position)
		4. PIN        → Single LED
		5. SOLENOID   → Single solenoid
		6. (none)     → Scattered RGB (RED_PIN/GREEN_PIN/BLUE_PIN)
	*/
	elementName->set_text(currentData->getValue(NAME));

	// Multi RGB.
	if (not currentData->getValue(POSITIONS).empty()) {
		notebookDeviceConnections->set_current_page(tabIndex::mRGB);
		for (const auto& position : Defaults::explode(currentData->getValue(POSITIONS), ',')) {
			setConnectorSelected(position);
		}
		comboBoxRGBMRGB->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// LED strip.
	else if (not currentData->getValue(STRIPSIZE).empty()) {
		notebookDeviceConnections->set_current_page(tabIndex::Strip);
		positionStrip->set_text(currentData->getValue(POSITION));
		sizeStrip->set_text(currentData->getValue(STRIPSIZE));

		int idx, tot;
		try {
			idx = std::stoi(currentData->getValue(POSITION)) -1;
			tot = std::stoi(currentData->getValue(STRIPSIZE));
		}
		catch (...) {
			// impossible invalid data, skip.
			return;
		}
		for (int c = idx; c < idx + tot; ++c) {
			Gtk::FlowBoxChild* child(pinsBox->get_child_at_index(c));
			if (not child) continue;
			pinsBox->select_child(*child);
		}
		comboBoxRGBStrip->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// RGB.
	else if (not currentData->getValue(POSITION).empty()) {
		notebookDeviceConnections->set_current_page(tabIndex::RGB);
		setConnectorSelected(currentData->getValue(POSITION));
		comboBoxRGBRGB->set_active_id(currentData->getValue(COLORFORMAT));
	}
	// Single.
	else if (not currentData->getValue(PIN).empty()) {
		notebookDeviceConnections->set_current_page(tabIndex::Single);
		pin->set_text(currentData->getValue(PIN));
		solenoid->set_active(false);
	}
	// Solenoid.
	else if (not currentData->getValue(SOLENOID).empty()) {
		notebookDeviceConnections->set_current_page(tabIndex::Single);
		pin->set_text(currentData->getValue(SOLENOID));
		solenoid->set_active(true);
		if (not currentData->getValue(TIME_ON).empty())
			timeOn->set_text(currentData->getValue(TIME_ON));
	}
	// Scattered RGB
	else {
		notebookDeviceConnections->set_current_page(tabIndex::sRGB);
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

string DialogElement::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({elementName->get_text()});
}

void DialogElement::changeNumberOfPins(const uint16_t newSize) noexcept {

	if (not newSize) {
		numberOfPins = 0;
		drawPins();
		return;
	}

	if (numberOfPins == newSize) return;

	if (not numberOfPins) {
		numberOfPins = newSize;
		drawPins();
		return;
	}

	// resize.
	if (newSize < numberOfPins) {
		uint16_t pinsToSearch(numberOfPins - newSize);
		std::unordered_set<Storage::BoxButton*> elementsToDelete;
		for (uint16_t c(numberOfPins - pinsToSearch); c < numberOfPins; ++c)
			findElementByPin(c, elementsToDelete);
		// delete elements
		string deleted;
		for (auto boxButton : elementsToDelete) {
			deleted += boxButton->getData()->getValue(NAME) + "\n";
			box->remove(*boxButton);
			items->remove(*boxButton);
		}
		if (not deleted.empty())
			Message::displayInfo("Elements being deleted due to resizing:\n" +deleted,
				familyToDialog.at(COLLECTION_DEVICES)
			);
	}

	numberOfPins = newSize;
	drawPins();
}

void DialogElement::handleLayoutChange(
	const Defaults::DeviceInfo& oldInfo,
	const Defaults::DeviceInfo& newInfo
) noexcept {

	//  remove strip elements if new device has no strip support.
	if (oldInfo.supportStrip and not newInfo.supportStrip) {
		vector<Storage::BoxButton*> toRemove;
		for (auto btn : *items)
			if (not btn->getData()->getValue(STRIPSIZE).empty())
				toRemove.push_back(btn);
		string deleted;
		for (auto btn : toRemove) {
			deleted += btn->getData()->getValue(NAME) + "\n";
			box->remove(*btn);
			items->remove(*btn);
		}
		if (not deleted.empty())
			Message::displayInfo(
				"Strip elements removed (new device does not support strips):\n" + deleted,
				familyToDialog.at(COLLECTION_DEVICES)
			);
	}

	// Convert positional RGB elements to scattered RGB if new device has no RGB layout.
	if (oldInfo.layoutRGB and not newInfo.layoutRGB) {
		for (auto btn : *items) {
			auto data {btn->getData()};
			Storage::Element::splitRGB(data);
			data->unSet(POSITION);
			data->unSet(POSITIONS);
			data->unSet(COLORFORMAT);
		}
	}
}

void DialogElement::drawPins() noexcept {

	// Remove pins layout.
	for (auto child : pinsBox->get_children())
		pinsBox->remove(*child);

	if (not numberOfPins) return;

	std::vector<std::pair<string, string>> pinsUsage(numberOfPins, {NO_COLOR, ""});
	findConnectorTypes(pinsUsage);
	// Create pin labels.
	vector<Gtk::Label *> labels;
	labels.reserve(numberOfPins);
	for (uint16_t c = 1; c <= numberOfPins; ++c) {
		Gtk::Label* label(Gtk::make_managed<Gtk::Label>(std::to_string(c)));
		label->get_style_context()->add_class(CSS_PIN_LABEL);
		auto& pinUsage(pinsUsage[c - 1]);
		string labelTxt("Connector " + std::to_string(c));
		if (pinUsage.first.empty()) {
			labelTxt += " is unused";
		}
		else {
			labelTxt += string(" is used by element") + (pinUsage.first == CSS_COLOR_MULTIPLE ? "s " : " ");
			label->get_style_context()->add_class(std::move(pinUsage.first));
		}
		label->set_tooltip_text(labelTxt + std::move(pinUsage.second));
		labels.push_back(label);
	}
	// call hardware function
	if (Defaults::devicesInfo.at(comboBoxDevices->get_active_id()).layoutRGB) {
		drawPinsRGB(labels);
	}
	else {
		drawPins(labels);
	}
	pinsBox->show_all();
}

void DialogElement::drawPinsRGB(vector<Gtk::Label*>& labels) noexcept {
	uint16_t totalLeds(numberOfPins / 3), pin(0);
	// Get the best size for the box.
	findLargestDivisor(totalLeds);
	for (uint led = 1; led <= totalLeds; ++led) {
		// Create box.
		Gtk::VBox* vboxConnector = Gtk::manage(new Gtk::VBox(true));
		pinsBox->add(*vboxConnector);
		vboxConnector->set_name("boxRGB_" + std::to_string(led));
		vboxConnector->get_style_context()->add_class(CSS_BOX_CONNECTOR);
		vboxConnector->set_hexpand(false);
		vboxConnector->set_vexpand(false);
		vboxConnector->set_valign(Gtk::ALIGN_START);
		// Create box Label
		Gtk::Label* connectorLabel(Gtk::make_managed<Gtk::Label>("LED " + std::to_string(led)));
		vboxConnector->pack_start(*connectorLabel, Gtk::PACK_SHRINK);
		// Create container for pins
		Gtk::HBox* hboxPins = Gtk::manage(new Gtk::HBox(true, 1));
		vboxConnector->pack_start(*hboxPins, Gtk::PACK_SHRINK);
		for (uint l = 0; l < 3; ++l) {
			hboxPins->pack_start(*labels[pin++]);
		}
	}
	if (numberOfPins % 3 == 0) return;

	// Create box for extra pins.
	Gtk::VBox* vboxConnector = Gtk::manage(new Gtk::VBox(true));
	pinsBox->add(*vboxConnector);
	vboxConnector->get_style_context()->add_class(CSS_BOX_CONNECTOR);
	vboxConnector->set_hexpand(false);
	vboxConnector->set_vexpand(false);
	vboxConnector->set_valign(Gtk::ALIGN_START);
	// Create box Label
	Gtk::Label* connectorLabel(Gtk::make_managed<Gtk::Label>("Extra"));
	vboxConnector->pack_start(*connectorLabel, Gtk::PACK_SHRINK);
	// Create container for pins
	Gtk::HBox* hboxPins{Gtk::manage(new Gtk::HBox(true, 1))};
	vboxConnector->pack_start(*hboxPins, Gtk::PACK_SHRINK);
	for (; pin < numberOfPins; ++pin) {
		hboxPins->pack_start(*labels[pin]);
	}
}

void DialogElement::drawPins(vector<Gtk::Label *>& labels) noexcept {
	findLargestDivisor(numberOfPins);
	for (uint16_t pin = 0; pin < numberOfPins; ++pin) {
		pinsBox->add(*labels[pin]);
	}
}

LEDSpicerUI::Ui::Storage::Data* DialogElement::createData(StringUMap& rawData) const noexcept {
	return new Storage::Element(rawData);
}

void DialogElement::addButtons(Storage::BoxButton& boxButton) noexcept {
	createEditButton(boxButton);
	createCloneButton(boxButton);
	createDeleteButton(boxButton);
	boxButton.show_all();
}

void DialogElement::findConnectorTypes(vector<std::pair<string, string>>& pinsUsage) noexcept {

	std::function<void(std::pair<string, string>&, const string&)> storePins = [&](std::pair<string, string>& pair, const string& color) {
		if (pair.first == color or pair.first == NO_COLOR)
			pair.first = color;
		else
			pair.first = CSS_COLOR_MULTIPLE;
	};

	std::function<void(
		vector<std::pair<string, string>>&,
		const string&,
		const Storage::Data*
	)> storePinsRGB = [&](
		vector<std::pair<string, string>>& pinsList,
		const string& position,
		const Storage::Data* data
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
				storePins(pair, CSS_COLOR_RED);
				break;
			case 'G':
				storePins(pair, CSS_COLOR_GREEN);
				break;
			case 'B':
				storePins(pair, CSS_COLOR_BLUE);
			}
		}
	};

	for (const auto& boxButton : *items) {
		const auto data(boxButton->getData());
		// Multiple RGB.
		if (not data->getValue(POSITIONS).empty()) {
			for (const auto& position : Defaults::explode(data->getValue(POSITIONS), ',')) {
				storePinsRGB(pinsUsage, position, data);
			}
		}
		// LED strip.
		else if (not data->getValue(STRIPSIZE).empty()) {
			const uint16_t
				s(std::stoi(data->getValue(POSITION))),
				t(s + std::stoi(data->getValue(STRIPSIZE)));
			for (uint16_t c(s); c < t; ++c) {
				storePinsRGB(pinsUsage, std::to_string(c), data);
			}
		}
		// RGB.
		else if (not data->getValue(POSITION).empty()) {
			storePinsRGB(pinsUsage, data->getValue(POSITION), data);
		}
		// Single.
		else if (not data->getValue(PIN).empty()) {
			const uint16_t index(std::stoi(data->getValue(PIN)) - 1);
			storePins(pinsUsage[index], CSS_COLOR_PIN);
			pinsUsage[index].second = data->getValue(NAME);
		}
		// Solenoid.
		else if (not data->getValue(SOLENOID).empty()) {
			const uint16_t index(std::stoi(data->getValue(SOLENOID)) - 1);
			storePins(pinsUsage[index], CSS_COLOR_SOLENOID);
			pinsUsage[index].second = data->getValue(NAME);
		}
		// Scattered RGB
		else {
			// Red.
			uint16_t index(std::stoi(data->getValue(RED_PIN)) - 1);
			storePins(pinsUsage[index], CSS_COLOR_RED);
			pinsUsage[index].second = data->getValue(NAME);
			// Green.
			index = std::stoi(data->getValue(GREEN_PIN)) - 1;
			storePins(pinsUsage[index], CSS_COLOR_GREEN);
			pinsUsage[index].second = data->getValue(NAME);
			// Blue.
			index = std::stoi(data->getValue(BLUE_PIN)) - 1;
			storePins(pinsUsage[index], CSS_COLOR_BLUE);
			pinsUsage[index].second = data->getValue(NAME);
		}
	}
}

void DialogElement::findLargestDivisor(uint16_t size) noexcept {

	int bestDivisor = MAX_COLUMNS;
	// Track smallest remainder for non-perfect division
	int bestRemainder = size;

	// Find the largest divisor that evenly divides size or minimizes remainder
	for (int c = MAX_COLUMNS; c >= MIN_COLUMNS; --c) {
		int remainder = size % c;
		if (remainder == 0) {
			// Perfect division found
			bestDivisor = c;
			break;
		}
		else if (remainder < bestRemainder) {
			// Better fit than previous
			bestRemainder = remainder;
			bestDivisor = c;
		}
	}

	if (size < bestDivisor) {
		// Handle small sizes: cap bestDivisor at size if smaller
		bestDivisor = std::max(MIN_COLUMNS, static_cast<int>(size));
	}

	int minPerLine = std::max(MIN_COLUMNS, bestDivisor / 2);
	pinsBox->set_max_children_per_line(bestDivisor);
	pinsBox->set_min_children_per_line(minPerLine);
}

void DialogElement::findElementByPin(uint16_t finder, std::unordered_set<Storage::BoxButton*>& elementsFound) noexcept {
	string connector(std::to_string(finder));
	for (auto boxButton : *items) {
		auto data(boxButton->getData());
		// Single LED.
		string subject(data->getValue(PIN, data->getValue(SOLENOID)));
		if (not subject.empty()) {
			if (connector == subject)
				elementsFound.insert(boxButton);
			continue;
		}
		// Multi LED.
		subject = data->getValue(POSITIONS);
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
		subject = data->getValue(POSITION);
		if (not data->getValue(STRIPSIZE).empty()) {
			uint
				firstPin(Storage::Element::findFirstConnectorIndexByPosition(subject)),
				lastPin{firstPin + std::stoi(data->getValue(STRIPSIZE)) * 3 - 1};

			if (finder >= firstPin and finder <= lastPin) {
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
		subject = data->getValue(RED_PIN);
		if (not subject.empty()) {
			uint16_t pinNum(std::stoi(subject));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
			pinNum = std::stoi(data->getValue(GREEN_PIN));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
			pinNum = std::stoi(data->getValue(BLUE_PIN));
			if (finder == pinNum) {
				elementsFound.insert(boxButton);
				continue;
			}
		}
	}
}

void DialogElement::onSwitchPage(Gtk::Widget*, uint pageNum) noexcept {
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
	clearFormConditinal(ALL_TAB_IDX & ~exclude);
};

void DialogElement::onCloneClicked(Storage::BoxButton& boxButton) noexcept {

	// Clone started, set mode, clear form.
	action = Actions::ADD;
	clearForm();

	// Clone Data, will return a copy with a different and unique ID.
	StringUMap values{boxButton.getData()->copyValues()};

	// Create new element from cloned values and populate widgets.
	currentData = createData(values);
	retrieveData();
	currentData->wipe();
	storeData();

	// Create button, will set tracker if applicable.
	Storage::BoxButton& newBoxButton{items->create(currentData)};
	Defaults::markDirty();

	// Update UI and call after create callback.
	addButtons(newBoxButton);
	box->add(newBoxButton);
	afterCreate(newBoxButton);
	currentData = nullptr;
}
