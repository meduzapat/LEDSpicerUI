/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirNode.cpp
 * @since     Feb 28, 2026
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

#include "DirNode.hpp"

using namespace LEDSpicerUI::Ui::Storage;

DirNode::DirNode(
	Values&       dest,
	DirNode*      parent,
	const string& filename
) noexcept :
	parent(parent),
	dest(dest)
{
	dest.setValue(UID,      std::to_string(++nodeCounter));
	dest.setValue(PID,      parent ? parent->getFsId() : emptyString);
	dest.setValue(FILENAME, filename);
}

const string& DirNode::getName() const noexcept {
	return dest.getValue(FILENAME);
}

const string& DirNode::getFsId() const noexcept {
	return dest.getValue(UID);
}

string DirNode::getPath() const noexcept {
	return parent ? parent->getFullPath() : emptyString;
}

string DirNode::getFullPath() const noexcept {
	string path(getPath());
	return path.empty() ? getName() : path + "/" + getName();
}
