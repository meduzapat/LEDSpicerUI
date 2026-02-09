/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSettings.cpp
 * @since     Feb 6, 2026
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

#include "DialogSettings.hpp"

using namespace LEDSpicerUI::Ui;

const string& DialogSettings::getBinaryPath() const {
	return binaryPath;
}

const string& DialogSettings::getDataDir() const {
	return dataDir;
}

const string& DialogSettings::getConfigPath() const {
	return configPath;
}

const StringVector& DialogSettings::getColorFiles() const {
	return dataDirStatus.colorFiles;
}

bool DialogSettings::loadSettings() {

	if (not SettingsFile::configExists()) return false;

	try {
		SettingsFile config(SettingsFile::getConfigFilePath());
		StringUMap settings = config.getSettings();

		setBinaryPath(XMLHelper::valueOf(settings, "binaryPath", ""), true);
		setDataDir(XMLHelper::valueOf(settings, "dataDir", ""), true);

		// Set projects dir, if not set use the default one.
		string projectsDir = XMLHelper::valueOf(settings, "projectsDir", "");
		Defaults::setProjectsDir(
			projectsDir.empty() ?
			Glib::get_user_data_dir() + "/" PACKAGE_NAME "/projects/" :
			projectsDir
		);
	}
	catch (Message& e) {
		e.displayError();
		return false;
	}
	return true;
}

void DialogSettings::saveSettings() {
	StringUMap settings {
		{"binaryPath",  binaryPath},
		{"dataDir",     dataDir},
		{"projectsDir", Defaults::getProjectsDir()}
	};
	try {
		SettingsFile::save(SettingsFile::getConfigFilePath(), settings);
	}
	catch (Message& e) {
		e.displayError();
	}
}

bool DialogSettings::isValid() const {
	return (
		not dataDir.empty() and
		not dataDirStatus.colorFiles.empty()
	);
}

