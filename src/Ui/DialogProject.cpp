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
	builder->get_widget("BtnApplyProject",       btnApply);

	fileProjectsDirSelect->signal_file_set().connect([this]() {
		setProjectsDir(fileProjectsDirSelect->get_filename(), false);
	});

	// Select project button
	comboSelectProject->signal_changed().connect([this]() {
		string name(comboSelectProject->get_active_id());
		if (name.empty()) {
			boxNewProjectName->set_visible(true);
			inputNewProjectName->set_text("");
			inputNewProjectName->grab_focus();
			btnApply->set_sensitive(false);
			return;
		}
		boxNewProjectName->set_visible(false);
		setProjectName(name);
	});

	inputNewProjectName->signal_changed().connect([this]() {
		string text = inputNewProjectName->get_text();
		string filtered = Defaults::sanitizeFilename(text);

		// Remove leading dots
		while (not filtered.empty() and filtered[0] == '.') {
			filtered.erase(0, 1);
		}

		// Update if changed (will re-trigger, but same text = no loop)
		if (filtered != text) {
			inputNewProjectName->set_text(filtered);
			inputNewProjectName->set_position(-1);
			return;
		}

		setProjectName(filtered);
	});

	// Dialog show
	signal_show().connect([this]() {
		comboSelectProject->set_active_id(projectName);
		inputNewProjectName->set_text("");
		fileProjectsDirSelect->set_filename(Defaults::getProjectsDir());
		labelProjectsPath->set_text(Defaults::getProjectsDir());
		boxNewProjectName->set_visible(projectName.empty());
		btnApply->set_sensitive(false);
		if (not Defaults::getProjectsDir().empty()) {
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

	if (Defaults::getProjectsDir().empty()) return;

	auto directory = Gio::File::create_for_path(Defaults::getProjectsDir());
	Glib::RefPtr<Gio::FileEnumerator> enumerator;
	try {
		enumerator = directory->enumerate_children();
	}
	catch (const Glib::Error& e) {
		Message::displayError(e.what());
	}

	StringVector projects;
	Glib::RefPtr<Gio::FileInfo> fileInfo;
	while ((fileInfo = enumerator->next_file())) {
		if (fileInfo->get_file_type() != Gio::FILE_TYPE_DIRECTORY) continue;
		string name(fileInfo->get_name());
		if (name.empty() or name[0] == '.') continue;
		projects.push_back(name);
	}

	std::sort(projects.begin(), projects.end());

	for (const auto& project : projects) {
		comboSelectProject->append(project, project);
	}
}

void DialogProject::setProjectsDir(const string& projectsDir, bool setFileProjectsDirSelector) {
	if (projectsDir.empty()) {
		Defaults::setProjectsDir("");
		labelProjectsPath->set_text("N/A");
	}
	else {
		Defaults::setProjectsDir(projectsDir + (projectsDir.back() != '/' ? "/" : ""));
		labelProjectsPath->set_text(Defaults::getProjectsDir());
		scanProjects();
	}
	if (setFileProjectsDirSelector) fileProjectsDirSelect->set_filename(Defaults::getProjectsDir());
	updateBoxProjectActions();
}

void DialogProject::setProjectName(const string& projectName) {
	// TODO: sanitize
	this->projectName = projectName;
	btnApply->set_sensitive(not this->projectName.empty());
}

void DialogProject::updateBoxProjectActions() {
	btnApply->set_sensitive(not projectName.empty());
	boxProjectActions->show_all();
}
