/* -/*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.cpp
 * @since     Feb 13, 2023
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

#include "MainWindow.hpp"

using namespace LEDSpicerUI::Ui;

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::ApplicationWindow(obj),
	// Set import dialog
	dic(DialogImport::Types::CONFIG, this),
	// Project directoy openner.
	dds(
		"Select Project Directory",
		Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER,
		Gtk::DialogFlags::DIALOG_MODAL | Gtk::DialogFlags::DIALOG_DESTROY_WITH_PARENT | Gtk::DialogFlags::DIALOG_USE_HEADER_BAR
	)
{

	// Setup ledspicer fields.
	builder->get_widget("InputUserId",     InputUserId);
	builder->get_widget("InputGroupId",    InputGroupId);
	builder->get_widget("InputPortNumber", InputPortNumber);
	builder->get_widget("InputFPS",        InputFPS);
	builder->get_widget("InputColors",     InputColors);
	builder->get_widget("InputLogLevel",   InputLogLevel);

	// Register Widgets that changes the status to not saved.
	Defaults::registerWidget(InputUserId);
	Defaults::registerWidget(InputGroupId);
	Defaults::registerWidget(InputPortNumber);
	Defaults::registerWidget(InputFPS);
	Defaults::registerWidget(InputColors);
	Defaults::registerWidget(InputLogLevel);

	// Initialize Dialog Colors.
	Gtk::Button* btnAddRandomColor;
	builder->get_widget("BtnAddRandomColor", btnAddRandomColor);
	builder->get_widget("BoxRandomColors",   boxRandomColors);
	DialogColors::getInstance(builder)->activateColorPicker(btnAddRandomColor, boxRandomColors);

	// Initialize Pin Handler.
	Forms::PinHandler::initialize(builder);

	// Initialize secondary dialogs.
	DialogElement::initialize(builder);
	DialogSelectElements::initialize(builder);
	DialogInputLinkMaps::initialize(builder);
	DialogInputMap::initialize(builder);

	// get Primary Dialogs
	builder->get_widget_derived("DialogDevice",     dd);
	builder->get_widget_derived("DialogRestrictor", dr);
	builder->get_widget_derived("DialogGroup",      dg);
	builder->get_widget_derived("DialogInput",      di);

	// Setup top buttons
	Gtk::Button
		* btnAbout,
		* btnOpenProject,
		* btnSaveProject;
	Gtk::FileChooserButton* dataDirSelect = nullptr;
	builder->get_widget("BtnSelectDir",   btnOpenProject);
	builder->get_widget("BtnSaveProject", btnSaveProject);
	builder->get_widget("BtnAbout",       btnAbout);
	builder->get_widget("dataDirSelect",  dataDirSelect);

	// Top directory information.
	Gtk::HeaderBar* header;
	builder->get_widget("Header", header);
	Defaults::initialize(header, btnSaveProject);


	Gtk::Fixed* FixedLayout;
	builder->get_widget("FixedLayout",  FixedLayout);

	// Import config file Dialog.
	Gtk::Button* btnImportConfig;
	builder->get_widget("BtnImportConfig", btnImportConfig);
	btnImportConfig->signal_clicked().connect([&]() {
		if (dic.run() == Gtk::ResponseType::RESPONSE_OK) {
			string newPath = dic.get_file()->get_path();
			import(newPath, false, dic.getConfigParameters());
			Defaults::markDirty();
		}
		dic.hide();
	});

	// Create select work directory FileChooserDialog
	prepareWorkingDirectory(builder);

	// Data selector action.
	dataDirSelect->set_current_folder(PACKAGE_DATA_DIR"..");
	dataDirSelect->signal_file_set().connect([&, btnOpenProject, dataDirSelect] () {
		btnOpenProject->set_sensitive(false);
		try {
			openDataDirectory(dataDirSelect->get_filename() + "/");
			btnOpenProject->set_sensitive(true);
			Defaults::cleanDirty();
		}
		catch(Message& e) {
			e.displayError();
		}
		catch(const Glib::Error& ex) {
			Message::displayError(ex.what());
		}
	});

	// Save project
	btnSaveProject->signal_clicked().connect([&]() {
		try {
			// Prepare dicrectories
			for (string d : {"/animations", "/inputs", "/profiles"}) {
				string path(workingDirectory + d);
				struct stat info;
				if (stat(path.c_str(), &info) != 0) {
					// Create new dir
					if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
						throw Message("Unable to save, failed to create directory " + d);
				}
				else if (not S_ISDIR(info.st_mode)) {
					// name exist but is not a dir.
					throw Message("Unable to save, a file with the name " + d + " already exist.");
				}
			}
			// Create config section.
			string xmlData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!-- " DEFAULT_MESSAGE "-->\n<LEDSpicer\n");
			Defaults::increaseTab();
			xmlData += readConfiguration();
			Defaults::reduceTab();
			xmlData += ">\n";
			// Create devices, restrictors and process
			Defaults::increaseTab();
			xmlData += dp->toXml() + dd->toXml() + dr->toXml();
			// Add layout.
			//"<layout defaultProfile="default">";
			Defaults::reduceTab();
			xmlData += "</LEDSpicer>\n";
			// Save conf
			string configFile(CONFIG_FILE);
			Glib::file_set_contents(configFile, xmlData);
			Defaults::cleanDirty();
			Message::displayInfo("Project saved successfully.");
		}
		catch (Message &e) {
			e.displayError();
		}
		catch (const Glib::FileError& e) {
			Message::displayError("Unable to the project, failed to write to " CONFIG_FILE " " + string(e.what()));
		}
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
	builder->get_widget("InputColorsFile",    InputColorsFile);
	builder->get_widget("InputCraftProfiles", InputCraftProfiles);
	Defaults::registerWidget(InputColorsFile);
	Defaults::registerWidget(InputCraftProfiles);
	builder->get_widget_derived("ListBoxDatasource", ListBoxDataSource, "BtnDatasourceUp", "BtnDatasourceDown");
	InputColors->signal_changed().connect([&]() {
		DialogColors::getInstance()->setColorsFromFile(dataDirectory + InputColors->get_active_id() + ".xml");
	});

	/*******************
	 * Process Handler *
	 *******************/
	builder->get_widget_derived("DialogProcess", dp);
	builder->get_widget("InputRunEvery", InputRunEvery);
}

