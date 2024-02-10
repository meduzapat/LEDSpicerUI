/* -/*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainWindow.cpp
 * @since     Feb 13, 2023
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

#include "MainWindow.hpp"

using namespace LEDSpicerUI::Ui;

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::ApplicationWindow(obj),
	MainDialogs(builder, this)
{
	// Setup ledspicer fields.
	builder->get_widget("InputUserId",     inputUserId);
	builder->get_widget("InputGroupId",    inputGroupId);
	builder->get_widget("InputPortNumber", inputPortNumber);
	builder->get_widget("InputFPS",        inputFPS);
	builder->get_widget("InputColors",     inputColors);
	builder->get_widget("InputLogLevel",   inputLogLevel);

	// Register Widgets that changes the status to not saved.
	Defaults::registerWidget(inputUserId);
	Defaults::registerWidget(inputGroupId);
	Defaults::registerWidget(inputPortNumber);
	Defaults::registerWidget(inputFPS);
	Defaults::registerWidget(inputColors);
	Defaults::registerWidget(inputLogLevel);

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

	// connect dialogs
	prepareDialogs(builder);

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
			// Prepare directories
			for (string d : {"/animations", "/inputs", "/profiles"}) {
				string path(workingDirectory + d);
				struct stat info;
				if (stat(path.c_str(), &info) != 0) {
					// Create new directory
					if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
						throw Message("Unable to save, failed to create directory " + d);
				}
				else if (not S_ISDIR(info.st_mode)) {
					// name exist but is not a directory.
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

			if (processes.getSize()) {
				xmlData += "<processLookup runEvery=\"" + inputRunEvery->get_text() + "\">\n";
				Defaults::increaseTab();
				for (auto p : processes) {
					xmlData += p->getData()->toXML();
				}
				Defaults::reduceTab();
				xmlData += "</processLookup>\n";
			}

			xmlData += "<devices>\n";
			Defaults::increaseTab();
			for (auto d : devices) {
				xmlData += d->getData()->toXML();
			}
			Defaults::reduceTab();
			xmlData += "</devices>\n";

			if (restrictors.getSize()) {
				xmlData += "<restrictors>\n";
				Defaults::increaseTab();
				for (auto r : restrictors) {
					xmlData += r->getData()->toXML();
				}
				Defaults::reduceTab();
				xmlData += "</restrictors>\n";
			}

			// Add layout.
			xmlData += "<layout defaultProfile=\"" + inputDefaultProfile->get_active_id() + "\">";
			Defaults::increaseTab();
			for (auto g : groups) {
				xmlData += g->getData()->toXML();
			}
			Defaults::reduceTab();
			xmlData += "</layout>\n";

			Defaults::reduceTab();
			xmlData += "</LEDSpicer>\n";

			// Save config
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
	builder->get_widget("InputColorsFile",    inputColorsFile);
	builder->get_widget("InputCraftProfiles", inputCraftProfiles);
	Defaults::registerWidget(inputColorsFile);
	Defaults::registerWidget(inputCraftProfiles);
	builder->get_widget_derived("ListBoxDatasource", listBoxDataSource, "BtnDatasourceUp", "BtnDatasourceDown");
	inputColors->signal_changed().connect([&]() {
		DialogColors::getInstance()->setColorsFromFile(dataDirectory + inputColors->get_active_id() + ".xml");
	});

	/*******************
	 * Process Handler *
	 *******************/
	builder->get_widget("InputRunEvery", inputRunEvery);
	Defaults::registerWidget(inputRunEvery);
}

MainWindow::~MainWindow() {
	delete listBoxDataSource;
}

