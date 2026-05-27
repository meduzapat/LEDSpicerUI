/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationFile.hpp
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
#include "Storage/Animation.hpp"

#pragma once

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::AnimationFile
 * Single-use parser for one animation XML file.
 * Walks every <actor> child of the LEDSpicer root and stores its raw
 * attributes into extractedData so DialogAnimation::load() can consume them.
 */
class AnimationFile : public ProjectFile {

public:

	/**
	 * @param filePath Full path to the .xml file on disk.
	 * @param parent   Owning directory node, or nullptr for root level.
	 * @throws Message on parse errors.
	 */
	AnimationFile(const string& filePath, Ui::Storage::DirectoryEntry* parent);

	virtual ~AnimationFile() = default;

	/**
	 * Writes the animation data back to an XML file at filePath.
	 * @param animation
	 * @param filePath
	 */
	static void save(const Ui::Storage::Animation& animation, const string& filePath);

};

} // namespace
