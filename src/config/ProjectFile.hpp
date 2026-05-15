/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProjectFile.hpp
 * @since     Feb 8, 2026
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

#include "XMLHelper.hpp"
#include "Settings.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI {

namespace Constants {

inline const string
	/// Commonly used empty string.
	PATH_INPUT     {"inputs/"},
	PATH_ANIMATION {"animations/"},
	PATH_PROFILE   {"profiles/"};
}

namespace Config {

	using Ui::Storage::DirectoryEntry;

/**
 * LEDSpicerUI::Config::ProjectFile
 * Base class for project-based XML files (inputs, animations, profiles).
 * Stores the bare filename and a pointer to the owning directory node so
 * callers can build scoped collection keys without storing path strings.
 */
class ProjectFile : public XMLHelper {

public:

	ProjectFile() = delete;

	/**
	 * Writes content to path, or shows a debug dialog instead if debugFiles is enabled.
	 */
	static void saveFile(const string& path, const string& content);

	virtual ~ProjectFile() = default;

	/// @return Bare filename without extension.
	const string& getFilename() const noexcept { return filename; }

	/// @return Owning directory node, or nullptr if at root level.
	DirectoryEntry* getParent() const noexcept { return parent; }

protected:

	/// Bare filename without extension.
	string filename;

	/// Owning directory node. nullptr = root of the type's tree.
	DirectoryEntry* parent;

	/**
	 * @param filePath Full path to the file on disk.
	 * @param fileType Expected type attribute for XML validation.
	 * @param parent   Owning directory node, or nullptr for root level.
	 * @throws Message on parse errors.
	 */
	ProjectFile(
		const string& filePath,
		const string& fileType,
		DirectoryEntry* parent
	) :
		XMLHelper(filePath, fileType),
		filename(Defaults::extractName(filePath, Glib::path_get_dirname(filePath))),
		parent(parent)
	{}
};

}} // namespace