DialogSettings::DialogSettings(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	GladeDialog(obj, builder)
{
	builder->get_widget("FileBinary",        fileBinary);
	builder->get_widget("FileDataDirSelect", fileDataDirSelect);
	builder->get_widget("LabelBinaryPath",   labelBinaryPath);
	builder->get_widget("LabelBinaryStatus", labelBinaryStatus);
	builder->get_widget("LabelDataPath",     labelDataPath);
	builder->get_widget("LabelSystemFiles",  labelSystemFiles);
	builder->get_widget("LabelConfigPath",   labelConfigPath);
	builder->get_widget("BtnApplySettings",  btnApply);

	// Binary selection
	fileBinary->signal_file_set().connect([this]() {
		setBinaryPath(fileBinary->get_filename(), false);
	});

	// Data dir selection
	fileDataDirSelect->signal_file_set().connect([this]() {
		setDataDir(fileDataDirSelect->get_filename(), false);
	});
}

void DialogSettings::processBinary() {

	if (binaryPath.empty()) {
		updateBinaryStatusLabel("");
		return;
	}

	string outputText;
	if (not Defaults::runCommand(binaryPath + " -h", outputText)) {
		updateBinaryStatusLabel("");
		return;
	}

	auto lines = Defaults::explode(outputText, '\n');
	if (lines.empty()) {
		updateBinaryStatusLabel("");
		return;
	}

	for (const auto& line : lines) {
		string val;

		val = Defaults::extractAfter(line, "Data dir:");
		if (not val.empty()) {
			setDataDir(val, true);
			continue;
		}

		val = Defaults::extractAfter(line, "Projects dir:");
		if (not val.empty()) {
			Defaults::setProjectsDir(val);
			continue;
		}

		val = Defaults::extractAfter(line, "will use ");
		if (not val.empty()) {
			setConfigPath(val);
		}
	}
}

bool DialogSettings::detectLedspicerVersion() {

	if (binaryPath.empty()) {
		updateBinaryStatusLabel("");
		return false;
	}

	string outputText, ledspicerVer;
	if (not Defaults::runCommand(binaryPath + " -v", outputText)) {
		updateBinaryStatusLabel("");
		return false;
	}
	ledspicerVer = Defaults::extractAfter(outputText, "LEDSpicer");
	if (ledspicerVer.empty()) {
		Message::displayError("The selected binary does not appear to be ledspicerd.", this);
		updateBinaryStatusLabel("");
		return false;
	}
	auto parts = Defaults::explode(ledspicerVer, ' ');
	if (parts.size() > 1) ledspicerVer = parts[0];
	updateBinaryStatusLabel(ledspicerVer);
	return true;
}

void DialogSettings::setBinaryPath(const string& binaryPath, bool setFileBinarySelector) {
	this->binaryPath = binaryPath;
	if (detectLedspicerVersion()) {
		processBinary();
		if (setFileBinarySelector) fileBinary->set_filename(binaryPath);
	}
//	updateBinaryStatusLabel(binaryPath);
}

void DialogSettings::updateBinaryStatusLabel(const string& version) {
	// Binary not detected or invalid.
	if (version.empty()) {
		binaryPath = "";
		configPath = "";
		labelBinaryStatus->set_text("Status: ❌ Not detected");
		labelBinaryPath->set_text("N/A");
		labelConfigPath->set_text("Will be saved inside the project directory");
	}
	else {
		labelBinaryStatus->set_text("Status: ✅ Detected (" + version + ")");
		labelBinaryPath->set_text(binaryPath);
	}
}

void DialogSettings::setConfigPath(const string& configPath) {
	bool isWritable = false;
	try {
		auto file = Gio::File::create_for_path(configPath);
		auto info = file->query_info("access::can-write");
		isWritable = info->get_attribute_boolean("access::can-write");
	}
	catch (...) {
		this->configPath = "";
		return;
	}

	this->configPath = configPath;
	if (configPath.empty()) {
		labelConfigPath->set_text("N/A");
		return;
	}


	labelConfigPath->set_text((isWritable ? "" : "🔒") + configPath);
}

void DialogSettings::setDataDir(const string &dataDir, bool setFileDataDirSelector) {
	if (dataDir.empty()) {
		this->dataDir = "";
		fileDataDirSelect->set_current_folder(Glib::get_home_dir());
	}
	else {
		this->dataDir = dataDir + (dataDir.back() != '/' ? "/" : "");
		if (setFileDataDirSelector) fileDataDirSelect->set_filename(this->dataDir);
		processDataDir();
	}
	updateDataDirLabels();
	updateApplyButton();
}

void DialogSettings::updateDataDirLabels() {
	string status;
	status += dataDirStatus.hasColors     ? "✅" : "❌";
	status += " colors.ini    ";
	status += dataDirStatus.hasGameData   ? "✅" : "❌";
	status += " gameData.xml    ";
	status += dataDirStatus.hasControls   ? "✅" : "❌";
	status += " controls.ini    ";
	status += dataDirStatus.colorFiles.empty() ? "❌" : "✅";
	status += " Color profiles    ";
	labelSystemFiles->set_text(status);
	labelDataPath->set_text(this->dataDir.empty() ? "N/A" : this->dataDir);
}

void DialogSettings::processDataDir() {

	dataDirStatus = {};

	if (dataDir.empty()) return;

	auto directory = Gio::File::create_for_path(dataDir);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;
	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error& e) {
		Message::displayError(e.what());
		return;
	}

	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		if (fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY) continue;

		string filename(fileInfo->get_name());

		if (filename == "gameData.xml") {
			dataDirStatus.hasGameData = true;
			continue;
		}
		else if (filename == "colors.ini") {
			dataDirStatus.hasColors = true;
			continue;
		}
		else if (filename == "controls.ini") {
			dataDirStatus.hasControls = true;
			continue;
		}

		auto parts = Defaults::explode(filename, '.');
		if (parts.size() < 2) continue;

		string ext(parts.back());
		if (ext != "xml") continue;

		try {
			XMLHelper testCol(dataDir + filename, "Colors");
			parts.pop_back();
			dataDirStatus.colorFiles.push_back(Defaults::implode(parts, '.'));
		}
		catch (...) {}
	}
}

void DialogSettings::updateApplyButton() {
	btnApply->set_sensitive(not dataDirStatus.colorFiles.empty());
}
