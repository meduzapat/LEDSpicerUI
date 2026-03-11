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

FileData::FileData(StringUMap& data, const DirNode* dir) :
	DirNode(data, dir),
	fsId("file_" + std::to_string(++fileCounter))
{
	setProperty(FILENAME, data.count(FILENAME) ? data.at(FILENAME) : "");
	setProperty(UID, fsId);
	data.erase(FILENAME);
}

const string FileData::createUniqueId() const {
	const string parentId = parent ? parent->getFsId() : "";
	return Defaults::createCommonUniqueId({parentId, getName()});
}

const string FileData::createPrettyName() const {
	return getFullPath() + "[" + getValue(NAME) + "]";
}

const string FileData::createTooltip() const {
	return "of type " + getValue(NAME);
}

string FileData::getFsId() const {
	return fsId;
}

string FileData::getName() const {
	return getProperty(FILENAME);
}
