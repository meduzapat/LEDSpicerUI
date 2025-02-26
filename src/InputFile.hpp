/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFile.hpp
 * @since     Nov 14, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

#ifndef INPUTFILE_HPP_
#define INPUTFILE_HPP_ 1

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::InputFile
 */
class InputFile: public XMLHelper {

public:

	InputFile() = delete;

	InputFile(const string& inputFile, const string& projectRoot);

	virtual ~InputFile() = default;

protected:

const string processMaps(tinyxml2::XMLElement* mapsNode, const string& inputName);

const string processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode, unordered_set<string>& listenEvents);

};

} /* namespace LEDSpicerUI */

#endif /* INPUTFILE_HPP_ */
