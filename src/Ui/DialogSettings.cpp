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
using LEDSpicerUI::Config::Settings;
using LEDSpicerUI::Config::SettingsFile;

bool DialogSettings::startup(Gtk::Window* parent) {
	try {
		if (loadSettings()) return true;
	}
	catch (Message& e) {
		if (SettingsFile::configExists()) {
			const string corrupt = SettingsFile::getSettingsPath() + ".corrupt";
			try {
				Gio::File::create_for_path(SettingsFile::getSettingsPath())
					->move(Gio::File::create_for_path(corrupt), Gio::FILE_COPY_OVERWRITE);
			}
			catch (...) {}
			Message::displayError(
				"The settings file is corrupt and was renamed to:\n" + corrupt +
				"\n\nStarting fresh.",
				parent
			);
		}
		Settings::get().load(Values{});
	}

	if (autoDetect()) {
		saveSettings();
		Message::displayInfo(
			"Welcome to LEDSpicerUI!\n\n"
			"LEDSpicer was detected and configured automatically.\n"
			"You can change settings anytime using the gear icon."
		);
		return true;
	}

	Gtk::MessageDialog dialog(
		*parent,
		"Welcome to LEDSpicerUI!",
		false,
		Gtk::MESSAGE_INFO,
		Gtk::BUTTONS_NONE,
		true
	);

	const string message = Settings::get().getBinaryPath().empty() ?
		"ledspicerd was not found in your system.\n\n"
		"You can:\n"
		"• Locate the binary manually if LEDSpicer is installed\n"
		"• Continue in portable mode (requires data directory)\n\n"
		"Note: In portable mode, ledspicer.conf is stored\n"
		"within the project directory."
		:
		"LEDSpicer was found but the data directory is missing or invalid.\n\n"
		"Please configure the data directory to continue.\n"
		"The data directory should contain color profiles, and optionally\n"
		"colors.ini, controls.ini, and gameData.xml.";

	dialog.set_secondary_text(message);
	dialog.add_button("Configure Settings", Gtk::RESPONSE_YES);
	dialog.add_button("Exit", Gtk::RESPONSE_NO);

	if (dialog.run() != Gtk::RESPONSE_YES)
		return false;

	dialog.hide();
	if (run() == Gtk::RESPONSE_APPLY) {
		saveSettings();
		return true;
	}
	if (not isValid()) {
		Message::displayError("Data directory is required. Application will close.");
		return false;
	}
	return true;
}

bool DialogSettings::autoDetect() {
	const string path = Glib::find_program_in_path("ledspicerd");
	if (path.empty()) return false;
	setBinaryPath(path, true);
	return isValid();
}

bool DialogSettings::loadSettings() {
	const bool loaded = SettingsFile::initialize();
	setBinaryPath(Settings::get().getBinaryPath(), true);
	setDataDir(Settings::get().getDataDir(), true);
	applyCurrentTheme();
	return loaded;
}

void DialogSettings::saveSettings() {
	try {
		SettingsFile::save();
	}
	catch (Message& e) {
		e.displayError();
	}
}

