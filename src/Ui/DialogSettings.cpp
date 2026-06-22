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

const string
	savedMessage      {"Settings saved"},
	themeSavedMessage {"Theme updated"};

bool DialogSettings::startup(Gtk::Window* parent) {
	try {
		if (loadSettings()) return true;
	}
	catch (const Message& e) {
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
	run();
	hide();
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
	// Rescan so applyCurrentTheme() validates the saved theme against disk.
	ThemeManager::getInstance().rescan();
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

	add_button("_Close", Gtk::ResponseType::RESPONSE_CLOSE);

	builder->get_widget("SwitchSettingsInteractiveMode",    switchInteractiveMode);
	builder->get_widget("SwitchSettingsPreserveEmptyDir",   switchPreserveEmptyDir);
	builder->get_widget("SwitchSettingsRemoveInvalidItems", switchRemoveInvalidItems);
	builder->get_widget("SwitchSettingsSaveBackup",         switchSaveBackup);
	builder->get_widget("SwitchSettingsDebugFiles",         switchDebugFiles);
	builder->get_widget("SwitchSettingsDebugDaemon",        switchDebugDaemon);

	builder->get_widget("BtnSettingsStyleAuto",  btnStyleAuto);
	builder->get_widget("BtnSettingsStyleLight", btnStyleLight);
	builder->get_widget("BtnSettingsStyleDark",  btnStyleDark);
	builder->get_widget("BoxSelectTheme",        flowBoxThemes);
	builder->get_widget("FileThemeDirSelect",    fileThemeDirSelect);
	builder->get_widget("BtnThemeRefresh",       btnThemeRefresh);

	builder->get_widget("ScaleLayoutGrid", scaleLayoutGrid);
	for (int v {0}; v <= 100; v += 10)
		scaleLayoutGrid->add_mark(v, Gtk::POS_BOTTOM, "");

	builder->get_widget("SpinLayoutTestTimeout", spinLayoutTestTimeout);

	// Sync all widgets to current Settings on every open.
	signal_show().connect([this]() {
		syncing = true;
		const auto& s = Settings::get();
		if (not s.getBinaryPath().empty())
			fileBinary->set_filename(s.getBinaryPath());
		if (not s.getDataDir().empty())
			fileDataDirSelect->set_filename(s.getDataDir());
		switchInteractiveMode->set_active(s.isInteractiveMode());
		switchPreserveEmptyDir->set_active(s.shouldPreserveEmptyDir());
		switchRemoveInvalidItems->set_active(s.shouldRemoveInvalidItems());
		switchSaveBackup->set_active(s.shouldSaveBackup());
		switchDebugFiles->set_active(s.shouldDebugFiles());
		switchDebugDaemon->set_active(s.shouldDebugDaemon());
		scaleLayoutGrid->set_value(s.getLayoutGrid());
		spinLayoutTestTimeout->set_value(s.getLayoutTestTimeout() / 1000.0);
		syncStyleButtons();
		const string themeDir {s.getThemePath()};
		if (not Glib::file_test(themeDir, Glib::FILE_TEST_IS_DIR)) {
			try { Gio::File::create_for_path(themeDir)->make_directory_with_parents(); }
			catch (...) {}
		}
		fileThemeDirSelect->set_filename(themeDir);
		selectingTheme = true;
		rebuildThemeTiles();
		selectCurrentThemeTile();
		selectingTheme = false;
		syncing = false;
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
			commit(themeSavedMessage);
		});
	};
	connectStyleBtn(btnStyleAuto,  Settings::ThemeStyle::Auto);
	connectStyleBtn(btnStyleLight, Settings::ThemeStyle::Light);
	connectStyleBtn(btnStyleDark,  Settings::ThemeStyle::Dark);

	// Theme tile selection. Clicking a tile selects its theme; clicking the
	// already-selected tile clears the selection (no theme).
	flowBoxThemes->signal_child_activated().connect([this](Gtk::FlowBoxChild* child) {
		if (selectingTheme) return;
		const string clicked  {child->get_name()};
		const string previous {Settings::get().getThemeName()};
		if (clicked == previous) {
			selectingTheme = true;
			flowBoxThemes->unselect_all();
			selectingTheme = false;
			Settings::get().setThemeName("");
			applyCurrentTheme();
			commit("Theme cleared");
			return;
		}
		Settings::get().setThemeName(clicked);
		applyCurrentTheme();
		commit(themeSavedMessage);
	});

	fileThemeDirSelect->signal_file_set().connect([this]() {
		string dir {fileThemeDirSelect->get_filename()};
		if (not dir.empty() and dir.back() != '/') dir += '/';
		Settings::get().setThemePath(dir);
		refreshThemes();
		commit("Themes directory updated");
	});

	btnThemeRefresh->signal_clicked().connect([this]() {
		refreshThemes();
		commit("Themes rescanned");
	});

	switchInteractiveMode->property_active().signal_changed().connect([this]() {
		Settings::get().setInteractiveMode(switchInteractiveMode->get_active());
		commit(savedMessage);
	});
	switchPreserveEmptyDir->property_active().signal_changed().connect([this]() {
		Settings::get().setPreserveEmptyDir(switchPreserveEmptyDir->get_active());
		commit(savedMessage);
	});
	switchRemoveInvalidItems->property_active().signal_changed().connect([this]() {
		Settings::get().setRemoveInvalidItems(switchRemoveInvalidItems->get_active());
		commit(savedMessage);
	});
	switchSaveBackup->property_active().signal_changed().connect([this]() {
		Settings::get().setSaveBackup(switchSaveBackup->get_active());
		commit(savedMessage);
	});
	switchDebugFiles->property_active().signal_changed().connect([this]() {
		Settings::get().setDebugFiles(switchDebugFiles->get_active());
		commit(savedMessage);
	});
	switchDebugDaemon->property_active().signal_changed().connect([this]() {
		Settings::get().setDebugDaemon(switchDebugDaemon->get_active());
		commit(savedMessage);
	});

	scaleLayoutGrid->signal_change_value().connect(
		[this](Gtk::ScrollType, double value) -> bool {
			scaleLayoutGrid->set_value(std::round(value / 10.0) * 10.0);
			return true;
		}
	);
	scaleLayoutGrid->signal_value_changed().connect([this]() {
		Settings::get().setLayoutGrid(static_cast<int>(scaleLayoutGrid->get_value()));
		commit(savedMessage);
	});

	spinLayoutTestTimeout->signal_value_changed().connect([this]() {
		Settings::get().setLayoutTestTimeout(spinLayoutTestTimeout->get_value());
		commit(savedMessage);
	});

	fileBinary->signal_file_set().connect([this]() {
		setBinaryPath(fileBinary->get_filename(), false);
		commit("Binary path updated");
	});

	fileDataDirSelect->signal_file_set().connect([this]() {
		setDataDir(fileDataDirSelect->get_filename(), false);
		if (Settings::get().getColorFiles().empty())
			commit("Data directory invalid", StatusBar::Severity::Warning);
		else
			commit("Data directory updated");
	});

	// Open this dialog when the settings button is clicked.
	Gtk::Button* btnSettings = nullptr;
	builder->get_widget("BtnSettings", btnSettings);
	btnSettings->signal_clicked().connect([this]() {
		run();
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

void DialogSettings::commit(const string& message, StatusBar::Severity severity) {
	if (syncing) return;
	saveSettings();
	StatusBar::getInstance().push(message, severity);
}

void DialogSettings::rebuildThemeTiles() {
	for (auto child : flowBoxThemes->get_children())
		flowBoxThemes->remove(*child);

	// No "None" tile: an empty selection (nothing highlighted) is the no-theme
	// state. Clicking the selected tile again clears it (see signal_child_activated).
	const string& themeDir = Settings::get().getThemePath();
	for (const auto& meta : ThemeManager::getInstance().getThemes())
		addThemeTile(meta.id, meta.name, themeDir + meta.id + "/preview.png", meta.description);
}

void DialogSettings::addThemeTile(const string& id, const string& name, const string& previewPath, const string& tooltip) {
	auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4));

	auto img = Gtk::manage(new Gtk::Image());
	img->set_size_request(120, 80);
	try {
		img->set(Gdk::Pixbuf::create_from_file(previewPath, 120, 80, true));
	}
	catch (const Glib::Error&) {
		img->set_from_icon_name("image-missing", Gtk::ICON_SIZE_DIALOG);
		img->set_pixel_size(64);
	}
	box->pack_start(*img, false, false);

	auto lbl = Gtk::manage(new Gtk::Label(name));
	box->pack_start(*lbl, false, false);
	box->show_all();

	auto child = Gtk::manage(new Gtk::FlowBoxChild());
	child->set_name(id);
	child->get_style_context()->add_class("ThemeTile");
	if (not tooltip.empty())
		child->set_tooltip_text(tooltip);
	child->add(*box);
	child->show();
	flowBoxThemes->add(*child);
}

void DialogSettings::selectCurrentThemeTile() {
	const string& currentId = Settings::get().getThemeName();
	if (currentId.empty()) {
		flowBoxThemes->unselect_all();
		return;
	}
	for (auto w : flowBoxThemes->get_children()) {
		auto child = static_cast<Gtk::FlowBoxChild*>(w);
		if (child->get_name() == currentId) {
			flowBoxThemes->select_child(*child);
			return;
		}
	}
}

void DialogSettings::refreshThemes() {
	ThemeManager::getInstance().rescan();

	// A selection whose theme directory vanished falls back to no theme.
	const string current {Settings::get().getThemeName()};
	if (not current.empty() and not ThemeManager::getInstance().hasTheme(current)) {
		Settings::get().setThemeName("");
		applyCurrentTheme();
		commit("Theme \"" + current + "\" not found; cleared", StatusBar::Severity::Warning);
	}

	selectingTheme = true;
	rebuildThemeTiles();
	selectCurrentThemeTile();
	selectingTheme = false;
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
	auto& tm = ThemeManager::getInstance();
	const string id {Settings::get().getThemeName()};
	// A saved theme whose files were removed falls back to no theme.
	if (not id.empty() and not tm.hasTheme(id))
		Settings::get().setThemeName("");
	tm.apply(Settings::get().getThemeName(), Settings::get().getThemeStyle());
}
