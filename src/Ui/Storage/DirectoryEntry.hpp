/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryEntry.hpp
 * @since     Feb 15, 2026
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

#include "Data.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::DirectoryEntry
 * Virtual directory representation for navigation.
 * Not persisted, created dynamically from item paths.
 * NAME stores directory name, PATH stores parent directory.
 */
class DirectoryEntry : public Data {

public:

	using Data::Data;

	virtual ~DirectoryEntry() = default;

	/**
	 * Creates display name with folder icon.
	 * @return Formatted name.
	 */
	const string createPrettyName() const override;

	/**
	 * Creates tooltip text.
	 * @return Tooltip string.
	 */
	const string createTooltip() const override;

	/**
	 * Returns CSS class for styling.
	 * @return CSS class name.
	 */
	const string getCssClass() const override;

	/**
	 * Creates unique identifier from parent path and name.
	 * @return Full path string.
	 */
	const string createUniqueId() const override;

	/**
	 * Returns parent directory path.
	 * @return Parent path string.
	 */
	const string getParentPath() const;

};

} // namespace
