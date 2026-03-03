/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProject.hpp
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

#include "GladeDialog.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * This Dialog will handle the project creation / selection
 */
class DialogProject : public GladeDialog<DialogProject> {

	friend class Gtk::Builder;

public:

	virtual ~DialogProject() = default;

	const string& getProjectName() const;

	void setProjectsDir(const string &projectsDir, bool setFileProjectsDirSelector);
	void setProjectName(const string &projectName);

	/**
	 * Scans projectsDir for available projects.
	 * Populates projects list with folder names.
	 * @throws if something goes wrong.
	 */
	void scanProjects();

protected:

	string projectName;

	Gtk::Label* labelProjectsPath = nullptr;

	Gtk::FileChooserButton* fileProjectsDirSelect = nullptr;

	Gtk::Entry* inputNewProjectName = nullptr;

	Gtk::Button* btnApply = nullptr;

	Gtk::Box
		* boxNewProjectName = nullptr, // Label + Entry pair.
		* boxProjectActions = nullptr; // Open / Select project actions.

	Gtk::ComboBoxText* comboSelectProject = nullptr;

	DialogProject(BaseObjectType *obj, const Glib::RefPtr<Gtk::Builder> &builder);

	/**
	 * Updates project and data labels.
	 * Activate open/select project if projects and data dirs are set.
	 */
	void updateBoxProjectActions();

};

} // Namespace
