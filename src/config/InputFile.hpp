/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFile.hpp
 * @since     Nov 14, 2023
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

#pragma once

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::InputFile
 * Single-use parser for one input XML file.
 * Extracts the input attributes and all maps/sources into extractedData
 * so DialogInput::load() can consume them.
 */
class InputFile : public ProjectFile {

public:

	/**
	 * @param filePath Full path to the .xml file on disk.
	 * @param parent Owning directory node, or nullptr for root level.
	 * @throws Message on parse errors.
	 */
	InputFile(const string& filePath, const Ui::Storage::DirectoryEntry* parent);

	virtual ~InputFile() = default;

private:

	/**
	 * Extracts maps from a single <maps> node into extractedData.
	 * @param mapsNode The <maps> XML element.
	 * @param inputName Scoped collection key for this source's maps.
	 * @return Error string, empty if clean.
	 */
	const string processMaps(tinyxml2::XMLElement* mapsNode, const string& inputName);

};

} // namespace
