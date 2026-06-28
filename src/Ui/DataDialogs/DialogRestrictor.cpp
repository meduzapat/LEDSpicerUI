/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.cpp
 * @since     Apr 30, 2023
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

#include "DialogRestrictor.hpp"
#include "config/Settings.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::StringVector;

const std::unordered_map<string, Defaults::Ways> DialogRestrictor::wayTokens {
	{"2",         Defaults::Ways::w2},
	{"vertical2", Defaults::Ways::w2v},
	{"4",         Defaults::Ways::w4},
	{"4x",        Defaults::Ways::w4x},
	{"8",         Defaults::Ways::w8},
	{"16",        Defaults::Ways::w16},
	{"49",        Defaults::Ways::w49},
	{"analog",    Defaults::Ways::analog},
	{"mouse",     Defaults::Ways::mouse},
	{"rotary8",   Defaults::Ways::rotary8},
	{"rotary12",  Defaults::Ways::rotary12},
};

const string& DialogRestrictor::wayToToken(Defaults::Ways way) noexcept {
	for (const auto& [token, w] : wayTokens)
		if (w == way)
			return token;
	return emptyString;
}

StringVector DialogRestrictor::buildRotatorArgs(
	const std::vector<std::pair<string, string>>& mappings,
	Defaults::Ways way
) noexcept {
	StringVector args;
	args.reserve(mappings.size() * 3);
	const string& token {wayToToken(way)};
	for (const auto& [player, joystick] : mappings) {
		args.push_back(player);
		args.push_back(joystick);
		args.push_back(token);
	}
	return args;
}

DialogRestrictor::DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept:
	DialogFormHost(obj, builder)
{

	registerChildDialog<DialogRestrictorMap>(builder, "DialogRestrictorMap", COLLECTION_RESTRICTOR_MAPS);
	DialogRestrictorMap::getInstance()->setOnMapsChanged([this]() { updateWaysTestState(); });

	// Register self so child dialogs can resolve the visible host window via familyToDialog.
	familyToDialog.emplace(COLLECTION_RESTRICTORS, this);

	// Connect Restrictor Box and buttons.
	builder->get_widget_derived("BoxRestrictors", box);

	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddRestrictor", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Restrictor fields.
	builder->get_widget("ComboBoxRestrictors",          selectorCombo);
	builder->get_widget("ComboBoxRestrictorId",         comboBoxId);
	builder->get_widget("InputRestrictorPort",          serialPort);
	builder->get_widget("InputRestrictorHasRestrictor", hasRestrictor);
	builder->get_widget("InputRestrictorHandleMouse",   handleMouse);
	builder->get_widget("InputRestrictorWilliamsMode",  williamsMode);
	builder->get_widget("InputRestrictorSpeedOn",       speedOn);
	builder->get_widget("InputRestrictorSpeedOff",      speedOff);
	builder->get_widget("BriefRestrictor",              brief);
	builder->get_widget("LabelRestrictorWays",          waysLabel);
	builder->get_widget("LabelRestrictorMappings",      mappingsLabel);
	builder->get_widget("BtnAddRestrictorMap",          btnAddRestrictorMap);


	// Populate Restrictors
	initializeSelector(noRestrictor, Defaults::restrictorsInfo);

	// Models.
	idListstore = static_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictorsId").get());

	// Restrictor Icons.
	for (auto& w : wayTokens) {
		Gtk::FlowBoxChild* i;
		builder->get_widget(w.first, i);
		i->hide();
		waysIcons.emplace(w.second, i);
	}

	// Way icons are also the test toggle group (single active, none allowed).
	builder->get_widget("FlowboxWays", flowboxWays);
	flowboxWays->signal_child_activated().connect(
		sigc::mem_fun(*this, &DialogRestrictor::onWayActivated)
	);

	selectorCombo->signal_changed().connect([this]() {
		string newName{selectorCombo->get_active_id()};
		string msg;
		if (not newName.empty() and not previousName.empty()) {
			const auto& newInfo = Defaults::restrictorsInfo.at(newName);
			const auto& oldInfo = Defaults::restrictorsInfo.at(previousName);
			msg = "Are you sure you want to convert \"" + oldInfo.name + "\" into \"" + newInfo.name + "\"?";
			if (newInfo.interfaces < oldInfo.interfaces)
				msg += "\nPlayer profiles above " + std::to_string(newInfo.interfaces) + " will be removed.";
		}
		if (handleTypeSwitch(DialogRestrictorMap::getInstance()->getBox(), msg)) {
			resetForm();
		}
	});
}

void DialogRestrictor::load(DataMap& values) noexcept {
	createItems(values[COLLECTION_RESTRICTORS], values);
}

void DialogRestrictor::createSubItems(DataMap& values) noexcept {
	DialogRestrictorMap::getInstance()->load(values);
}