MainWindow::~MainWindow() {
	// derived classes.
	delete dp;
	delete di;
	delete dg;
	delete dd;
	delete dr;
	delete DialogElement::getInstance();
	delete DialogSelectElements::getInstance();
	delete DialogColors::getInstance();
	delete Forms::PinHandler::getInstance();
	Forms::CollectionHandler::wipe();
	delete ListBoxDataSource;
}

void MainWindow::openDataDirectory(const string& dataDirectory) {
	// Set color list.
	InputColors->remove_all();
	InputColors->append("", "Select Colors");
	string msg;

	auto directory = Gio::File::create_for_path(dataDirectory);
	if (not directory)
		throw Message(dataDirectory + " is not valid anymore, try a different directory");

	auto enumerator = directory->enumerate_children();
	if (not enumerator)
		throw Message(dataDirectory + " is invalid, try a different directory");

	this->dataDirectory = dataDirectory;
	Glib::RefPtr<Gio::FileInfo> file_info;
	while ((file_info = enumerator->next_file())) {
		auto type = file_info->get_file_type();
		if (type == Gio::FILE_TYPE_DIRECTORY)
			continue;

		string filename(file_info->get_name());
		if (filename == "gameData.xml") {
			msg += "Games data file gameData.xml Found\n";
			continue;
		}
		if (filename == "colors.ini") {
			msg += "Games data file colors.ini Found\n";
			continue;
		}
		if (filename == "controls.ini") {
			msg += "Games data file controls.ini Found\n";
			continue;
		}
		auto parts = Defaults::explode(filename, '.');
		string ext(parts.back());
		if (ext == "xml") {
			parts.pop_back();
			string name(Defaults::implode(parts, '.'));
			try {
				XMLHelper testCol(dataDirectory + filename, "Colors");
				msg += "Colors file " + filename + " Found\n";
				InputColors->append(name, name);
			}
			catch(...) {}
		}
	}

	if (msg.empty())
		throw Message("The directory does not contain any useful file");
	Message::displayInfo(msg);
}

void MainWindow::prepareWorkingDirectory(Glib::RefPtr<Gtk::Builder> const &builder) {
	// Create select work directory FileChooserDialog
	dds.set_transient_for(*this);
	dds.set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
	dds.set_default_size(900, 700);

	// Prepare open project dialog functionality.
	dds.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL)->get_style_context()->add_class("backgroundRed");
	dds.add_button("_Select", Gtk::ResponseType::RESPONSE_OK)->get_style_context()->add_class("backgroundGreen");

	Gtk::Button* btnOpenProject;
	builder->get_widget("BtnSelectDir", btnOpenProject);

	// Activate configuration tabs.
	Gtk::Notebook* MainTabs = nullptr;
	builder->get_widget("MainTabs", MainTabs);
	btnOpenProject->signal_clicked().connect([&, MainTabs]() {
		if (dds.run() != Gtk::ResponseType::RESPONSE_OK) {
			dds.hide();
			return;
		}
		string newPath = dds.get_file()->get_path();
		if (newPath == workingDirectory) {
			// TODO add revert option
			Message::displayInfo("Already working on that project", &dds);
			dds.hide();
			return;
		}
		if (Defaults::isDirty() and Message::ask("All unsaved changes will be loss, are you sure?", &dds) != Gtk::RESPONSE_YES) {
			dds.hide();
			return;
		}
		// set working directory
		workingDirectory = newPath;
		Defaults::setSubtitle(workingDirectory);

		// wipe random colors and any other color
		setColorFile("");
		// set values.
		try {
			// Old.
			import(workingDirectory + "/ledspicer.conf", true, IMPORT_ALL);
		}
		catch (Message& e) {
			// New.
			// wipe forms.
			dr->wipeContents();
			dp->wipeContents();
			dg->wipeContents();
			DialogSelectElements::getInstance()->wipeContents();
			DialogElement::getInstance()->wipeContents();
			dd->wipeContents();
			unordered_map<string, string> values;
			setConfiguration(values);
		}
		Defaults::cleanDirty();
		MainTabs->set_sensitive(true);
		dds.hide();
	});
}

