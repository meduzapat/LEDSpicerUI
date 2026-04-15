/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RestrictorMap.cpp
 * @since     Oct 14, 2023
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

#include "RestrictorMap.hpp"

using namespace LEDSpicerUI::Ui::Storage;

string RestrictorMap::createPrettyName() const noexcept {
	string name("Player " + getValue(PLAYER) + " Joystick " + getValue(JOYSTICK));
	// Split hardware name out of additional info and check for multi.
	if (not getValue(RESTRICTOR_INTERFACE).empty())
		name += " [ interface " + getValue(RESTRICTOR_INTERFACE) + "]";
	return name;
}

string RestrictorMap::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({getValue(PLAYER), getValue(JOYSTICK)});
}
