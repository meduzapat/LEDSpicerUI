/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Hardware.hpp
 * @since     May 13, 2026
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

#include "Parent.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Hardware
 *
 * Provides hardware common code.
 */
class Hardware : public Parent {

public:

	using Parent::Parent;

	string createPrettyName() const noexcept {
		string
			name{getValue(NAME)},
			r{Defaults::devicesInfo.at(name).name};
		if (Defaults::isIdUser(name))
			r += " Id: " + getValue(ID);
		if (Defaults::isSerial(name))
			r += " Port: " + (getValue(PORT).empty() ? "<autodetect>" : getValue(PORT));
		return r;
	}
};

}
