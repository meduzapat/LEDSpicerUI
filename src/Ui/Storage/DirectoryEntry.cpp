/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryEntry.cpp
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

#include "DirectoryEntry.hpp"

using namespace LEDSpicerUI::Ui::Storage;


string DirectoryEntry::getName() const {
	return getValue(NAME);
}

const string DirectoryEntry::createUniqueId() const {
	return Defaults::createCommonUniqueId({
		isRoot() ? static_cast<const DirectoryEntry*>(parent)->getFsId() : "",
		getValue(NAME)
	});
}

const string DirectoryEntry::createPrettyName() const {
	return "📁 " + getValue(NAME);
}

const string DirectoryEntry::createTooltip() const {
	return getFullPath();
}

const string DirectoryEntry::getCssClass() const {
	return "DirectoryBoxButton";
}

bool DirectoryEntry::isEmpty() const {
	return not contents.getSize();
}

const string& DirectoryEntry::getFsId() const {
	return fsId;
}
