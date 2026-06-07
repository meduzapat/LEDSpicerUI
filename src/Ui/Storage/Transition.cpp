/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Transition.cpp
 * @since     Jun 2026
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

#include "Transition.hpp"

using namespace LEDSpicerUI::Ui::Storage;

string Transition::createPrettyName() const noexcept {
	const string& name {getValue(NAME)};
	if (name.empty()) return "None";
	auto it {Defaults::transitionsInfo.find(name)};
	return it != Defaults::transitionsInfo.end() ? it->second.name : name;
}

string Transition::toXML() const noexcept {
	if (isNone()) return emptyString;
	return Data::toXML();
}
