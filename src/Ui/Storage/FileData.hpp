/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileData.hpp
 * @since     Feb 23, 2026
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

#include "DirNode.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::FileData
 * Data class for file-based items (Inputs, Animations, Profiles).
 * Holds a pointer to its owning DirectoryEntry so path resolution is always
 * live — directory renames propagate automatically with no cascade needed.
  */
class FileData : public DirNode {

public:

	/**
	 * @param data Item field data. FILENAME is extracted and stored as a property.
	 * @param dir  Owning directory. nullptr means root of the type's tree.
	 */
	FileData(StringUMap& data, const DirNode* dir);

	const string createUniqueId() const override;
	const string createPrettyName() const override;
	const string createTooltip() const override;

	/**
	 * Returns a stable app-wide identifier that never changes even if renamed.
	 * @return Stable code string, e.g. "dir_1".
	 */
	string getFsId() const override;

	string getName() const override;

protected:

	/// Stable app-wide file identifier. Never changes, never serialized.
	const string fsId;

	/// Counter for stable code generation.
	inline static size_t fileCounter = 0;
};

} // namespace
