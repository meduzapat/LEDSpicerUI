/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileDirectoryNavigator.hpp
 * @since     May 2026
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

#include "DirectoryNavigator.hpp"
#include "DataDialogs/DialogProfile.hpp"
#include "config/ProfileFile.hpp"
#include "DialogImport.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::ProfileDirectoryNavigator
 * Specialized navigator for Profile files.
 * Handles loading/saving and manages DialogProfile.
 */
class ProfileDirectoryNavigator : public DirectoryNavigator {

public:

	ProfileDirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder, Gtk::Window* parentWindow) noexcept;

	virtual ~ProfileDirectoryNavigator() = default;

	void clear() noexcept override;

	/**
	 * @return Name of the profile currently selected as default, or empty string.
	 */
	const string& getDefaultProfileName() const noexcept { return defaultProfileName; }

	/**
	 * Sets the default profile by name. Used after load() to restore the
	 * selection persisted in the config file.
	 * @param name
	 */
	void setDefaultProfileName(const string& name) noexcept { defaultProfileName = name; }

protected:

	/// Import dialog for existing LEDSpicer profile files.
	DialogImport dialogImportProfile;

	/// Name of the currently selected default profile (config-file scope).
	string defaultProfileName;

	void setupDialog() noexcept override;

	void extractData(const string& filePath, Storage::DirectoryEntry* parent) override;

	void saveItem(Storage::Data* item, const string& filePath) const noexcept override;

	const string& getSubDir() const noexcept override { return PATH_PROFILE; }

};

} // namespace