void MainWindow::openDataDirectory(const string& dataDirectory) {
	// Set color list.
	inputColors->remove_all();
	inputColors->append("", "Select Colors");
	string msg;

	auto directory = Gio::File::create_for_path(dataDirectory);
	if (not directory)
		throw Message(dataDirectory + " is not valid anymore, try a different directory");

	auto enumerator = directory->enumerate_children();
	if (not enumerator)
		throw Message(dataDirectory + " is invalid, try a different directory");

	this->dataDirectory = dataDirectory;
	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		auto type = fileInfo->get_file_type();
		if (type == Gio::FILE_TYPE_DIRECTORY)
			continue;

		string filename(fileInfo->get_name());
		for (auto& f : {"gameData.xml", "colors.ini", "controls.ini"}) {
			string f2(f);
			if (filename == f2) {
				msg += "✅ Games data file " + f2 + " Found\n";
				continue;
			}
		}
		auto parts = Defaults::explode(filename, '.');
		string ext(parts.back());
		if (ext == "xml") {
			parts.pop_back();
			string name(Defaults::implode(parts, '.'));
			try {
				XMLHelper testCol(dataDirectory + filename, "Colors");
				msg += "✅ Colors file " + filename + " Found\n";
				inputColors->append(name, name);
			}
			catch(...) {}
		}
	}

	if (msg.empty())
		throw Message("The directory does not contain any useful file");
	// Warning game data files not found.
	for (auto& f : {"gameData.xml", "colors.ini", "controls.ini"}) {
		if (msg.find(f) == string::npos) {
			msg += "❌ Games data file " + string(f) + " Not Found\n";
		}
	}
	Message::displayInfo(msg);
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
			import(newPath, false, dialogImportConfig.getConfigParameters());
			Defaults::markDirty();
		}
		dialogImportConfig.hide();
	});

	// Dialog to import input plugin files.
	Gtk::Button* btnImportInput;
	builder->get_widget("BtnImportInput", btnImportInput);
	btnImportInput->signal_clicked().connect([&]() {
		if (dialogImportInput.run() == Gtk::ResponseType::RESPONSE_OK) {
			// Retrieve the selected files or directories
			vector<string> selectedFiles(dialogImportInput.get_filenames());
			// Process each selected file or directory
			for (const auto& selectedFile : selectedFiles) {
				import(selectedFile, false, IMPORT_INPUTS);
			}
		}
		dialogImportInput.hide();
	});

	Gtk::Button
		* btnOpenProject,
		* btnAddInput;
	builder->get_widget("BtnSelectDir", btnOpenProject);
	builder->get_widget("BtnAddInput",  btnAddInput);

	// Activate configuration tabs.
	Gtk::Notebook* MainTabs = nullptr;
	builder->get_widget("MainTabs", MainTabs);

	MainTabs->signal_switch_page().connect([=](Gtk::Widget* page, guint pageNum) {
		// Inputs
		if (pageNum == 4) {
			bool sensitive(
				Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)->getSize() and
				Storage::CollectionHandler::getInstance(COLLECTION_GROUP)->getSize());
			btnImportInput->set_sensitive(sensitive);
			btnAddInput->set_sensitive(sensitive);
		}
	});

	btnOpenProject->signal_clicked().connect([&, MainTabs]() {
		if (dialogSelectWorkingDirectory.run() != Gtk::ResponseType::RESPONSE_OK) {
			dialogSelectWorkingDirectory.hide();
			return;
		}
		string newPath = dialogSelectWorkingDirectory.get_file()->get_path();
		if (newPath == workingDirectory) {
			// TODO add revert option
			Message::displayInfo("Already working on that project", &dialogSelectWorkingDirectory);
			dialogSelectWorkingDirectory.hide();
			return;
		}
		if (Defaults::isDirty() and Message::ask("All unsaved changes will be loss, are you sure?", &dialogSelectWorkingDirectory) != Gtk::RESPONSE_YES) {
			dialogSelectWorkingDirectory.hide();
			return;
		}
		// set working directory
		workingDirectory = newPath;
		Defaults::setSubtitle(workingDirectory);

		// wipe random colors and any other color
		setColorFile("");

		// Old data.
		try {
			import(workingDirectory + "/ledspicer.conf", true, IMPORT_ALL);
		}
		// New data.
		catch (Message& e) {
			// wipe all data.
			devices.wipe();
			restrictors.wipe();
			processes.wipe();
			groups.wipe();
			inputs.wipe();
			unordered_map<string, string> values;
			setConfiguration(values);
		}
		Defaults::cleanDirty();
		MainTabs->set_sensitive(true);
		dialogSelectWorkingDirectory.hide();
	});
}