void DialogRestrictor::resetForm() noexcept {

	string name = selectorCombo->get_active_id();

	if (name == "UltraStik360") {
		hasRestrictor->get_parent()->show();
	}
	if (name == "GPWiz49") {
		williamsMode->show();
	}
	if (name == "GPWiz40RotoX") {
		speedOn->get_parent()->get_parent()->show();
	}

	if (Defaults::isIdUser(name, false)) {
		comboBoxId->get_parent()->show();
	}

	if (Defaults::isSerial(name, false)) {
		serialPort->get_parent()->show();
	}

	// Set ways.
	for (auto& w : Defaults::restrictorsInfo.at(name).ways) {
		waysIcons.at(w)->show();
	}

	brief->set_label(Defaults::restrictorsInfo.at(name).brief);
	btnApply->set_sensitive(true);
	// Disable add interface button if there no more interfaces left.
	btnAddRestrictorMap->set_sensitive(
		DialogRestrictorMap::getInstance()->getSize() < Defaults::restrictorsInfo.at(selectorCombo->get_active_id()).interfaces
	);
	updateWaysTestState();
	DialogForm::resetForm();
}

void DialogRestrictor::isValid() const {
	string
		name(selectorCombo->get_active_id()),
		id(comboBoxId->get_active_id()),
		port(serialPort->get_text());

	bool checkDupe = true;

	if (name.empty()) {
		throw Message("Invalid restrictor.");
	}

	if (Defaults::isIdUser(name, false) and id.empty()) {
		throw Message("Invalid restrictor number.");
	}

	string
		newName(createUniqueId()),
		hardwareName("Hardware ");
	hardwareName += Defaults::restrictorsInfo.at(name).name;

	auto ch{currentData->getCollectionHandler()};
	if (action == Actions::EDIT)
		checkDupe = (ch->get(newName) != currentData);


	if (Defaults::isIdUser(name, false) and checkDupe and ch->isIdSet(newName)) {
		throw Message(hardwareName + " ID " + id + " already exists.");
	}
	if (Defaults::isSerial(name, false) and checkDupe and ch->isIdSet(newName)) {
		throw Message(hardwareName + " that connects to " + (port.empty() ? "<autodetect>" : port) + " already exists.");
	}
	if (checkDupe and ch->isIdSet(newName)) {
		throw Message(hardwareName + " already exists.");
	}

//	if (action != Actions::LOAD and not DialogRestrictorMap::getInstance()->getBox()->getSize())
//		throw Message("Add at least one player mapping.");

}

void DialogRestrictor::storeData() noexcept {

	string name(selectorCombo->get_active_id());

	currentData->setValue(NAME, name);
	if (Defaults::isIdUser(name, false)) {
		currentData->setValue(ID, comboBoxId->get_active_id());
	}

	if (Defaults::isSerial(name, false)) {
		currentData->setValue(PORT, serialPort->get_text());
	}

	if (name == "UltraStik360") {
		currentData->setValue(US360_HAS_RESTRICTOR, hasRestrictor->get_active());
		currentData->setValue(US360_USE_MOUSE,      handleMouse->get_active());
	}
	else if (name == "GPWiz49") {
		currentData->setValue(GZ49_WILLIAMS, williamsMode->get_active());
	}
	else if (name == "GPWiz40RotoX") {
		currentData->setValue(GZ40_SPEED_ON,  speedOn->get_value_as_int());
		currentData->setValue(GZ40_SPEED_OFF, speedOff->get_value_as_int());
	}
}

void DialogRestrictor::retrieveData() noexcept {

	string name(currentData->getValue(NAME));

	selectorCombo->set_active_id(name);

	if (Defaults::isIdUser(name, false)) {
		comboBoxId->set_active_id(currentData->getValue(ID));
	}
	if (Defaults::isSerial(name, false)) {
		serialPort->set_text(currentData->getValue(PORT));
	}

	if (name == "UltraStik360") {
		hasRestrictor->set_active(currentData->is(US360_HAS_RESTRICTOR));
		handleMouse->set_active(currentData->is(US360_USE_MOUSE));
	}
	else if (name == "GPWiz49") {
		williamsMode->set_active(currentData->is(GZ49_WILLIAMS));
	}
	else if (name == "GPWiz40RotoX") {
		speedOn->set_value(currentData->getInt(GZ40_SPEED_ON));
		speedOff->set_value(currentData->getInt(GZ40_SPEED_OFF));
	}

	markUsed([this](const string& id) {
		return currentData->getCollectionHandler()->countByKey(NAME, id) < Defaults::restrictorsInfo.at(id).maxIds;
	});
}

string DialogRestrictor::createUniqueId() const noexcept {
	return Defaults::createHardwareUniqueId({
		{NAME, selectorCombo->get_active_id()},
		{ID,   comboBoxId->get_active_id()},
		{PORT, serialPort->get_text()}
	}, false);
}

LEDSpicerUI::Ui::Storage::Data* DialogRestrictor::createData(Values& rawData) const noexcept {
	return new Storage::Restrictor(rawData);
}

