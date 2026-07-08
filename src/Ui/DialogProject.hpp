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
#include "config/Settings.hpp"

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

	/**
	 * Whether a new project should embed its config despite an available system one.
	 * @return true when portable was requested.
	 */
	bool isPortableRequested() const noexcept {
		return toggleNewProjectPortable->get_active();
	}

	/**
	 * keeps a single use flag.
	 * @return true once per such conversion.
	 */
	bool takeCurrentProjectConverted() noexcept {
		const bool value {currentProjectConverted};
		currentProjectConverted = false;
		return value;
	}

	void setProjectsDir(const string& projectsDir, bool setFileProjectsDirSelector);

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

	Gtk::ToggleButton* toggleNewProjectPortable = nullptr;

	Gtk::Button* btnApply = nullptr;

	Gtk::Box
		* boxNewProjectName = nullptr, // Label + Entry pair.
		* boxProjectActions = nullptr, // Open / Select project actions.
		* boxProjectConvert = nullptr; // Config location conversions for the selected project.

	Gtk::Button
		* btnProjectMakePortable = nullptr,
		* btnProjectDeploy       = nullptr,
		* btnProjectUseSystem    = nullptr;

	/// True when a conversion changed the open project's config location; see takeCurrentProjectConverted().
	bool currentProjectConverted = false;

	Gtk::ComboBoxText* comboSelectProject = nullptr;

	DialogProject(BaseObjectType *obj, const Glib::RefPtr<Gtk::Builder> &builder);

	/**
	 * Updates project and data labels.
	 * Activate open/select project if projects and data dirs are set.
	 */
	void updateBoxProjectActions();

	/**
	 * Validates a candidate new-project name.
	 * @param name candidate name
	 * @throws Message if the projects directory is unset, the name is reserved, or it exists.
	 */
	void checkNewName(const string& name) const;

	/**
	 * Sets Apply sensitivity and the name-entry error icon for the current selection.
	 */
	void refreshApplyState();

	/**
	 * Shows the conversion actions matching the selected project's on-disk
	 * config location, disabled with the reason when not executable.
	 */
	void updateConvertActions();

	/**
	 * Copies the system config into the selected project's directory.
	 */
	void makePortable();

	/**
	 * Copies the selected project's embedded config over the system one,
	 * keeping the previous system config as a backup.
	 */
	void deployToSystem();

	/**
	 * Renames the selected project's embedded config to a backup so the
	 * project follows the system config again.
	 */
	void useSystemConfig();

	/**
	 * Reports a finished conversion and refreshes the actions.
	 * @param message status bar text.
	 * @param sourceChanged true when the project's config location moved,
	 *        requiring a reload if it is the open project.
	 */
	void afterConversion(const string& message, bool sourceChanged);

};

} // Namespace