void MainWindow::setConfiguration(unordered_map<string, string>& values) {
	// ledspicerd
	inputUserId->set_text(XMLHelper::valueOf(values,        "userId",   DEFAULT_USERID));
	inputGroupId->set_text(XMLHelper::valueOf(values,       "groupId",  DEFAULT_GROUPID));
	inputPortNumber->set_text(XMLHelper::valueOf(values,    "port",     DEFAULT_PORT));
	inputFPS->set_text(XMLHelper::valueOf(values,           "fps",      DEFAULT_FPS));
	setColorFile(XMLHelper::valueOf(values,                 "colors",   DEFAULT_COLORS));
	inputLogLevel->set_active_id(XMLHelper::valueOf(values, "logLevel", DEFAULT_LOGLEVEL));
	listBoxDataSource->sortAndMark(Defaults::explode(XMLHelper::valueOf(values, "dataSource", DEFAULT_DATASOURCE), ','));
	auto randomColors(Defaults::explode(XMLHelper::valueOf(values, "randomColors", ""), ','));
	if (not randomColors.empty()) {
		DialogColors::getInstance()->populateColorBox(boxRandomColors, randomColors);
	}

	// DEFAULT_PROFILE
	// emitter
	inputColorsFile->set_active(XMLHelper::valueOf(values,    "colorsFile",   DEFAULT_COLORSINFO)   == "true");
	inputCraftProfiles->set_active(XMLHelper::valueOf(values, "craftProfile", DEFAULT_CRAFTPROFILE) == "true");
}

string MainWindow::readConfiguration() {
	unordered_map<string, string> r {
		// ledspicerd
		{"version", "1.0"},
		{"type",    "Configuration"},
		{"userId",  inputUserId->get_text()},
		{"userId",  inputUserId->get_text()},
		{"groupId", inputGroupId->get_text()},
		{"port",    inputPortNumber->get_text()},
		{"fps",     inputFPS->get_text()},
		{"logLevel", inputLogLevel->get_active_id()},
		{"colors",   inputColors->get_active_id()},
		{"dataSource",   Defaults::implode(listBoxDataSource->getCheckedValues(), ',')},
		{"randomColors", Defaults::implode(DialogColors::getInstance()->getColorBoxValues(boxRandomColors), ',')},
	};
	// emitter
	if (inputColorsFile->get_active()) {
		r.emplace("colorsFile", "true");
	}
	if (inputCraftProfiles->get_active()) {
		r.emplace("craftProfile", "true");
	}

	return XMLHelper::toXML(r);
}

void MainWindow::import(const string& dataFilePath, bool wipe, uint8_t importFlags) {
	if (importFlags & IMPORT_INPUTS) {
		InputFile datafile(dataFilePath);
		if (wipe) {
			inputs.wipe();
		}
		DataDialogs::DialogInput::getInstance()->load(&datafile);
		return;
	}

	ConfigFile datafile(dataFilePath);
	if (importFlags & IMPORT_CONFIG) {
		auto c(datafile.getSettings());
		// check if color are different.
		const string
			colorFile(c.count("colorsFile") ? c.at("colorsFile") : ""),
			previous(inputColors->get_active_id());
		if (not previous.empty() and previous != colorFile)
			Message::displayInfo("Warning\nColors definition file changed, element color changed");
		setConfiguration(c);
	}
	if (importFlags & IMPORT_DEVICES) {
		if (wipe) {
			devices.wipe();
			groups.wipe();
		}
		DataDialogs::DialogDevice::getInstance()->load(&datafile);
		DataDialogs::DialogGroup::getInstance()->load(&datafile);
		// TODO: set default profile

	}
	if (importFlags & IMPORT_RESTRICTORS) {
		if (wipe) {
			restrictors.wipe();
		}
		DataDialogs::DialogRestrictor::getInstance()->load(&datafile);
	}

	if (importFlags & IMPORT_MAPPINGS) {
		if (wipe) {
			processes.wipe();
		}
		DataDialogs::DialogProcess::getInstance()->load(&datafile);
		inputRunEvery->set_text(datafile.getProcessLookupRunEvery());
	}

	// todo: using default profile load inputs and animations.
	/*	if (wipe) {
		inputs.wipe();
	}*/
}

void MainWindow::setColorFile(const string& colorFile) {

	if (colorFile.empty()) {
		inputColors->set_active_id("");
		return;
	}
	try {
		XMLHelper datafile(dataDirectory + colorFile + ".xml", "Colors");
		inputColors->set_active_id(colorFile);
	}
	catch (Message& e) {
		e.displayError();
	}
}
