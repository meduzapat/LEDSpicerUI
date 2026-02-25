/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.cpp
 * @since     Feb 13, 2023
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

#include "MainWindow.hpp"

using namespace LEDSpicerUI::Ui;

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::ApplicationWindow(obj),
	// Set import dialog
	dialogImportConfig(DialogImport::Types::CONFIG, this),
	inputNavigator(builder)
{

	Message::initialize(builder, this);

	DialogSettings::buildInstance(builder, "DialogSettings");
	DialogColors::buildInstance(builder,   "DialogColors");
	DialogProject::buildInstance(builder,  "DialogProject");

	// Initialize secondary dialogs.
	DataDialogs::DialogSelect::buildInstance(builder, "DialogSelect");

	// Initialize Primary Dialogs.
	DataDialogs::DialogDevice::buildInstance(builder, "DialogDevice");
	DataDialogs::DialogRestrictor::buildInstance(builder, "DialogRestrictor");
	DataDialogs::DialogProcess::buildInstance(builder, "DialogProcess");
	DataDialogs::DialogGroup::buildInstance(builder, "DialogGroup");
	DataDialogs::DialogProfile::buildInstance(builder, "DialogProfile");

	// Connect primary dialogs with the collections.
	DataDialogs::DialogDevice::getInstance()->setOwner(&devices);
	DataDialogs::DialogRestrictor::getInstance()->setOwner(&restrictors);
	DataDialogs::DialogProcess::getInstance()->setOwner(&processes);
	DataDialogs::DialogGroup::getInstance()->setOwner(&groups);
	DataDialogs::DialogProfile::getInstance()->setOwner(&profiles);

	// Setup ledspicer fields.
	builder->get_widget("InputUserId",     inputUserId);
	builder->get_widget("InputGroupId",    inputGroupId);
	builder->get_widget("InputPortNumber", inputPortNumber);
	builder->get_widget("InputFPS",        inputFPS);
	builder->get_widget("ComboColors",     comboColors);
	builder->get_widget("ComboLogLevel",   comboLogLevel);

	// Register Widgets that changes the status to not saved.
	Defaults::registerWidget(inputUserId);
	Defaults::registerWidget(inputGroupId);
	Defaults::registerWidget(inputPortNumber);
	Defaults::registerWidget(inputFPS);
	Defaults::registerWidget(comboColors);
	Defaults::registerWidget(comboLogLevel);

	// Setup top buttons
	Gtk::Button
		* btnAbout,
		* btnSaveProject;

	builder->get_widget("BtnSaveProject", btnSaveProject);
	builder->get_widget("BtnAbout",       btnAbout);

	// Top directory information.
	Gtk::HeaderBar* header;
	builder->get_widget("Header", header);
	Defaults::initialize(header, btnSaveProject);

	Gtk::Fixed* FixedLayout;
	builder->get_widget("FixedLayout",  FixedLayout);

	// connect dialogs
	prepareDialogs(builder);

	// Save project
	btnSaveProject->signal_clicked().connect([this]() {
		try {
			if (boxRandomColors->get_children().size() == 1) {
				throw Message("The number of random colors need to be more than one or zero.");
			}

			ConfigFile::save(ConfigFile::ConfigData(
				DialogSettings::getInstance()->getConfigPath(),
				comboDefaultProfile->get_active_text().raw(),
				inputRunEvery->get_text().raw(),
				ledspicerConfigToXml(),
				devices,
				restrictors,
				groups,
				processes
			));

			Defaults::cleanDirty();
			Message::displayInfo("Project saved successfully.");
		}
		catch (Message& e) {
			e.displayError();
		}
		catch (const Glib::FileError& e) {
			Message::displayError("Unable to save project: " + string(e.what()));
		}
	});

	// Settings.
	Gtk::Button* btnSettings = nullptr;
	builder->get_widget("BtnSettings", btnSettings);
	btnSettings->signal_clicked().connect([this]() {
		if (DialogSettings::getInstance()->run() == Gtk::RESPONSE_APPLY) {
			DialogSettings::getInstance()->saveSettings();
			populateColorsCombo();
		}
		DialogSettings::getInstance()->hide();
	});

	// About.
	btnAbout->signal_clicked().connect([builder]() {
		Gtk::Dialog* d;
		builder->get_widget("DialogAbout", d);
		d->run();
		d->hide();
	});

	/***********
	 * Emitter *
	 ***********/
	builder->get_widget("ToggleCraftProfiles", toggleCraftProfiles);
	builder->get_widget("ComboUseColors",      comboUseColors);
	Defaults::registerWidget(toggleCraftProfiles);
	Defaults::registerWidget(comboUseColors);
	// When the input profiles is active, and the arcade profile is missing, ask for creating an empty arcade profile
	toggleCraftProfiles->signal_toggled().connect([&]() {
		bool isActive = toggleCraftProfiles->get_active();
		comboUseColors->set_sensitive(isActive);
		if (isActive) {
			// check if the arcade profile exist.
//			if (Message::ask("You are using craft profiles option, but the foundation profile for arcades is missing\nDo you want to create an empty arcade profile?") == Gtk::ResponseType::RESPONSE_YES) {
//				Message::displayInfo("The profile was created");
//			}
		}
	});
	builder->get_widget_derived("ListBoxDatasource", listBoxDataSource, "BtnDatasourceUp", "BtnDatasourceDown");
	comboColors->signal_changed().connect([&]() {
		DialogColors::getInstance()->setColorsFromFile(DialogSettings::getInstance()->getDataDir() + comboColors->get_active_id() + ".xml");
	});

	/*******************
	 * Process Handler *
	 *******************/
	builder->get_widget("InputRunEvery", inputRunEvery);
	Defaults::registerWidget(inputRunEvery);

	/*******************
	 * Default Profile *
	 *******************/
	builder->get_widget("ComboDefaultProfile", comboDefaultProfile);
	Defaults::registerWidget(comboDefaultProfile);

	// Check for unsaved project.
	signal_delete_event().connect([](GdkEventAny*) {
		if (Defaults::isDirty())
			if (Message::ask("Are you sure you want to exit without saving your changes?") != Gtk::ResponseType::RESPONSE_YES)
				return true;
		return false;
	});

	signal_show().connect([this]() {

		// Try to load existing settings.
		if (DialogSettings::getInstance()->loadSettings()) {
			// Settings loaded successfully, populate colors.
			populateColorsCombo();
			return;
		}

		// First run - attempt auto-detection.
		string binaryPath = Glib::find_program_in_path("ledspicerd");

		if (not binaryPath.empty()) {
			// Found in PATH - auto-configure.
			DialogSettings::getInstance()->setBinaryPath(binaryPath, true);

			if (DialogSettings::getInstance()->isValid()) {
				// Auto-detection successful.
				DialogSettings::getInstance()->saveSettings();
				populateColorsCombo();
				Message::displayInfo(
					"Welcome to LEDSpicerUI!\n\n"
					"LEDSpicer was detected and configured automatically.\n"
					"You can change settings anytime using the gear icon."
				);
				return;
			}
		}

		// Binary not found or data dir invalid - show welcome dialog.
		Gtk::MessageDialog dialog(
			*this,
			"Welcome to LEDSpicerUI!",
			false,
			Gtk::MESSAGE_INFO,
			Gtk::BUTTONS_NONE,
			true
		);

		string message;
		if (binaryPath.empty()) {
			message =
				"ledspicerd was not found in your system.\n\n"
				"You can:\n"
				"• Locate the binary manually if LEDSpicer is installed\n"
				"• Continue in portable mode (requires data directory)\n\n"
				"Note: In portable mode, ledspicer.conf is stored\n"
				"within the project directory.";
		}
		else {
			message =
				"LEDSpicer was found but the data directory is missing or invalid.\n\n"
				"Please configure the data directory to continue.\n"
				"The data directory should contain color profiles, and optionally\n"
				"colors.ini, controls.ini, and gameData.xml.";
		}
		dialog.set_secondary_text(message);

		dialog.add_button("Configure Settings", Gtk::RESPONSE_YES);
		dialog.add_button("Exit", Gtk::RESPONSE_NO);

		if (dialog.run() == Gtk::RESPONSE_YES) {
			dialog.hide();

			// Open settings dialog.
			if (DialogSettings::getInstance()->run() == Gtk::RESPONSE_APPLY) {
				DialogSettings::getInstance()->saveSettings();
				populateColorsCombo();
			}
			else {
				// User cancelled settings - check if we have minimum requirements.
				if (not DialogSettings::getInstance()->isValid()) {
					Message::displayError("Data directory is required. Application will close.");
					close();
					return;
				}
			}
		}
		else {
			// User chose to exit.
			close();
			return;
		}
	});
}

