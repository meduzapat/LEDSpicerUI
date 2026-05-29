/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileFile.hpp
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

#include "ProjectFile.hpp"
#include "Storage/Profile.hpp"

#pragma once

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::ProfileFile
 * Single-use parser for one profile XML file.
 * Extracts the profile root attributes and the four child sections
 * (alwaysOnElements, alwaysOnGroups, animations, inputs) into extractedData
 * so DialogProfile::load() can consume them.
 */
class ProfileFile : public ProjectFile {

public:

	/**
	 * @param filePath Full path to the .xml file on disk.
	 * @param parent   Owning directory node, or nullptr for root level.
	 * @throws Message on parse errors.
	 */
	ProfileFile(const string& filePath, Ui::Storage::DirectoryEntry* parent);

	virtual ~ProfileFile() = default;

	/**
	 * Writes the profile data back to an XML file at filePath.
	 * @param profile
	 * @param filePath
	 */
	static void save(const Ui::Storage::Profile& profile, const string& filePath);

};

} // namespace
