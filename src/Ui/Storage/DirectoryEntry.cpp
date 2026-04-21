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

DirectoryEntry::DirectoryEntry(
	StringUMap&     data,
	DirectoryEntry* parent
) noexcept :
	Data(data),
	DirNode(getProperties(), parent, getValue(FILENAME))
{}

string DirectoryEntry::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		not isAtRoot() ? parent->getFsId() : emptyString,
		getName()
	});
}

string DirectoryEntry::createPrettyName() const noexcept {
	return "📁 " + getName();
}

string DirectoryEntry::createTooltip() const noexcept {
	return getFullPath();
}

bool DirectoryEntry::isEmpty() const noexcept {
	return not contents.getSize();
}