MainWindow::~MainWindow() {

	// Data dialogs.
	delete DataDialogs::DialogSelect::getInstance();
	delete DataDialogs::DialogProfile::getInstance();
	delete DataDialogs::DialogRestrictor::getInstance();
	delete DataDialogs::DialogProcess::getInstance();
	delete DataDialogs::DialogGroup::getInstance();
	delete DataDialogs::DialogDevice::getInstance();

	// Miscellaneous dialogs.
	delete DialogColors::getInstance();
	Storage::CollectionHandler::purgeAll();

	delete listBoxDataSource;
}

void MainWindow::prepareDialogs(Glib::RefPtr<Gtk::Builder> const &builder) {

	// Initialize Dialog Colors.
	Gtk::Button* btnAddRandomColor;
	builder->get_widget("BtnAddRandomColor", btnAddRandomColor);
	builder->get_widget("BoxRandomColors",   boxRandomColors);
	DialogColors::getInstance()->activateColorPicker(btnAddRandomColor, boxRandomColors);

	// Dialog to import config files.
	Gtk::Button* btnImportConfig;
	builder->get_widget("BtnImportConfig", btnImportConfig);
	btnImportConfig->signal_clicked().connect([&]() {
		if (dialogImportConfig.run() == Gtk::ResponseType::RESPONSE_OK) {
			string newPath = dialogImportConfig.get_file()->get_path();
			try {
				readConfigFile(newPath, false, dialogImportConfig.getConfigParameters());
			}
			catch (Message& e) {
				Message::displayError(XMLHelper::cleanError(e.getMessage()));
			}
			Defaults::markDirty();
		}
		dialogImportConfig.hide();
	});

	Gtk::Button
		* btnSelectProject,
		* btnAddInput,
		* btnImportInput;
	builder->get_widget("BtnSelectProject", btnSelectProject);
	builder->get_widget("BtnAddInput",      btnAddInput);
	builder->get_widget("BtnImportInput",   btnImportInput);

	// Activate configuration tabs.
	Gtk::Notebook* MainTabs = nullptr;
	builder->get_widget("MainTabs", MainTabs);
	MainTabs->signal_switch_page().connect([btnImportInput, btnAddInput](Gtk::Widget*, guint pageNum) {
		bool sensitive(
			DataDialogs::DialogElement::getInstance()->getCollectionHandler()->getSize() and
			DataDialogs::DialogGroup::getInstance()->getCollectionHandler()->getSize()
		);
		// Inputs: If there no elements or groups, importing or adding inputs is not possible.
		if (pageNum == 4) {
			btnImportInput->set_sensitive(sensitive);
			btnAddInput->set_sensitive(sensitive);
		}
	});

	btnSelectProject->signal_clicked().connect([&, MainTabs]() {
		if (DialogProject::getInstance()->run() != Gtk::ResponseType::RESPONSE_APPLY) {
			DialogProject::getInstance()->hide();
			return;
		}
		string newProject = DialogProject::getInstance()->getProjectName();
		if (newProject == currentProjectName) {
			// TODO add revert option, instead of warning, ask to reload without saving.
			Message::displayInfo("Already working on that project", DialogProject::getInstance());
			DialogProject::getInstance()->hide();
			return;
		}
		if (Defaults::isDirty() and Message::ask("All unsaved changes will be loss, are you sure?", DialogProject::getInstance()) != Gtk::ResponseType::RESPONSE_YES) {
			DialogProject::getInstance()->hide();
			return;
		}

		currentProjectName = newProject;
		// TODO when selecting a project check if is portable mode and do if (mode == Portable) configPath = dataDir + this->projectName + "/ledspicer.conf" in settings
		// Wipe random colors and any other color and read config.
		setColorFile("");
		bool exists = Glib::file_test(DialogSettings::getInstance()->getConfigPath(), Glib::FileTest::FILE_TEST_EXISTS);
		// Old data.
		try {
			readConfigFile(DialogSettings::getInstance()->getConfigPath(), true, IMPORT_ALL);
		}
		// New data.
		catch (Message& e) {
			if (exists)
				Message::displayError(XMLHelper::cleanError("The config file raised an error:\n" + e.getMessage()));
			// Wipe all data.
			profiles.wipe();
			devices.wipe();
			restrictors.wipe();
			processes.wipe();
			groups.wipe();
			inputNavigator.clear();
			// animationNavigator.clear();
			//profileNavigator.clear();
			StringUMap values;
			setConfiguration(values);
		}
		Defaults::cleanDirty();
		MainTabs->set_sensitive(true);
		DialogProject::getInstance()->hide();
	});
}