bool DialogSettings::isValid() const {
	return (
		not Settings::get().getDataDir().empty() and
		not Settings::get().getColorFiles().empty()
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

	builder->get_widget("SwitchSettingsInteractiveMode",    switchInteractiveMode);
	builder->get_widget("SwitchSettingsCleanProjectDir",    switchCleanProjectDir);
	builder->get_widget("SwitchSettingsPreserveEmptyDir",   switchPreserveEmptyDir);
	builder->get_widget("SwitchSettingsRemoveInvalidItems", switchRemoveInvalidItems);
	builder->get_widget("SwitchSettingsSaveBackup",         switchSaveBackup);
	builder->get_widget("SwitchSettingsDebugFiles",         switchDebugFiles);

	builder->get_widget("BtnSettingsStyleAuto",  btnStyleAuto);
	builder->get_widget("BtnSettingsStyleLight", btnStyleLight);
	builder->get_widget("BtnSettingsStyleDark",  btnStyleDark);
	builder->get_widget("BoxSelectTheme",        flowBoxThemes);

	// Sync all widgets to current Settings on every open.
	signal_show().connect([this]() {
		const auto& s = Settings::get();
		if (not s.getBinaryPath().empty())
			fileBinary->set_filename(s.getBinaryPath());
		if (not s.getDataDir().empty())
			fileDataDirSelect->set_filename(s.getDataDir());
		switchInteractiveMode->set_active(s.isInteractiveMode());
		switchCleanProjectDir->set_active(s.shouldCleanProjectDir());
		switchPreserveEmptyDir->set_active(s.shouldPreserveEmptyDir());
		switchRemoveInvalidItems->set_active(s.shouldRemoveInvalidItems());
		switchSaveBackup->set_active(s.shouldSaveBackup());
		switchDebugFiles->set_active(s.shouldDebugFiles());
		syncStyleButtons();
		if (flowBoxThemes->get_children().empty())
			populateThemes();
		selectingTheme = true;
		const string& currentId = s.getThemeName();
		for (auto w : flowBoxThemes->get_children()) {
			auto child = static_cast<Gtk::FlowBoxChild*>(w);
			if (child->get_name() == currentId) {
				flowBoxThemes->select_child(*child);
				break;
			}
		}
		selectingTheme = false;
	});

	// Style toggle buttons act as a mutual-exclusion radio group.
	auto connectStyleBtn = [this](Gtk::ToggleButton* btn, Settings::ThemeStyle style) {
		btn->signal_toggled().connect([this, btn, style]() {
			if (settingStyle) return;
			if (not btn->get_active()) {
				// Prevent the user from deselecting the active button.
				settingStyle = true;
				btn->set_active(true);
				settingStyle = false;
				return;
			}
			settingStyle = true;
			btnStyleAuto->set_active(style == Settings::ThemeStyle::Auto);
			btnStyleLight->set_active(style == Settings::ThemeStyle::Light);
			btnStyleDark->set_active(style == Settings::ThemeStyle::Dark);
			settingStyle = false;
			Settings::get().setThemeStyle(style);
			applyCurrentTheme();
		});
	};
	connectStyleBtn(btnStyleAuto,  Settings::ThemeStyle::Auto);
	connectStyleBtn(btnStyleLight, Settings::ThemeStyle::Light);
	connectStyleBtn(btnStyleDark,  Settings::ThemeStyle::Dark);

	// Theme tile selection.
	flowBoxThemes->signal_selected_children_changed().connect([this]() {
		if (selectingTheme) return;
		auto selected = flowBoxThemes->get_selected_children();
		if (selected.empty()) return;
		Settings::get().setThemeName(selected[0]->get_name());
		applyCurrentTheme();
	});

	switchInteractiveMode->property_active().signal_changed().connect([this]() {
		Settings::get().setInteractiveMode(switchInteractiveMode->get_active());
	});
	switchCleanProjectDir->property_active().signal_changed().connect([this]() {
		Settings::get().setCleanProjectDir(switchCleanProjectDir->get_active());
	});
	switchPreserveEmptyDir->property_active().signal_changed().connect([this]() {
		Settings::get().setPreserveEmptyDir(switchPreserveEmptyDir->get_active());
	});
	switchRemoveInvalidItems->property_active().signal_changed().connect([this]() {
		Settings::get().setRemoveInvalidItems(switchRemoveInvalidItems->get_active());
	});
	switchSaveBackup->property_active().signal_changed().connect([this]() {
		Settings::get().setSaveBackup(switchSaveBackup->get_active());
	});
	switchDebugFiles->property_active().signal_changed().connect([this]() {
		Settings::get().setDebugFiles(switchDebugFiles->get_active());
	});

	fileBinary->signal_file_set().connect([this]() {
		setBinaryPath(fileBinary->get_filename(), false);
	});

	fileDataDirSelect->signal_file_set().connect([this]() {
		setDataDir(fileDataDirSelect->get_filename(), false);
	});

	// Open this dialog when the settings button is clicked; save on Apply.
	Gtk::Button* btnSettings = nullptr;
	builder->get_widget("BtnSettings", btnSettings);
	btnSettings->signal_clicked().connect([this]() {
		if (run() == Gtk::RESPONSE_APPLY)
			saveSettings();
		hide();
	});
}

bool DialogSettings::detectLedspicerVersion() {

	const string& binary = Settings::get().getBinaryPath();
	if (binary.empty()) {
		updateBinaryStatusLabel("");
		return false;
	}

	string outputText, ledspicerVer;
	if (not Defaults::runCommand(binary + " -v", outputText)) {
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
	Settings::get().setBinaryPath(binaryPath);
	if (detectLedspicerVersion()) {
		processBinary();
		if (setFileBinarySelector) fileBinary->set_filename(binaryPath);
	}
}

void DialogSettings::updateBinaryStatusLabel(const string& version) {
	const bool detected = not version.empty();
	if (not detected) {
		Settings::get().setBinaryPath("");
		Settings::get().setConfigPath("");
		labelBinaryStatus->set_text("Status: ❌ Not detected");
		labelBinaryPath->set_text("N/A");
		labelConfigPath->set_text("Will be saved inside the project directory");
	}
	else {
		labelBinaryStatus->set_text("Status: ✅ Detected (" + version + ")");
		labelBinaryPath->set_text(Settings::get().getBinaryPath());
	}
	switchInteractiveMode->set_sensitive(detected);
	// Mode is derived automatically by Settings from binaryPath state.
}

void DialogSettings::processBinary() {

	const string& binary = Settings::get().getBinaryPath();
	if (binary.empty()) {
		updateBinaryStatusLabel("");
		return;
	}

	string outputText;
	if (not Defaults::runCommand(binary + " -h", outputText)) {
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
			Settings::get().setProjectsDir(val);
			continue;
		}

		val = Defaults::extractAfter(line, "will use ");
		if (not val.empty()) {
			setConfigPath(val);
		}
	}
}

void DialogSettings::setConfigPath(const string& configPath) {

	if (configPath.empty()) {
		Settings::get().setConfigPath("");
		labelConfigPath->set_text("N/A");
		return;
	}

	bool isWritable = false;
	try {
		auto file = Gio::File::create_for_path(configPath);
		auto info = file->query_info("access::can-write");
		isWritable = info->get_attribute_boolean("access::can-write");
	}
	catch (...) {
		Settings::get().setConfigPath("");
		return;
	}

	Settings::get().setConfigPath(configPath);
	labelConfigPath->set_text((isWritable ? "" : "🔒") + configPath);
}

void DialogSettings::setDataDir(const string& dataDir, bool setFileDataDirSelector) {

	Settings::get().setDataDirStatus(false, false, false);
	Settings::get().setColorFiles({});

	if (dataDir.empty()) {
		Settings::get().setDataDir("");
		fileDataDirSelect->set_current_folder(Glib::get_home_dir());
	}
	else {
		const string normalized = dataDir + (dataDir.back() != '/' ? "/" : "");
		Settings::get().setDataDir(normalized);
		if (setFileDataDirSelector) fileDataDirSelect->set_filename(normalized);
		processDataDir();
	}
	updateDataDirLabels();
	updateApplyButton();
}

void DialogSettings::updateDataDirLabels() {
	const auto& s = Settings::get();
	string status;
	status += s.getHasColors()          ? "✅" : "❌";
	status += " colors.ini    ";
	status += s.getHasGameData()        ? "✅" : "❌";
	status += " gameData.xml    ";
	status += s.getHasControls()        ? "✅" : "❌";
	status += " controls.ini    ";
	status += s.getColorFiles().empty() ? "❌" : "✅";
	status += " Color profiles    ";
	labelSystemFiles->set_text(status);
	labelDataPath->set_text(s.getDataDir().empty() ? "N/A" : s.getDataDir());
}

void DialogSettings::processDataDir() {

	const string& dataDir = Settings::get().getDataDir();
	if (dataDir.empty()) return;

	auto directory = Gio::File::create_for_path(dataDir);
	Glib::RefPtr<Gio::FileEnumerator> enumerator;
	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error& e) {
		Message::displayError(e.what(), this);
		return;
	}

	bool hasGameData = false, hasColors = false, hasControls = false;
	StringVector colorFiles;

	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		if (fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY) continue;

		const string filename {fileInfo->get_name()};

		if      (filename == "gameData.xml") { hasGameData = true; continue; }
		else if (filename == "colors.ini")   { hasColors   = true; continue; }
		else if (filename == "controls.ini") { hasControls = true; continue; }

		auto parts = Defaults::explode(filename, '.');
		if (parts.size() < 2) continue;
		if (parts.back() != "xml") continue;

		try {
			XMLHelper testCol(dataDir + filename, "Colors");
			parts.pop_back();
			colorFiles.push_back(Defaults::implode(parts, '.'));
		}
		catch (...) {}
	}

	Settings::get().setDataDirStatus(hasGameData, hasColors, hasControls);
	Settings::get().setColorFiles(std::move(colorFiles));
}

