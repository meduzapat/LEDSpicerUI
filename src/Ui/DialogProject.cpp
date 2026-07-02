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
	builder->get_widget("LabelProjectsPath",     labelProjectsPath);
	builder->get_widget("FileProjectsDirSelect", fileProjectsDirSelect);
	builder->get_widget("ComboSelectProject",    comboSelectProject);
	builder->get_widget("InputNewProjectName",   inputNewProjectName);
	builder->get_widget("BoxProjectActions",     boxProjectActions);
	builder->get_widget("BoxNewProjectName",     boxNewProjectName);

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
			projectName = name;
		}
		refreshApplyState();
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
		labelProjectsPath->set_text(pd.empty() ? "N/A" : pd);
		boxNewProjectName->set_visible(projectName.empty());
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
