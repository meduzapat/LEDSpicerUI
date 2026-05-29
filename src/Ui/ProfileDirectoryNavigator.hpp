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

	void load() noexcept override;

	/**
	 * @return Full relative path of the default profile (no extension), or empty string if none.
	 */
	string getDefaultProfileName() const noexcept;

	/**
	 * Resolves the default profile by matching fullPath against loaded profiles.
	 * Called after load() to restore the selection persisted in the config file.
	 * If no match is found, the selection is left empty (save will report an error).
	 * @param fullPath Relative path without extension, as stored in ledspicer.conf.
	 */
	void setDefaultProfileName(const string& fullPath) noexcept;

protected:

	/// Import dialog for existing LEDSpicer profile files.
	DialogImport dialogImportProfile;

	/// Path from config (target for the load-time search); cleared once matched.
	string defaultProfilePath;

	/// Tile of the selected default profile; nullptr = none.
	Storage::BoxButton* defaultProfileBB = nullptr;

	void setupDialog() noexcept override;

	void extractData(const string& filePath, Storage::DirectoryEntry* parent) override;

	void saveItem(Storage::Data* item, const string& filePath) const noexcept override;

	const string& getSubDir() const noexcept override { return PATH_PROFILE; }

};

} // namespace
