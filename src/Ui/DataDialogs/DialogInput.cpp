/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDialog.cpp
 * @since     Feb 14, 2023
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

#include "DialogInput.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInput* DialogInput::instance = nullptr;

void DialogInput::initialize(Glib::RefPtr<Gtk::Builder> const& builder) {
	if (not instance) {
		builder->get_widget_derived("DialogInput", instance);
	}
}

DialogInput* DialogInput::getInstance() {
	return instance;
}

DialogInput::DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder),
	dialogImportInput(DialogImport::Types::INPUT, this)
{

	DialogInputSource::initialize(builder);

	// Connect list box and buttons.
	builder->get_widget_derived("BoxInputs", box);
	builder->get_widget("BtnApplyInput",     btnApply);

	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddInput", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	// Generic.
	builder->get_widget("ComboBoxInputSelectInput", comboBoxInputSelectInput);
	builder->get_widget("EntryInputName",           entryInputName);

	// Actions.
	builder->get_widget("SwitchInputBlink", switchInputBlink);

	// Necessary to select and sort mappings.
	builder->get_widget_derived("BoxInputMaps",       boxInputMaps);
	builder->get_widget_derived("BoxInputLinkedMaps", boxInputLinkedMaps);

	// Blinker needs this.
	builder->get_widget("SpinInputTimes", spinInputTimes);

	// Actions and Blinker share speed.
	builder->get_widget("ComboBoxInputSpeed", comboBoxInputSpeed);

	// Conditional sections.
	builder->get_widget("BoxLinkedElementsAndGroups", boxLinkedElementsAndGroupsBox);
	builder->get_widget("BoxInputSourcesBox",         boxInputSourcesBox);
	builder->get_widget("BoxInputCreditsSettings",    boxInputCreditsSettings);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);

	// When blink is off, disable speed.
	switchInputBlink->signal_state_changed().connect([&](bool) {
		comboBoxInputSpeed->set_sensitive(switchInputBlink->get_state());
	});

	// When a plugin is selected, set field visibility rules.
	comboBoxInputSelectInput->signal_changed().connect([&]() {

		string name(comboBoxInputSelectInput->get_active_text());

		if (name == "Select Plugin") {
			btnApply->set_sensitive(false);
			btnAddInputMap->set_sensitive(false);
			return;
		}

		clearForm();
		btnApply->set_sensitive(true);
		btnAddInputMap->set_sensitive(true);

		if (name == "Actions" or name == "Credits") {
			boxLinkedElementsAndGroupsBox->show();
			boxInputMaps->set_selection_mode(Gtk::SelectionMode::SELECTION_MULTIPLE);
		}

		if (name == "Actions") {
			comboBoxInputSpeed->get_parent()->show();
			switchInputBlink->get_parent()->show();
			switchInputBlink->set_active(true);
		}

		if (name == "Blinker") {
			spinInputTimes->get_parent()->show();
			comboBoxInputSpeed->get_parent()->show();
		}

		if (name == "Credits") {
			boxInputCreditsSettings->show();
			comboBoxInputSpeed->get_parent()->show();
		}

		if (
			name == "Actions" or
			name == "Blinker" or
			name == "Impulse" or
			name == "Credits"
		) {
			boxInputSourcesBox->show();
			btnAddInputMap->hide();
		}
		else {
			// Single-source plugins: Mame, Network, etc.
			boxInputSourcesBox->hide();
			btnAddInputMap->show();
		}
	});

	// Dialog to import input plugin files.
	Gtk::Button* btnImportInput;
	builder->get_widget("BtnImportInput", btnImportInput);
	btnImportInput->signal_clicked().connect([this]() {
		if (dialogImportInput.run() == Gtk::ResponseType::RESPONSE_OK) {
			StringVector selectedFiles(dialogImportInput.get_filenames());
			for (const auto& selectedFile : selectedFiles) {
				try {
					InputFile datafile(selectedFile);
					load(&datafile);
				}
				catch (Message& e) {
					Message::displayError(XMLHelper::cleanError(e.getMessage()));
				}
			}
		}
		dialogImportInput.hide();
	});
}

void DialogInput::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_INPUT), values);
}

void DialogInput::createSubItems(XMLHelper* values) {
	DialogInputMap::getInstance()->load(values);
	DialogInputLinkMaps::getInstance()->load(values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInput::getCollectionHandler() const {
	return LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_INPUT);
}

void DialogInput::resetForm() {
	comboBoxInputSelectInput->set_active(0);
	clearForm();
}

void DialogInput::clearForm() {

	boxInputMaps->set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);

	boxLinkedElementsAndGroupsBox->hide();
	boxInputSourcesBox->hide();
	boxInputLinkedMaps->hide();
	boxInputCreditsSettings->hide();
	entryInputName->set_text("");

	comboBoxInputSpeed->get_parent()->hide();
	comboBoxInputSpeed->set_active_text("Normal");

	spinInputTimes->get_parent()->hide();
	spinInputTimes->set_text("");

	switchInputBlink->get_parent()->hide();
	switchInputBlink->set_active(false);
}

void DialogInput::isValid() const {

	string filename(createUniqueId());
	if (filename.empty()) {
		if (action != Actions::LOAD)
			entryInputName->grab_focus();
		throw Message("Invalid name.");
	}

	if (getCollectionHandler()->isIdSet(filename)) {
		if (action != Actions::EDIT or filename != currentData->createUniqueId()) {
			if (action != Actions::LOAD)
				entryInputName->grab_focus();
			throw Message("Name already in use.");
		}
	}

	if (comboBoxInputSelectInput->get_active_text() == "Blinker") {
		auto blinks(spinInputTimes->get_value_as_int());
		if (blinks < 0 or blinks > 255)
			spinInputTimes->set_value(0);
	}
}

void DialogInput::storeData() {

	string name(comboBoxInputSelectInput->get_active_text());
	currentData->setValue(NAME, name);

	currentData->setProperty(PATH,     createUniqueId());
	currentData->setProperty(FILENAME, entryInputName->get_text());

	if (name == "Actions") {
		currentData->setValue("blink", switchInputBlink->get_state() ? "true" : "false");
		currentData->setValue("speed", comboBoxInputSpeed->get_active_text());
	}

	if (name == "Blinker") {
		currentData->setValue("times", spinInputTimes->get_text());
		currentData->setValue("speed", comboBoxInputSpeed->get_active_text());
	}

	if (name == "Credits") {
		currentData->setValue("speed", comboBoxInputSpeed->get_active_text());
	}
}

void DialogInput::retrieveData() {

	string name(currentData->getValue(NAME));
	comboBoxInputSelectInput->set_active_text(name);
	entryInputName->set_text(currentData->getValue(FILENAME));

	if (name == "Actions") {
		switchInputBlink->set_active(currentData->getValue("blink") == "true");
		comboBoxInputSpeed->set_active_text(currentData->getValue("speed"));
	}

	if (name == "Blinker") {
		spinInputTimes->set_text(currentData->getValue("times"));
		comboBoxInputSpeed->set_active_text(currentData->getValue("speed"));
	}

	if (name == "Credits") {
		comboBoxInputSpeed->set_active_text(currentData->getValue("speed"));
	}
}

string const DialogInput::createUniqueId() const {
	return Defaults::createCommonUniqueId({currentPath, entryInputName->get_text()});
}

void DialogInput::setPath(const string& path) {
	currentPath = path;
}

const string DialogInput::getType() const {
	return "input";
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::createData(StringUMap& rawData) {
	return new Storage::Input(rawData);
}