void DialogRestrictor::onEmpty() noexcept {
	btnAddRestrictorMap->set_sensitive(false);
	comboBoxId->get_parent()->hide();
	comboBoxId->set_active_id("");
	serialPort->get_parent()->hide();
	serialPort->set_text("");

	// UltraStik360
	hasRestrictor->get_parent()->hide();
	hasRestrictor->set_active(false);
	handleMouse->set_active(false);

	// GPWiz49
	williamsMode->hide();
	williamsMode->set_active(false);

	// GPWiz40RotoX
	speedOn->get_parent()->get_parent()->hide();
	speedOn->set_value(GZ40_DEFAULT_SPEED);
	speedOff->set_value(GZ40_DEFAULT_SPEED);

	for (auto& [way, child] : waysIcons) {
		child->hide();
	}
	brief->set_label("");
	btnApply->set_sensitive(false);
}

void DialogRestrictor::onSelected() noexcept {
	const string& name {selectorCombo->get_active_id()};
	const auto isUsed {[=](const string& id) {
		return currentData->getCollectionHandler()->isIdSet(Defaults::createHardwareUniqueId({{NAME, name}, {ID, id}}, false));
	}};
	const bool isIdUser {Defaults::isIdUser(name, false)};
	if (isIdUser) {
		Defaults::populateComboBoxWithIds(
			idListstore,
			Defaults::restrictorsInfo.at(name).maxIds,
			isUsed,
			"Restrictor Number",
			"Hardware #"
		);
	}
	DialogRestrictorMap::getInstance()->populateInterfacesCombobox();

	if (isIdUser)
		Defaults::selectFirstAvailableId(comboBoxId, Defaults::restrictorsInfo.at(name).maxIds, isUsed);

	const uint8_t newInterfaces {Defaults::restrictorsInfo.at(name).interfaces};
	if (Defaults::restrictorsInfo.at(previousName).interfaces > newInterfaces)
		DialogRestrictorMap::getInstance()->trimToInterfaces(newInterfaces);
}

void DialogRestrictor::updateWaysTestState() noexcept {
	const bool live {
		Config::Settings::get().isInteractive()
		and testLive and testLive()
		and DialogRestrictorMap::getInstance()->getSize() > 0
	};
	selectedWay = nullptr;
	flowboxWays->unselect_all();
	flowboxWays->set_selection_mode(live ? Gtk::SELECTION_SINGLE : Gtk::SELECTION_NONE);
	flowboxWays->set_activate_on_single_click(live);
	// Portable: informational. Otherwise sensitive only when live.
	flowboxWays->set_sensitive(live or Config::Settings::get().isPortable());

	// Narrowing: multi-interface restrictor with 2+ mappings, while live.
	const string name {selectorCombo->get_active_id()};
	const bool multi {
		live
		and Defaults::isMulti(name)
		and DialogRestrictorMap::getInstance()->getSize() >= 2
	};
	auto mapsBox {DialogRestrictorMap::getInstance()->getBox()};
	mapsBox->unselect_all();
	mapsBox->set_selection_mode(multi ? Gtk::SELECTION_MULTIPLE : Gtk::SELECTION_NONE);

	// Selection mode re-enables child focus; these groups are mouse-driven.
	for (auto child : flowboxWays->get_children())
		child->set_can_focus(false);
	for (auto child : mapsBox->get_children())
		child->set_can_focus(false);

	waysLabel->set_text(live
		? "Click a direction or rotation to test the hardware"
		: "Supported directions and rotations");
	mappingsLabel->set_text(multi
		? "Player mappings — select rows to scope the test (none tests all)"
		: "Player mappings");
}

void DialogRestrictor::onWayActivated(Gtk::FlowBoxChild* child) noexcept {
	if (rotatorRunning)
		return;
	// Re-click clears the toggle; sends nothing.
	if (child == selectedWay) {
		flowboxWays->unselect_all();
		selectedWay = nullptr;
		return;
	}
	flowboxWays->select_child(*child);
	selectedWay = child;

	if (not rotatorRunner)
		return;

	// Resolve the toggled way from its icon.
	Defaults::Ways way {Defaults::Ways::invalid};
	for (const auto& [w, icon] : waysIcons)
		if (icon == child) {
			way = w;
			break;
		}

	// Scope: selected mappings, else all (none == all).
	std::vector<std::pair<string, string>> mappings;
	const auto selected {DialogRestrictorMap::getInstance()->getBox()->get_selected_children()};
	if (not selected.empty())
		for (auto child : selected) {
			auto data {static_cast<Storage::BoxButton*>(child)->getData()};
			mappings.emplace_back(data->getValue(PLAYER), data->getValue(JOYSTICK));
		}
	else if (auto maps {getChildCollection(COLLECTION_RESTRICTOR_MAPS)})
		for (auto bb : *maps)
			mappings.emplace_back(bb->getData()->getValue(PLAYER), bb->getData()->getValue(JOYSTICK));

	rotatorRunning = true;
	string output;
	const bool ok {rotatorRunner(buildRotatorArgs(mappings, way), output)};
	rotatorRunning = false;

	if (ok) {
		if (Config::Settings::get().shouldDebugHardwareTest() and not output.empty())
			StatusBar::getInstance().push(output, StatusBar::Severity::Debug);
	}
	else if (not output.empty())
		StatusBar::getInstance().push(output, StatusBar::Severity::Error);
}
