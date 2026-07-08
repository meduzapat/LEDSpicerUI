/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProject.cpp
 * @since     Feb 1, 2026
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

#include "DialogProject.hpp"

using namespace LEDSpicerUI::Ui;
using LEDSpicerUI::Config::Settings;

DialogProject::DialogProject(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) :
	GladeDialog(obj, builder)
{

	// Get widgets
	builder->get_widget("LabelProjectsPath",        labelProjectsPath);
	builder->get_widget("FileProjectsDirSelect",    fileProjectsDirSelect);
	builder->get_widget("ComboSelectProject",       comboSelectProject);
	builder->get_widget("InputNewProjectName",      inputNewProjectName);
	builder->get_widget("ToggleNewProjectPortable", toggleNewProjectPortable);
	builder->get_widget("BoxProjectActions",        boxProjectActions);
	builder->get_widget("BoxNewProjectName",        boxNewProjectName);
	builder->get_widget("BoxProjectConvert",        boxProjectConvert);
	builder->get_widget("BtnProjectMakePortable",   btnProjectMakePortable);
	builder->get_widget("BtnProjectDeploy",         btnProjectDeploy);
	builder->get_widget("BtnProjectUseSystem",      btnProjectUseSystem);

	btnProjectMakePortable->signal_clicked().connect([this]() { makePortable();     });
	btnProjectDeploy->signal_clicked().connect([this]()       { deployToSystem();   });
	btnProjectUseSystem->signal_clicked().connect([this]()    { useSystemConfig();  });

	add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	btnApply = add_button("_Apply", Gtk::ResponseType::RESPONSE_APPLY);

	fileProjectsDirSelect->signal_file_set().connect([this]() {
		setProjectsDir(fileProjectsDirSelect->get_filename(), false);
	});

	// Select project button
	comboSelectProject->signal_changed().connect([this]() {
		const string name {comboSelectProject->get_active_id()};
		if (name.empty()) {
			boxNewProjectName->set_visible(true);
			inputNewProjectName->set_text("");
			inputNewProjectName->grab_focus();
		}
		else {
			boxNewProjectName->set_visible(false);
			toggleNewProjectPortable->set_active(false);
			projectName = name;
		}
		refreshApplyState();
		updateConvertActions();
	});

	Defaults::attachFilenameFilter(inputNewProjectName);
	inputNewProjectName->signal_changed().connect([this]() {
		projectName = inputNewProjectName->get_text().raw();
		refreshApplyState();
	});

	// Dialog show
	signal_show().connect([this]() {
		const string& pd {Settings::get().getProjectsDir()};
		comboSelectProject->set_active_id(projectName);
		inputNewProjectName->set_text("");
		if (not pd.empty()) fileProjectsDirSelect->set_filename(pd);
		/*
		 * Unsaved project changes freeze the projects directory:
		 * changing it would rebase the open project onto a different location.
		 * Dirtiness cannot change while this modal dialog is open,
		 * so evaluating here is enough.
		 */
		const bool pathsFrozen {not Settings::get().getCurrentProject().empty() and Defaults::isDirty()};
		fileProjectsDirSelect->set_sensitive(not pathsFrozen);
		fileProjectsDirSelect->get_parent()->set_tooltip_text(
			pathsFrozen ? "Save or discard the project changes to change the projects folder." : ""
		);
		labelProjectsPath->set_text(pd.empty() ? "N/A" : pd);
		boxNewProjectName->set_visible(projectName.empty());
		// The portable choice only exists when the system config is an option too.
		toggleNewProjectPortable->set_active(false);
		toggleNewProjectPortable->set_visible(Settings::get().isSystemConfigAvailable());
		btnApply->set_sensitive(false);
		if (not pd.empty()) {
			scanProjects();
			updateBoxProjectActions();
		}
	});
}

const string& DialogProject::getProjectName() const {
	return projectName;
}

