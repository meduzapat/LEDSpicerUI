/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.cpp
 * @since     Feb 14, 2023
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

#include "DialogInput.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInput::DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder),
	dialogImportInput(DialogImport::Types::INPUT, this)
{

	//	DataDialogs::DialogInputLinkMaps::buildInstance(builder);
	DataDialogs::DialogInputSource::buildInstance(builder, "DialogInputSource");

	childDialogs.push_back(DialogInputSource::getInstance());

	Gtk::Button
		* btnAddInputSource = nullptr, // Only to hide/show when source is selected.
		* btnAddInputMap    = nullptr, // Only to hide/show when source is selected.
		* btnAddInput       = nullptr; // Form shooter.

	builder->get_widget_derived("BoxInputs",          box);
	builder->get_widget("BtnAddInput",                btnAddInput);
	builder->get_widget("BtnApplyInput",              btnApply);
	builder->get_widget("BtnAddInputMap",             btnAddInputMap);
	builder->get_widget("BtnAddInputSource",          btnAddInputSource);
	builder->get_widget("ComboBoxInputSelectInput",   comboBoxInputSelectInput);
	builder->get_widget("ComboBoxInputSpeed",         comboBoxInputSpeed);
	builder->get_widget("EntryInputName",             entryInputName);
	builder->get_widget("SwitchInputBlink",           switchInputBlink);
	builder->get_widget("SpinInputTimes",             spinInputTimes);
	builder->get_widget("BoxInputSourcesBox",         boxInputSourcesBox);
	builder->get_widget("BoxInputCreditsSettings",    boxInputCreditsSettings);
	builder->get_widget_derived("BoxInputLinkedMaps", boxInputLinkedMaps);

	setSignalAdd(btnAddInput);
	setSignalApply();

	// Populate input types.
	comboBoxInputSelectInput->append("", "Select Input Type");
	for (const auto& [id, info] : Defaults::inputsInfo)
		comboBoxInputSelectInput->append(id, info.name);
	comboBoxInputSelectInput->set_active(0);

	// Show/hide plugin-specific options when plugin type changes.
	comboBoxInputSelectInput->signal_changed().connect([this, btnAddInputSource, btnAddInputMap]() {
		string name(comboBoxInputSelectInput->get_active_id());
		bool valid = not name.empty();
		btnAddInputSource->set_sensitive(valid);
		boxInputSourcesBox->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE));
		boxInputLinkedMaps->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_LINKED_MAPS));
		btnAddInputMap->set_visible(not Defaults::inputHasFlag(name, Defaults::INPUT_NEEDS_SOURCE));
		btnAddInputMap->set_sensitive(valid);
		btnApply->set_sensitive(valid);
		comboBoxInputSpeed->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_SPEED));
		spinInputTimes->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_TIMES));
		switchInputBlink->get_parent()->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_BLINK));
		boxInputCreditsSettings->set_visible(Defaults::inputHasFlag(name, Defaults::INPUT_HAS_CREDITS));
	});

	// Import button.
	Gtk::Button* btnImportInput = nullptr;
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

DialogInput::~DialogInput() {
	//	delete DataDialogs::DialogInputLinkMaps::getInstance();
	delete DataDialogs::DialogInputSource::getInstance();
}

void DialogInput::load(XMLHelper* values) {
	createItems(values->getData(COLLECTION_INPUT), values);
}

void DialogInput::createSubItems(XMLHelper* values) {
	DialogInputSource::getInstance()->load(values);
//	DialogInputLinkMaps::getInstance()->load(values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInput::getCollectionHandler() const {
	return Storage::CollectionHandler::getInstance(COLLECTION_INPUT);
}

void DialogInput::resetForm() {
	comboBoxInputSelectInput->set_active(0);
	clearForm();
}

void DialogInput::clearForm() {
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
	btnApply->set_sensitive(false);
}

void DialogInput::isValid() const {
	string filename(entryInputName->get_text());
	if (filename.empty()) {
		if (action != Actions::LOAD)
			entryInputName->grab_focus();
		throw Message("Invalid name.");
	}

	string uid(createUniqueId());
	if (getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or uid != currentData->createUniqueId()) {
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
	string name(comboBoxInputSelectInput->get_active_id());
	currentData->setValue(NAME, name);
	currentData->setProperty(PATH,     currentPath);
	currentData->setProperty(FILENAME, entryInputName->get_text());

	if (name == "Actions") {
		currentData->setValue("blink", switchInputBlink->get_active() ? "true" : "false");
	}
	if (name == "Blinker") {
		currentData->setValue("times", spinInputTimes->get_text());
	}
	if (name == "Actions" or name == "Blinker") {
		currentData->setValue("speed", comboBoxInputSpeed->get_active_text());
	}
}

void DialogInput::retrieveData() {
	string name(currentData->getValue(NAME));
	comboBoxInputSelectInput->set_active_id(name);
	entryInputName->set_text(currentData->getProperty(FILENAME));

	if (name == "Actions") {
		switchInputBlink->set_active(currentData->getValue("blink") == "true");
	}
	if (name == "Blinker") {
		spinInputTimes->set_text(currentData->getValue("times"));
	}
	if (name == "Actions" or name == "Blinker") {
		comboBoxInputSpeed->set_active_text(currentData->getValue("speed"));
	}
}

const string DialogInput::createUniqueId() const {
	string filename(entryInputName->get_text());
	return currentPath.empty() ? filename : currentPath + "/" + filename;
}

void DialogInput::setPath(const string& path) {
	currentPath = path;
}

const string DialogInput::getType() const {
	return "input";
}

LEDSpicerUI::Ui::Storage::Data* DialogInput::createData(StringUMap& rawData) {
	// Inject PATH and FILENAME as separate keys so Input constructor
	// can store them as properties without parsing.
	rawData[PATH]     = currentPath;
	rawData[FILENAME] = rawData.count(FILENAME) ? rawData.at(FILENAME) : "";
	return new Storage::Input(rawData);
}
