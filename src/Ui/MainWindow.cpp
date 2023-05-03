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

bool MainWindow::dirty = false;

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	Gtk::ApplicationWindow(obj)
{

	builder->get_widget("Header", header);

	// Setup fields.
	builder->get_widget("InputUserId",     InputUserId);
	builder->get_widget("InputGroupId",    InputGroupId);
	builder->get_widget("InputPortNumber", InputPortNumber);
	builder->get_widget("InputFPS",        InputFPS);
	builder->get_widget("InputColors",     InputColors);
	builder->get_widget("InputLogLevel",   InputLogLevel);

	// Initialize Dialog Colors.
	Gtk::Button* btnAddRandomColor;
	Gtk::FlowBox* boxRandomColors;
	builder->get_widget("BtnAddRandomColor", btnAddRandomColor);
	builder->get_widget("BoxRandomColors",   boxRandomColors);
	DialogColors::getInstance(builder)->activateColorPicker(btnAddRandomColor, boxRandomColors);

	// Pin Handler.
	Forms::PinHandler::getInstance(builder);

	// Dialog Elements.
	DialogElement::getInstance(builder);

	// Dialog Select Elements.
	DialogSelectElements::getInstance(builder);

	// Dialog device.
	builder->get_widget_derived("DialogDevice", dd);

	// Dialog device.
	builder->get_widget_derived("DialogRestrictor", dr);

	// Connect Group Box and Dialog.
	builder->get_widget_derived("DialogGroup", dg);

	// Setup top buttons
	Gtk::Button
		* btnAbout,
		* btnOpenProject,
		* btnImportConfig;

	builder->get_widget("BtnSelectDir",   btnOpenProject);
	builder->get_widget("BtnSaveProject", btnSaveProject);
	builder->get_widget("BtnAbout",       btnAbout);
	Gtk::FileChooserButton* dataDirSelect = nullptr;
	builder->get_widget("dataDirSelect",  dataDirSelect);

	Gtk::Fixed* FixedLayout;
	builder->get_widget("FixedLayout",  FixedLayout);

	// Create select work directory FileChooserDialog
	prepareWorkingDirectory(builder);

	// Import config file.
	builder->get_widget("BtnImportConfig", btnImportConfig);

	// Data selector action.
	dataDirSelect->set_current_folder(PACKAGE_DATA_DIR"..");
	dataDirSelect->signal_file_set().connect([&, btnOpenProject, dataDirSelect] () {
		openDataDirectory(dataDirSelect->get_filename() + "/");
		btnOpenProject->set_sensitive(true);
	});

	// Save project
	btnSaveProject->signal_clicked().connect([]() {

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

	// Data sources
	builder->get_widget("InputColorsFile",    InputColorsFile);
	builder->get_widget("InputCraftProfiles", InputCraftProfiles);
	builder->get_widget("ListBoxDatasource",  ListBoxDataSource);
	Gtk::Button
		* BtnDatasourceUp   = nullptr,
		* BtnDatasourceDown = nullptr;
	builder->get_widget("BtnDatasourceUp",   BtnDatasourceUp);
	builder->get_widget("BtnDatasourceDown", BtnDatasourceDown);
	// When a row is selected handle buttons for sanity.
	ListBoxDataSource->signal_row_selected().connect([=](Gtk::ListBoxRow* row) {
		if (!row)
			return;
		int index = row->get_index();
		BtnDatasourceUp->set_sensitive(index);
		BtnDatasourceDown->set_sensitive(index != ListBoxDataSource->get_children().size() - 1);
	});

	BtnDatasourceUp->signal_clicked().connect([=]() {
		auto selectedRow = ListBoxDataSource->get_selected_row();
		auto index = selectedRow->get_index();
		ListBoxDataSource->remove(*selectedRow);
		ListBoxDataSource->insert(*selectedRow, index - 1);
		ListBoxDataSource->unselect_all();
		ListBoxDataSource->select_row(*selectedRow);
	});

	BtnDatasourceDown->signal_clicked().connect([=]() {
		auto selectedRow = ListBoxDataSource->get_selected_row();
		auto index = selectedRow->get_index();
		ListBoxDataSource->remove(*selectedRow);
		ListBoxDataSource->insert(*selectedRow, index + 1);
		ListBoxDataSource->unselect_all();
		ListBoxDataSource->select_row(*selectedRow);
	});

	InputColors->signal_changed().connect([&]() {
		DialogColors::getInstance()->setColorsFromFile(dataDirectory + InputColors->get_active_id() + ".xml");
	});
}

MainWindow::~MainWindow() {
	// derived classes.
	delete dg;
	delete dd;
	delete dr;
	delete DialogElement::getInstance();
	delete DialogSelectElements::getInstance();
	delete DialogColors::getInstance();
	delete Forms::PinHandler::getInstance();
	delete dds;
	Forms::CollectionHandler::wipe();
}

void MainWindow::openDataDirectory(const string& dataDirectory) {
	// Set color list.
	InputColors->remove_all();
	InputColors->append("", "Select Colors");
	string msg;
	try {
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

			string filename = file_info->get_name();
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
			auto parts = XMLHelper::explode(filename, '.');
			string ext(parts.back());
			if (ext == "xml") {
				parts.pop_back();
				string name(XMLHelper::implode(parts, '.'));
				try {
					XMLHelper testCol(dataDirectory + filename, "Colors");
					msg += "Colors file " + filename + " Found\n";
					InputColors->append(name, name);
				}
				catch(...) {}
			}
		}
	}
	catch(Message& e) {
		e.displayError(this);
		return;
	}
	catch(const Glib::Error& ex) {
		Message::displayError(ex.what(), this);
		return;
	}
	if (not msg.empty()) {
		Message::displayInfo(msg, this);
		InputColors->set_sensitive(true);
	}
	else {
		Message::displayInfo("The directory does not contain any useful file\nSome features may be disabled.", this);
		InputColors->set_sensitive(false);
	}
}

void MainWindow::prepareWorkingDirectory(Glib::RefPtr<Gtk::Builder> const &builder) {
	// Create select work directory FileChooserDialog
	dds = new Gtk::FileChooserDialog("Select Project Directory", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	dds->set_modal(true);
	dds->set_transient_for(*this);
	dds->set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
	dds->set_default_size(900, 700);

	// prepare open project dialog functionality.
	Gtk::Button* btnCancel = dds->add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	Gtk::Button* btnOk     = dds->add_button("Select", Gtk::ResponseType::RESPONSE_OK);
	btnCancel->get_style_context()->add_class("backgroundRed");
	btnOk->get_style_context()->add_class("backgroundGreen");

	Gtk::Button* btnOpenProject;
	builder->get_widget("BtnSelectDir", btnOpenProject);
	btnOpenProject->signal_clicked().connect([&, builder]() {
		if (dds->run() != Gtk::ResponseType::RESPONSE_OK) {
			dds->hide();
			return;
		}
		// Activate configuration tabs.
		Gtk::Notebook* MainTabs = nullptr;
		builder->get_widget("MainTabs", MainTabs);

		string newPath = dds->get_file()->get_path();
		if (newPath == workingDirectory) {
			Message::displayInfo("Already working on that project", dds);
			dds->hide();
			return;
		}
		if (dirty and Message::ask("All unsaved changes will be loss, are you sure you want to", dds) != Gtk::RESPONSE_OK) {
			dds->hide();
			return;
		}
		// set working directory
		workingDirectory = newPath;
		setHeader();
		// set values.
		unordered_map<string, string> values;
		try {
			// Old.
			values = import(workingDirectory + "/ledspicer.conf", true);
		}
		catch (Message& e) {
			// New.
			values = {
				{"craftProfile", "true"},
				{"colorsFile",   "true"},
				{"dataSource",   "file, mame"}
			};
		}
		setConfiguration(values);
		btnSaveProject->set_sensitive(false);
		MainTabs->set_sensitive(true);
		dds->hide();
	});
}

void MainWindow::setConfiguration(unordered_map<string, string>& values) {
	// ledspicerd
	InputUserId->set_text(XMLHelper::valueOf(values,        "userId",   "1000"));
	InputGroupId->set_text(XMLHelper::valueOf(values,       "groupId",  "1000"));
	InputPortNumber->set_text(XMLHelper::valueOf(values,    "port",     "16161"));
	InputFPS->set_text(XMLHelper::valueOf(values,           "fps",      "30"));
	setColorFile(XMLHelper::valueOf(values,                 "colors",   ""));
	InputLogLevel->set_active_id(XMLHelper::valueOf(values, "logLevel", "Info"));
	// emitter
	InputColorsFile->set_active(XMLHelper::valueOf(values,    "colorsFile",   "false") == "true");
	InputCraftProfiles->set_active(XMLHelper::valueOf(values, "craftProfile", "false") == "true");

	// process lookup

	//InputRunEvery InputProcessLookupMappings
}

unordered_map<string, string> MainWindow::import(const string& ledspicerconf, bool wipe) {
	ConfigFile datafile(ledspicerconf);
	for (auto i : *dd->createItems(datafile.getDevices(), wipe)) {
		DialogElement::getInstance()->createItems(datafile.getDeviceElements(i->getForm()->createName()), i, wipe);
	}
	for (auto i : *dg->createItems(datafile.getGroups(), wipe)) {
		DialogSelectElements::getInstance()->createItems(datafile.getGroupElements(i->getForm()->createName()), i, wipe);
	}
	// TODO: set default profile
	// TODO: read restrictors.
	return datafile.getSettings();
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
		e.displayError(this);
	}
}

bool MainWindow::isDirty() {
	//container->signal_notify_property().connect(sigc::mem_fun(*this, &MyClass::on_container_child_changed));
	return dirty;
}

void MainWindow::setHeader() {
	header->set_subtitle((isDirty() ? "* " : "") + workingDirectory);
}
