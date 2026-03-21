/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProjectFile.cpp
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

#include "ProjectFile.hpp"

using namespace LEDSpicerUI::Config;

namespace {
	string extractBasename(const string& filePath) {
		string base(Glib::path_get_basename(filePath));
		const auto dot(base.find_last_of('.'));
		if (dot != string::npos)
			base = base.substr(0, dot);
		return base;
	}
}

ProjectFile::ProjectFile(
	const string& filePath,
	const string& fileType,
	const LEDSpicerUI::Ui::Storage::DirectoryEntry* parent
) :
	XMLHelper(filePath, fileType),
	filename(extractBasename(filePath)),
	parent(parent)
{}

const string& ProjectFile::getFilename() const {
	return filename;
}

const LEDSpicerUI::Ui::Storage::DirectoryEntry* ProjectFile::getParent() const {
	return parent;
}
