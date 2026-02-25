/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileData.cpp
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

#include "FileData.hpp"

using namespace LEDSpicerUI::Ui::Storage;

FileData::FileData(StringUMap& data) : Data(data) {
	static size_t fileCounter = 0;
	setProperty(FILE_ID, std::to_string(++fileCounter));
	// PATH and FILENAME come pre-split from file loader or Dialog.
	setProperty(PATH,     data.count(PATH)     ? data.at(PATH)     : "");
	setProperty(FILENAME, data.count(FILENAME) ? data.at(FILENAME) : "");
	// Store as properties so they never serialize into the XML attributes.
	data.erase(PATH);
	data.erase(FILENAME);
}

const string FileData::createUniqueId() const {
	string
		path(getProperty(PATH)),
		filename(getProperty(FILENAME));
	if (filename.empty()) return "";
	return path.empty() ? filename : path + "/" + filename;
}

const string FileData::createPrettyName() const {
	return getProperty(FILENAME) + "[" + getValue(NAME) + "]";
}

const string FileData::createTooltip() const {
	return "of type " + getValue(NAME);
}
