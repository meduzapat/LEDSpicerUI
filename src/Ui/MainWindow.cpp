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
using namespace Storage;
using namespace DataDialogs;
using LEDSpicerUI::Config::Settings;

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::ApplicationWindow(obj),
	// Set import dialog
	dialogImportConfig(DialogImport::Types::CONFIG, this),

	inputNavigator(builder, this),
	animationNavigator(builder, this),
	profileNavigator(builder, this)
{

	Message::initialize(builder, this);
	StatusBar::initialize(builder);

	DialogSettings::buildInstance(builder, "DialogSettings");
	DialogProject::buildInstance(builder,  "DialogProject");

	// Initialize secondary dialogs.
	DialogSelect::buildInstance(builder, "DialogSelect");

	// Initialize Primary Dialogs.
	DialogDevice::buildInstance(builder,     "DialogDevice");
	DialogRestrictor::buildInstance(builder, "DialogRestrictor");
	DialogProcess::buildInstance(builder,    "DialogProcess");
	DialogGroup::buildInstance(builder,      "DialogGroup");

	// Connect primary dialogs with the collections.
	DialogDevice::getInstance()->setOwner(&devices,         nullptr);
	DialogRestrictor::getInstance()->setOwner(&restrictors, nullptr);
	DialogProcess::getInstance()->setOwner(&processes,      nullptr);
	DialogGroup::getInstance()->setOwner(&groups,           nullptr);

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
				throw Message("The number of random colors need to be more than one or none.");
			}

			ConfigFile::save(ConfigFile::ConfigData(
				Settings::get().getActiveConfigPath(),
				profileNavigator.getDefaultProfileName(),
				inputRunEvery->get_text().raw(),
				packLedspicerConfig(),
				devices,
				restrictors,
				groups,
				processes
			));

			inputNavigator.save();
			animationNavigator.save();
			profileNavigator.save();

			Defaults::cleanDirty();
			DialogSettings::getInstance()->saveSettings();
			Message::displayInfo("Project saved successfully.");
		}
		catch (Message& e) {
			e.displayError();
		}
		catch (const Glib::FileError& e) {
			Message::displayError("Unable to save project: " + string(e.what()));
		}
	});

	// Refresh colors combo whenever the color file list is replaced.
	Settings::get().onColorFilesChanged([this]() {
		populateColorsCombo();
	});

	// About.
	btnAbout->signal_clicked().connect([builder]() {
		Gtk::Dialog* d;
		Gtk::Label* l;
		builder->get_widget("DialogAbout", d);
		builder->get_widget("LabelAboutBuild", l);
		l->set_text(
			"Built with: GTK+ " +
			std::to_string(gtk_get_major_version()) + "." +
			std::to_string(gtk_get_minor_version()) + "." +
			std::to_string(gtk_get_micro_version())
		);
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
	builder->get_widget_derived("ListBoxDatasource", listBoxDataSource, "BtnDatasourceUp", "BtnDatasourceDown");
	comboColors->signal_changed().connect([&]() {
		DialogColors::getInstance()->setColorsFromFile(Settings::get().getDataDir() + comboColors->get_active_id() + ".xml");
	});

	/*******************
	 * Process Handler *
	 *******************/
	builder->get_widget("InputRunEvery", inputRunEvery);
	Defaults::registerWidget(inputRunEvery);

	// Check for unsaved project.
	signal_delete_event().connect([](GdkEventAny*) {
		if (Defaults::isDirty())
			if (Message::ask("Are you sure you want to exit without saving your changes?") != Gtk::ResponseType::RESPONSE_YES)
				return true;
		return false;
	});

	signal_show().connect([this]() {
		Defaults::setIgnoreChanges(true);
		if (not DialogSettings::getInstance()->startup(this)) {
			Defaults::setIgnoreChanges(false);
			close();
			return;
		}
		const string& defaultProject = Settings::get().getDefaultProject();
		if (not defaultProject.empty())
			openProject(defaultProject);
		Defaults::setIgnoreChanges(false);
	});
}

MainWindow::~MainWindow() {

	profileNavigator.clear();
	animationNavigator.clear();
	inputNavigator.clear();

	// need to be wipe in the correct order or the dependencies will cause problems.
	groups.wipe();
	processes.wipe();
	restrictors.wipe();
	devices.wipe();

	CollectionHandler::purgeAll();

	// Data dialogs.
	delete DialogSelect::getInstance();
	delete DialogProfile::getInstance();
	delete DialogRestrictor::getInstance();
	delete DialogProcess::getInstance();
	delete DialogGroup::getInstance();
	delete DialogDevice::getInstance();

	// Miscellaneous dialogs.
	delete DialogColors::getInstance();
}

void MainWindow::prepareDialogs(Glib::RefPtr<Gtk::Builder> const &builder) {

	// Initialize Dialog Colors.
	Gtk::Button* btnAddRandomColor;
	builder->get_widget("BtnAddRandomColor", btnAddRandomColor);
	builder->get_widget("BoxRandomColors",   boxRandomColors);
	DialogColors::getInstance()->activateColorPicker(btnAddRandomColor, boxRandomColors);

	// Dialog to import config files.
	builder->get_widget("BtnImportConfig", btnImportConfig);
	btnImportConfig->set_sensitive(false);
	btnImportConfig->signal_clicked().connect([this]() {
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

	Gtk::Button* btnSelectProject;
	builder->get_widget("BtnSelectProject", btnSelectProject);

	// Activate configuration tabs.
	builder->get_widget("MainTabs",    mainTabs);
	builder->get_widget("MainTabsBox", mainTabsBox);
	mainTabs->property_visible_child_name().signal_changed().connect([this]() {
		const auto child {mainTabs->get_visible_child_name()};
		if (child == "inputs")     inputNavigator.onActivate();
		if (child == "animations") animationNavigator.onActivate();
		if (child == "profiles")   profileNavigator.onActivate();
	});

	btnSelectProject->signal_clicked().connect([this]() {
		if (DialogProject::getInstance()->run() != Gtk::ResponseType::RESPONSE_APPLY) {
			DialogProject::getInstance()->hide();
			return;
		}
		const string& newProject {DialogProject::getInstance()->getProjectName()};
		if (newProject == Settings::get().getCurrentProject()) {
			// TODO add revert option, instead of warning, ask to reload without saving.
			Message::displayInfo("Already working on that project", DialogProject::getInstance());
			DialogProject::getInstance()->hide();
			return;
		}
		if (Defaults::isDirty() and Message::ask("All unsaved changes will be loss, are you sure?", DialogProject::getInstance()) != Gtk::ResponseType::RESPONSE_YES) {
			DialogProject::getInstance()->hide();
			return;
		}
		openProject(newProject);
		DialogProject::getInstance()->hide();
	});
}

void MainWindow::setConfiguration(const Values& values) {
	// ledspicerd
	inputUserId->set_text(values.getValue("userId", DEFAULT_USERID));
	inputGroupId->set_text(values.getValue("groupId", DEFAULT_GROUPID));
	inputPortNumber->set_text(values.getValue("port", DEFAULT_PORT));
	inputFPS->set_text(values.getValue("fps", DEFAULT_FPS));
	comboColors->set_active_id(values.getValue("colors", DEFAULT_COLORS));
	comboLogLevel->set_active_id(values.getValue("logLevel", DEFAULT_LOGLEVEL));
	listBoxDataSource->sortAndMark(Defaults::explode(values.getValue("dataSource", DEFAULT_DATASOURCE), ','));
	auto randomColors(Defaults::explode(values.getValue("randomColors"), ','));
	if (not randomColors.empty()) {
		DialogColors::getInstance()->populateColorBox(boxRandomColors, randomColors);
	}

	// DEFAULT_PROFILE
	// emitter
	toggleCraftProfiles->set_active(values.getValue("craftProfile", DEFAULT_CRAFTPROFILE) == HUMAN_TRUE);
	comboUseColors->set_active_id(values.getValue("colorsFile", DEFAULT_COLORSINFO));
}

LEDSpicerUI::Values MainWindow::packLedspicerConfig() const noexcept {
	Values r {
		// ledspicerd.
		{"version",        PACKAGE_DATA_VERSION},
		{"type",           "Configuration"},
		{"defaultProject", Settings::get().getCurrentProject()},
		{"userId",         inputUserId->get_text()},
		{"groupId",        inputGroupId->get_text()},
		{"port",           inputPortNumber->get_text()},
		{"fps",            inputFPS->get_text()},
		{"logLevel",       comboLogLevel->get_active_id()},
		{"colors",         comboColors->get_active_id()},
		{"dataSource",     Defaults::implode(listBoxDataSource->getCheckedValues(), ',')},
		// Emitter.
		{"craftProfile",   toggleCraftProfiles->get_active() ? HUMAN_TRUE : HUMAN_FALSE},
		{"colorsFile",     comboUseColors->get_active_id()},
	};

	if (const auto rc {DialogColors::getInstance()->getColorBoxValues(boxRandomColors)}; not rc.empty())
		r.setValue("randomColors", Defaults::implode(rc, ','));

	return r;
}

void MainWindow::openProject(const string& name) {
	Settings::get().setCurrentProject(name);
	Defaults::setSubtitle(name);
	comboColors->set_active_id("");

	try {
		readConfigFile(Settings::get().getActiveConfigPath(), true, IMPORT_ALL);
		DialogSettings::getInstance()->saveSettings();
	}
	catch (Message& e) {
		if (Glib::file_test(Settings::get().getActiveConfigPath(), Glib::FileTest::FILE_TEST_EXISTS))
			Message::displayError(XMLHelper::cleanError("The config file raised an error:\n" + e.getMessage()));
		devices.wipe();
		restrictors.wipe();
		processes.wipe();
		groups.wipe();
		inputNavigator.clear();
		animationNavigator.clear();
		profileNavigator.clear();
		DialogColors::getInstance()->resetColorButtons();
		Values values;
		setConfiguration(values);
	}

	DialogDevice::getInstance()->refreshItems();
	DialogGroup::getInstance()->refreshItems();
	DialogRestrictor::getInstance()->refreshItems();
	DialogProcess::getInstance()->refreshItems();
	Defaults::cleanDirty();
	mainTabs->set_visible_child("configuration");
	mainTabsBox->set_sensitive(true);
	btnImportConfig->set_sensitive(true);
}

void MainWindow::readConfigFile(const string& dataFilePath, bool wipe, uint8_t importFlags) {

	ConfigFile datafile(dataFilePath);
	if (importFlags & Defaults::ImportFlags::CONFIG) {
		const auto& c(datafile.getRootInfo());
		// check if color are different.
		const string
			& colors {c.getValue("colors")},
			& previous(comboColors->get_active_id());
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
		DialogDevice::getInstance()->load(datafile.getDataMap());
		DialogGroup::getInstance()->load(datafile.getDataMap());
	}

	// Load restrictors from config file.
	if (importFlags & Defaults::ImportFlags::RESTRICTORS) {
		if (wipe) {
			restrictors.wipe();
		}
		DialogRestrictor::getInstance()->load(datafile.getDataMap());
	}

	// Load process mappings from config file.
	if (importFlags & Defaults::ImportFlags::MAPPINGS) {
		if (wipe) {
			processes.wipe();
		}
		DialogProcess::getInstance()->load(datafile.getDataMap());
		inputRunEvery->set_text(datafile.getRootInfo().getValue(PARAM_MILLISECONDS));
	}

	// Load inputs, animations and profiles from the project subdirectories.
	if (wipe and not Settings::get().getProjectDir().empty()) {
		inputNavigator.clear();
		animationNavigator.clear();
		profileNavigator.setDefaultProfileName(datafile.getRootInfo().getValue("defaultProfile"));
		profileNavigator.clear();

		inputNavigator.load();
		animationNavigator.load();
		profileNavigator.load();
	}
}

void MainWindow::populateColorsCombo() {
	const string active = comboColors->get_active_id();
	comboColors->remove_all();
	for (const auto& c : Settings::get().getColorFiles())
		comboColors->append(c, c);
	if (not active.empty())
		comboColors->set_active_id(active);
}