void MainWindow::setConfiguration(StringUMap& values) {
	// ledspicerd
	inputUserId->set_text(XMLHelper::valueOf(values,        "userId",   DEFAULT_USERID));
	inputGroupId->set_text(XMLHelper::valueOf(values,       "groupId",  DEFAULT_GROUPID));
	inputPortNumber->set_text(XMLHelper::valueOf(values,    "port",     DEFAULT_PORT));
	inputFPS->set_text(XMLHelper::valueOf(values,           "fps",      DEFAULT_FPS));
	setColorFile(XMLHelper::valueOf(values,                 "colors",   DEFAULT_COLORS));
	comboLogLevel->set_active_id(XMLHelper::valueOf(values, "logLevel", DEFAULT_LOGLEVEL));
	listBoxDataSource->sortAndMark(Defaults::explode(XMLHelper::valueOf(values, "dataSource", DEFAULT_DATASOURCE), ','));
	auto randomColors(Defaults::explode(XMLHelper::valueOf(values, "randomColors", ""), ','));
	if (not randomColors.empty()) {
		DialogColors::getInstance()->populateColorBox(boxRandomColors, randomColors);
	}

	// DEFAULT_PROFILE
	// emitter
	toggleCraftProfiles->set_active(XMLHelper::valueOf(values, "craftProfile", DEFAULT_CRAFTPROFILE) == "true");
	comboUseColors->set_active_id(XMLHelper::valueOf(values,   "colorsFile",   DEFAULT_COLORSINFO));
}

