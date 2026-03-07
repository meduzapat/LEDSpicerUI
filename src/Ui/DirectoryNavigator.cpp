/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryNavigator.cpp
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

#include "DirectoryNavigator.hpp"

using namespace LEDSpicerUI::Ui;

void DirectoryNavigator::onActivate() {
	wireDialogs(currentDir);
}

void DirectoryNavigator::enterDirectory(Storage::DirectoryEntry* dir) {
	currentDir = dir;
	wireDialogs(currentDir);
}

void DirectoryNavigator::navigateUp() {
	if (currentDir == &rootDir) return;
	Storage::DirNode* parent = currentDir->getParent();
	currentDir = parent ? static_cast<Storage::DirectoryEntry*>(parent) : &rootDir;
	wireDialogs(currentDir);
}

bool DirectoryNavigator::isAtRoot() const {
	return currentDir == &rootDir;
}

Storage::DirectoryEntry* DirectoryNavigator::getCurrentDir() const {
	return currentDir;
}
