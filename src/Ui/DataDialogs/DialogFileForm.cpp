/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogFileForm.cpp
 * @since     Feb 25, 2026
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

#include "DialogFileForm.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;


void DialogFileForm::setCurrentDirectory(Storage::DirectoryEntry* directory) {
	currentDirectory = directory;
}

LEDSpicerUI::Ui::Storage::DirectoryEntry* DialogFileForm::getCurrentDirectory() const {
	return currentDirectory;
}

string DialogFileForm::getFullPath(const string& filename) const {
	if (not currentDirectory) return filename;
	return currentDirectory->createUniqueId() + "/" + filename;
}

bool DialogFileForm::isUniqueFilename(const string& filename) const {
	if (not items) return true;
	for (const auto* bb : *items) {
		auto* fd = dynamic_cast<Storage::FileData*>(bb->getData());
		if (not fd or fd == currentData) continue;
		if (fd->getDirectory() == currentDirectory and fd->getProperty(FILENAME) == filename)
			return false;
	}
	return true;
}