StringUMap MainWindow::ledspicerConfigToXml() const {
	StringUMap r {
		// ledspicerd.
		{"version",      PACKAGE_DATA_VERSION},
		{"type",         "Configuration"},
		{"userId",       inputUserId->get_text()},
		{"userId",       inputUserId->get_text()},
		{"groupId",      inputGroupId->get_text()},
		{"port",         inputPortNumber->get_text()},
		{"fps",          inputFPS->get_text()},
		{"logLevel",     comboLogLevel->get_active_id()},
		{"colors",       comboColors->get_active_id()},
		{"dataSource",   Defaults::implode(listBoxDataSource->getCheckedValues(), ',')},
		{"randomColors", Defaults::implode(DialogColors::getInstance()->getColorBoxValues(boxRandomColors), ',')},
		// Emitter.
		{"craftProfile", toggleCraftProfiles->get_active() ? "true" : "false"},
		{"colorsFile",   comboUseColors->get_active_id()},

	};
	return r;
}

void MainWindow::readConfigFile(const string& dataFilePath, bool wipe, uint8_t importFlags) {

	ConfigFile datafile(dataFilePath);
	if (importFlags & Defaults::ImportFlags::CONFIG) {
		auto c(datafile.getSettings());
		// check if color are different.
		const string
			colors(c.find("colors") != c.end() ? c.at("colors") : ""),
			previous(comboColors->get_active_id());
		if (not previous.empty() and previous != colors)
			Message::displayInfo("Warning\nColors definition file changed, element color changed");
		setConfiguration(c);
	}

	// Load Devices, elements and groups from config file.
	if (importFlags & Defaults::ImportFlags::DEVICES) {
		if (wipe) {
			devices.wipe();
			groups.wipe();
		}
		DataDialogs::DialogDevice::getInstance()->load(&datafile);
		DataDialogs::DialogDevice::getInstance()->refreshBox();
		DataDialogs::DialogGroup::getInstance()->load(&datafile);
		DataDialogs::DialogGroup::getInstance()->refreshBox();
		// TODO: set default profile
	}

	// Load restrictors from config file.
	if (importFlags & Defaults::ImportFlags::RESTRICTORS) {
		if (wipe) {
			restrictors.wipe();
		}
		DataDialogs::DialogRestrictor::getInstance()->load(&datafile);
		DataDialogs::DialogRestrictor::getInstance()->refreshBox();
	}

	// Load process mappings from config file.
	if (importFlags & Defaults::ImportFlags::MAPPINGS) {
		if (wipe) {
			processes.wipe();
		}
		DataDialogs::DialogProcess::getInstance()->load(&datafile);
		DataDialogs::DialogProcess::getInstance()->refreshBox();
		inputRunEvery->set_text(datafile.getProcessLookupRunEvery());
	}

	// TODO: use registered profiles.
	comboDefaultProfile->remove_all();
	for (const auto& item : {datafile.getDefaultProfile()})
		comboDefaultProfile->append(item);

	// this will trigger load.
	comboDefaultProfile->set_active_text(datafile.getDefaultProfile());

	// todo: using default profile load inputs and animations.
	/*	if (wipe) {
		inputs.wipe();
		//DataDialogs::DialogProfile::getInstance()->refreshBox();
	}*/
}

void MainWindow::setColorFile(const string& colorFile) {

	if (colorFile.empty()) {
		comboColors->set_active_id("");
		return;
	}

	try {
		XMLHelper datafile(DialogSettings::getInstance()->getDataDir() + colorFile + ".xml", "Colors");
		comboColors->set_active_id(colorFile);
	}
	catch (Message& e) {
		e.displayError();
	}
}

void MainWindow::populateColorsCombo() {
	comboColors->remove_all();
	comboColors->append("", "Select Colors");
	for (const auto& c : DialogSettings::getInstance()->getColorFiles()) {
		comboColors->append(c, c);
	}
}