void DialogProject::scanProjects() {

	comboSelectProject->remove_all();
	comboSelectProject->append("", "Create New...");
	comboSelectProject->set_active_id("");

	const string& pd {Settings::get().getProjectsDir()};
	if (pd.empty()) return;

	const auto directory {Gio::File::create_for_path(pd)};
	Glib::RefPtr<Gio::FileEnumerator> enumerator;
	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error& e) {
		Message::displayError(e.what(), this);
		return;
	}

	// Transient: collected only to sort before populating the combo, which is the
	// single source of truth for the project list.
	StringVector names;
	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		if (fileInfo->get_file_type() != Gio::FILE_TYPE_DIRECTORY) continue;
		const string name {fileInfo->get_name()};
		if (name.empty() or name[0] == '.') continue;
		// Skip sibling backup directories left by the rename-based save transaction.
		if (name.size() > BACKUP_SUFFIX.size() and
			name.compare(name.size() - BACKUP_SUFFIX.size(), BACKUP_SUFFIX.size(), BACKUP_SUFFIX) == 0)
			continue;
		names.push_back(name);
	}

	std::sort(names.begin(), names.end());

	for (const auto& name : names)
		comboSelectProject->append(name, name);
}

void DialogProject::setProjectsDir(const string& projectsDir, bool setFileProjectsDirSelector) {
	if (projectsDir.empty()) {
		Settings::get().setProjectsDir("");
		labelProjectsPath->set_text("N/A");
	}
	else {
		const string normalized {projectsDir + (projectsDir.back() != '/' ? "/" : "")};
		Settings::get().setProjectsDir(normalized);
		labelProjectsPath->set_text(normalized);
		scanProjects();
	}
	if (setFileProjectsDirSelector) fileProjectsDirSelect->set_filename(Settings::get().getProjectsDir());
	updateBoxProjectActions();
}

void DialogProject::updateBoxProjectActions() {
	refreshApplyState();
	boxProjectActions->show_all();
	updateConvertActions();
}

void DialogProject::updateConvertActions() {

	const auto& s {Settings::get()};
	const string id {comboSelectProject->get_active_id()};
	const string projectDir {s.getProjectDir(id)};

	// Conversions act on existing projects only.
	if (id.empty() or not Glib::file_test(projectDir, Glib::FILE_TEST_IS_DIR)) {
		boxProjectConvert->set_visible(false);
		return;
	}
	boxProjectConvert->set_visible(true);

	const string& systemConf {s.getConfigPath()};
	const bool
		isPortable         {Glib::file_test(projectDir + CONFIG_FILE, Glib::FILE_TEST_EXISTS)},
		systemConfExists   {not systemConf.empty() and Glib::file_test(systemConf, Glib::FILE_TEST_EXISTS)},
		projectDirWritable {Settings::isPathWritable(projectDir)},
		// The open project cannot be converted with unsaved changes.
		frozen             {id == s.getCurrentProject() and Defaults::isDirty()};

	btnProjectMakePortable->set_visible(not isPortable);
	btnProjectDeploy->set_visible(isPortable);
	btnProjectUseSystem->set_visible(isPortable);

	btnProjectMakePortable->set_sensitive(not frozen and systemConfExists and projectDirWritable);
	btnProjectDeploy->set_sensitive(not frozen and Settings::isPathWritable(systemConf));
	btnProjectUseSystem->set_sensitive(not frozen and systemConfExists and projectDirWritable);

	// Insensitive buttons show no tooltip; the reason goes on the row.
	string reason;
	if (frozen)
		reason = "Save or discard the project changes first.";
	else if (not isPortable and not systemConfExists)
		reason = "There is no system configuration to copy.";
	else if (isPortable and not systemConfExists and not Settings::isPathWritable(systemConf))
		reason = "There is no system configuration location.";
	else if (not projectDirWritable)
		reason = "The project directory is read-only.";
	boxProjectConvert->set_tooltip_text(reason);
}

void DialogProject::makePortable() {

	const string id {comboSelectProject->get_active_id()};
	if (Message::ask(
		"Copy the system configuration into \"" + id + "\"?\n"
		"The project will use its own copy from then on.",
		this
	) != Gtk::ResponseType::RESPONSE_YES) return;

	try {
		std::filesystem::copy_file(
			Settings::get().getConfigPath(),
			Settings::get().getProjectDir(id) + CONFIG_FILE
		);
		afterConversion("Project \"" + id + "\" is now portable", true);
	}
	catch (const std::filesystem::filesystem_error& e) {
		Message::displayError("Unable to make the project portable: " + string(e.what()), this);
	}
}