void MainWindow::setConfiguration(unordered_map<string, string>& values) {
	// ledspicerd
	InputUserId->set_text(XMLHelper::valueOf(values,        "userId",   DEFAULT_USERID));
	InputGroupId->set_text(XMLHelper::valueOf(values,       "groupId",  DEFAULT_GROUPID));
	InputPortNumber->set_text(XMLHelper::valueOf(values,    "port",     DEFAULT_PORT));
	InputFPS->set_text(XMLHelper::valueOf(values,           "fps",      DEFAULT_FPS));
	setColorFile(XMLHelper::valueOf(values,                 "colors",   DEFAULT_COLORS));
	InputLogLevel->set_active_id(XMLHelper::valueOf(values, "logLevel", DEFAULT_LOGLEVEL));
	ListBoxDataSource->sortAndMark(Defaults::explode(XMLHelper::valueOf(values, "dataSource", DEFAULT_DATASOURCE), ','));
	auto randomColors(Defaults::explode(XMLHelper::valueOf(values, "randomColors", ""), ','));
	if (not randomColors.empty())
		DialogColors::getInstance()->populateColorBox(boxRandomColors, randomColors);

	// DEFAULT_PROFILE
	// emitter
	InputColorsFile->set_active(XMLHelper::valueOf(values,    "colorsFile",   DEFAULT_COLORSINFO)   == "true");
	InputCraftProfiles->set_active(XMLHelper::valueOf(values, "craftProfile", DEFAULT_CRAFTPROFILE) == "true");
}

string MainWindow::readConfiguration() {
	unordered_map<string, string> r {
		// ledspicerd
		{"version", "1.0"},
		{"type",    "Configuration"},
		{"userId",  InputUserId->get_text()},
		{"userId",  InputUserId->get_text()},
		{"groupId", InputGroupId->get_text()},
		{"port",    InputPortNumber->get_text()},
		{"fps",     InputFPS->get_text()},
		{"logLevel", InputLogLevel->get_active_id()},
		{"colors",   InputColors->get_active_id()},
		{"dataSource",   Defaults::implode(ListBoxDataSource->getCheckedValues(), ',')},
		{"randomColors", Defaults::implode(DialogColors::getInstance()->getColorBoxValues(boxRandomColors), ',')},
	};
	// emitter
	if (InputColorsFile->get_active())
		r.emplace("colorsFile", "true");
	if (InputCraftProfiles->get_active())
		r.emplace("craftProfile", "true");

	return XMLHelper::toXML(r);
}

void MainWindow::import(const string& ledspicerconf, bool wipe, uint8_t importFlags) {
	ConfigFile datafile(ledspicerconf);
	if (importFlags & IMPORT_CONFIG) {
		auto c(datafile.getSettings());
		// check if color are different.
		const string
			colorFile(c.count("colorsFile") ? c.at("colorsFile") : ""),
			previous(InputColors->get_active_id());
		if (not previous.empty() and previous != colorFile)
			Message::displayInfo("Warning\nColors definition file changed, element color changed");
		setConfiguration(c);
	}
	if (importFlags & IMPORT_DEVICES) {
		for (auto i : dd->createItems(datafile.getDevices(), wipe))
			DialogElement::getInstance()->createItems(datafile.getDeviceElements(i->getForm()->createName()), i, wipe);

		for (auto i : dg->createItems(datafile.getGroups(), wipe))
			DialogSelectElements::getInstance()->createItems(datafile.getGroupElements(i->getForm()->createName()), i, wipe);
		// TODO: set default profile
	}
	if (importFlags & IMPORT_RESTRICTORS) {
		dr->createItems(datafile.getRestrictors(), wipe);
	}

	if (importFlags & IMPORT_MAPPINGS) {
		dp->createItems(datafile.getProcess(), wipe);
		dp->setRunEvery(datafile.getProcessLookupRunEvery());
	}
}

void MainWindow::setColorFile(const string& colorFile) {

	if (colorFile.empty()) {
		InputColors->set_active_id("");
		return;
	}
	try {
		XMLHelper datafile(dataDirectory + colorFile + ".xml", "Colors");
		InputColors->set_active_id(colorFile);
	}
	catch (Message& e) {
		e.displayError();
	}
}

