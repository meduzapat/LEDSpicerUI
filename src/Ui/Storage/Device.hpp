/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
 * @since     Feb 26, 2023
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

#include "Revertible.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Device
 *
 * A class that adds storage to Devices form.
 * A device will provide a number of pins where the LEDS are connected, but this are handled by elements.
 * A device will store the elements attached to the pins.
 */
class Device : public Revertible {

public:

	Device(StringUMap& data) :
		Revertible(data, COLLECTION_DEVICES, {{COLLECTION_ELEMENT, BoxButtonCollection()}}) {}

	virtual ~Device() = default;

	const string createPrettyName() const noexcept override;
	const string createUniqueId() const noexcept override;
	string_view getCssClass() const noexcept override { return "DeviceBoxButton"; }
	const string toXML() const override;

};

} // namespace