void DialogProject::deployToSystem() {

	const string id {comboSelectProject->get_active_id()};
	const string& systemConf {Settings::get().getConfigPath()};
	if (Message::ask(
		"Overwrite the system configuration with the one from \"" + id + "\"?\n"
		"The previous one is kept as " CONFIG_FILE + BACKUP_SUFFIX + ".",
		this
	) != Gtk::ResponseType::RESPONSE_YES) return;

	try {
		if (Glib::file_test(systemConf, Glib::FILE_TEST_EXISTS))
			std::filesystem::copy_file(systemConf, systemConf + BACKUP_SUFFIX, std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file(
			Settings::get().getProjectDir(id) + CONFIG_FILE,
			systemConf,
			std::filesystem::copy_options::overwrite_existing
		);
		// The project keeps its embedded config; nothing to reload.
		afterConversion("Deployed \"" + id + "\" to the system configuration", false);
	}
	catch (const std::filesystem::filesystem_error& e) {
		Message::displayError("Unable to deploy to the system configuration: " + string(e.what()), this);
	}
}

void DialogProject::useSystemConfig() {

	const string id {comboSelectProject->get_active_id()};
	if (Message::ask(
		"Stop using the own configuration of \"" + id + "\" and follow the system one?\n"
		"The project copy is kept as " CONFIG_FILE + BACKUP_SUFFIX + ".",
		this
	) != Gtk::ResponseType::RESPONSE_YES) return;

	const string embedded {Settings::get().getProjectDir(id) + CONFIG_FILE};
	try {
		std::filesystem::rename(embedded, embedded + BACKUP_SUFFIX);
		afterConversion("Project \"" + id + "\" now follows the system configuration", true);
	}
	catch (const std::filesystem::filesystem_error& e) {
		Message::displayError("Unable to switch to the system configuration: " + string(e.what()), this);
	}
}

void DialogProject::afterConversion(const string& message, bool sourceChanged) {
	if (sourceChanged and comboSelectProject->get_active_id() == Settings::get().getCurrentProject())
		currentProjectConverted = true;
	StatusBar::getInstance().push(message, StatusBar::Severity::Success);
	updateConvertActions();
}

void DialogProject::checkNewName(const string& name) const {
	if (Settings::get().getProjectsDir().empty())
		throw Message("Set a projects directory first.");
	// Reserved by the save transaction for backups.
	if (name.size() >= BACKUP_SUFFIX.size() and
		name.compare(name.size() - BACKUP_SUFFIX.size(), BACKUP_SUFFIX.size(), BACKUP_SUFFIX) == 0
	)
		throw Message("Names ending in \"" + BACKUP_SUFFIX + "\" are reserved.");
	if (Defaults::comboBoxHasId(comboSelectProject, name))
		throw Message("A project with that name already exists.");
}

void DialogProject::refreshApplyState() {
	// An existing project chosen in the combo is always valid.
	if (not comboSelectProject->get_active_id().empty()) {
		inputNewProjectName->unset_icon(Gtk::ENTRY_ICON_SECONDARY);
		btnApply->set_sensitive(true);
		return;
	}
	const string name {inputNewProjectName->get_text()};
	if (name.empty()) {
		inputNewProjectName->unset_icon(Gtk::ENTRY_ICON_SECONDARY);
		btnApply->set_sensitive(false);
		return;
	}
	// New project: validate the typed name, flagging it inline on failure.
	try {
		checkNewName(name);
		inputNewProjectName->unset_icon(Gtk::ENTRY_ICON_SECONDARY);
		btnApply->set_sensitive(true);
	}
	catch (Message& e) {
		inputNewProjectName->set_icon_from_icon_name("dialog-error", Gtk::ENTRY_ICON_SECONDARY);
		inputNewProjectName->set_icon_tooltip_text(e.takeMessage(), Gtk::ENTRY_ICON_SECONDARY);
		btnApply->set_sensitive(false);
	}
}
