/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
 * @since     Feb 26, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "DataDialogs/DialogElement.hpp"
#include "Data.hpp"
#include "PinHandler.hpp"

#ifndef DEVICE_HPP_
#define DEVICE_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Device
 * A class that adds functionality for Devices form.
 * A device is driven by device name combobox.
 * Then it needs either a hardware ID, a port number or noghing else.
 * A device will provide a number of pins where the LEDS are connected, but this are handled by elements.
 * A device will store the elements attached to the pins.
 */
class Device : public Data {

public:

	using Data::Data;

	virtual ~Device();

	const string createPrettyName() const override;

	const string createUniqueId() const override;

	const string getCssClass() const override;

	void destroy() override;

	void activate() override;

	void deActivate() override;

	const string toXML() const override;

protected:

	/// Store a copy of elements from the elements dialog.
	BoxButtonCollection elements;

	/// Device's pin usage, stores the pin and the class used, handled by elements.
	std::map<uint8_t, string> pinClasses;

};

} /* namespace */

#endif /* DEVICE_HPP_ */
