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

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogRestrictor::DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogFormHost(obj, builder, COLLECTION_RESTRICTORS)
{

	DataDialogs::DialogRestrictorMap::buildInstance(builder, "DialogRestrictorMap");
	childDialogs.push_back(DialogRestrictorMap::getInstance());

	// Connect Restrictor Box and buttons.
	builder->get_widget_derived("BoxRestrictors", box);
	builder->get_widget("BtnApplyRestrictors",    btnApply);
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
	builder->get_widget("BtnAddRestrictorMap",          btnAddRestrictorMap);


	// Populate Restrictors
	initializeSelector(noRestrictor, Defaults::restrictorsInfo);

	// Models.
	idListstore = static_cast<Gtk::ListStore*>(builder->get_object("liststoreRestrictorsId").get());

	// Restrictor Icons.
	for (auto& w : Defaults::wayIds) {
		Gtk::FlowBoxChild* i;
		builder->get_widget(w.first, i);
		i->hide();
		waysIcons.emplace(w.second, i);
	}

	selectorCombo->signal_changed().connect([this]() {
		if (handleTypeSwitch(
			DialogRestrictorMap::getInstance()->getBox(),
			"Are you sure you want to change the restrictor? All mappings will be lost.")
		) {
			resetForm();
		}
	});
}

DialogRestrictor::~DialogRestrictor() {
	delete DataDialogs::DialogRestrictorMap::getInstance();
}

void DialogRestrictor::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_RESTRICTORS), values);
}

void DialogRestrictor::createSubItems(XMLHelper* values) {
	DialogRestrictorMap::getInstance()->load(values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogRestrictor::getCollectionHandler() const {
	return LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_RESTRICTORS);
}

void DialogRestrictor::resetForm() {

	const string name = selectorCombo->get_active_id();

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

	brief->set_label(Defaults::restrictorsInfo.at(name).brief.data());
	btnApply->set_sensitive(true);
	// Disable add interface button if there no more interfaces left.
	btnAddRestrictorMap->set_sensitive(
		DialogRestrictorMap::getInstance()->getValues().size() < Defaults::restrictorsInfo.at(selectorCombo->get_active_id()).interfaces
	);
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
	if (action == Actions::EDIT) {
		checkDupe = (currentData->createUniqueId() != newName);
	}

	if (Defaults::isIdUser(name, false) and checkDupe and getCollectionHandler()->isIdSet(newName)) {
		throw Message(hardwareName + " ID " + id + " already exists.");
	}
	if (Defaults::isSerial(name, false) and checkDupe and getCollectionHandler()->isIdSet(newName)) {
		throw Message(hardwareName + " that connects to " + (port.empty() ? "<autodetect>" : port) + " already exists.");
	}
	if (checkDupe and getCollectionHandler()->isIdSet(newName)) {
		throw Message(hardwareName + " already exists.");
	}
}

void DialogRestrictor::storeData() {

	const string name(selectorCombo->get_active_id());

	currentData->setValue(NAME, name);
	if (Defaults::isIdUser(name, false)) {
		currentData->setValue(ID, comboBoxId->get_active_id());
	}

	if (Defaults::isSerial(name, false)) {
		currentData->setValue(PORT, serialPort->get_text());
	}

	if (name == "UltraStik360") {
		currentData->setValue(US360_HAS_RESTRICTOR, hasRestrictor->get_active() ? "true" : "false");
		currentData->setValue(US360_USE_MOUSE,      handleMouse->get_active()   ? "true" : "false");
	}
	else if (name == "GPWiz49") {
		currentData->setValue(GZ49_WILLIAMS, williamsMode->get_active() ? "true" : "false");
	}
	else if (name == "GPWiz40RotoX") {
		currentData->setValue(GZ40_SPEED_ON, std::to_string(static_cast<uint8_t>(speedOn->get_value())));
		currentData->setValue(GZ40_SPEED_OFF, std::to_string(static_cast<uint8_t>(speedOff->get_value())));
	}
}

void DialogRestrictor::retrieveData() {

	const string name(currentData->getValue(NAME));

	selectorCombo->set_active_id(name);

	if (Defaults::isIdUser(name, false)) {
		comboBoxId->set_active_id(currentData->getValue(ID));
	}
	if (Defaults::isSerial(name, false)) {
		serialPort->set_text(currentData->getValue(PORT));
	}

	if (name == "UltraStik360") {
		hasRestrictor->set_active(currentData->getValue(US360_HAS_RESTRICTOR) == "true");
		handleMouse->set_active(currentData->getValue(US360_USE_MOUSE) == "true");
	}
	else if (name == "GPWiz49") {
		williamsMode->set_active(currentData->getValue(GZ49_WILLIAMS) == "true");
	}
	else if (name == "GPWiz40RotoX") {
		speedOn->set_value(std::stod(currentData->getValue(GZ40_SPEED_ON, std::to_string(GZ40_DEFAULT_SPEED))));
		speedOff->set_value(std::stod(currentData->getValue(GZ40_SPEED_OFF, std::to_string(GZ40_DEFAULT_SPEED))));
	}

	markUsed([this](const string& id) {
		return getCollectionHandler()->countByKey(NAME, id) < Defaults::restrictorsInfo.at(id).maxIds;
	});
}

string const DialogRestrictor::createUniqueId() const {
	return Defaults::createHardwareUniqueId({
		{NAME, selectorCombo->get_active_id()},
		{ID,   comboBoxId->get_active_id()},
		{PORT, serialPort->get_text()}
	}, false);
}

string_view DialogRestrictor::getType() const noexcept {
	return TYPE_RESTRICTOR;
}

LEDSpicerUI::Ui::Storage::Data* DialogRestrictor::createData(StringUMap& rawData) noexcept {
	return new Storage::Restrictor(rawData);
}

void DialogRestrictor::onEmpty() {
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

	for (auto& w : Defaults::allWays) {
		waysIcons.at(w)->hide();
	}
	brief->set_label("");
	btnApply->set_sensitive(false);
}

void DialogRestrictor::onSelected() {
	const string name = selectorCombo->get_active_id();
	if (Defaults::isIdUser(name, false)) {
		Defaults::populateComboBoxWithIds(
			idListstore,
			Defaults::restrictorsInfo.at(name).maxIds,
			[=](const string& id) {
				return getCollectionHandler()->isIdSet(Defaults::createHardwareUniqueId({{NAME, name}, {ID, id}}, false));
			},
			"Restrictor Number",
			"Hardware #"
		);
	}
	DialogRestrictorMap::getInstance()->populateInterfacesCombobox();
}