void DialogSettings::updateApplyButton() {
	btnApply->set_sensitive(not Settings::get().getColorFiles().empty());
}

void DialogSettings::populateThemes() {
	for (const auto& meta : ThemeManager::getInstance().getThemes()) {
		auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4));

		auto img = Gtk::manage(new Gtk::Image());
		img->set_size_request(120, 80);
		const string imgPath{PACKAGE_DATA_DIR "themes/" + meta.id + "/preview.png"};
		try {
			img->set(Gdk::Pixbuf::create_from_file(imgPath, 120, 80, true));
		}
		catch (const Glib::Error&) {
			img->set_from_icon_name("image-missing", Gtk::ICON_SIZE_DIALOG);
			img->set_pixel_size(64);
		}
		box->pack_start(*img, false, false);

		auto lbl = Gtk::manage(new Gtk::Label(meta.name));
		box->pack_start(*lbl, false, false);
		box->show_all();

		auto child = Gtk::manage(new Gtk::FlowBoxChild());
		child->set_name(meta.id);
		child->get_style_context()->add_class("ThemeTile");
		if (not meta.description.empty())
			child->set_tooltip_text(meta.description);
		child->add(*box);
		child->show();
		flowBoxThemes->add(*child);
	}
}

void DialogSettings::syncStyleButtons() {
	settingStyle = true;
	const auto style = Settings::get().getThemeStyle();
	btnStyleAuto->set_active(style  == Settings::ThemeStyle::Auto);
	btnStyleLight->set_active(style == Settings::ThemeStyle::Light);
	btnStyleDark->set_active(style  == Settings::ThemeStyle::Dark);
	settingStyle = false;
}

void DialogSettings::applyCurrentTheme() {
	ThemeManager::getInstance().apply(
		Settings::get().getThemeName(),
		Settings::get().getThemeStyle()
	);
}
