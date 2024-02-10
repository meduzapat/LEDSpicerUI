/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PinHandler.hpp
 * @since     Mar 22, 2023
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

#include "Message.hpp"
#include <map>

#ifndef PINHANDLER_HPP_
#define PINHANDLER_HPP_ 1

const int MAX_COLUMNS = 20;
const int MIN_COLUMNS = 10;

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::PinHandler
 * Takes cares of storing used pins and to display a box with them.
 */
class PinHandler: public Gtk::FlowBox {

	friend class Gtk::Builder;

public:

	PinHandler() = delete;

	virtual ~PinHandler() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Returns an instance of PinHandler or null if not initialized.
	 * @return self
	 */
	static PinHandler* getInstance();

	void setCurrentDevicePins(std::map<uint8_t, string>* pinClasses);

	/**
	 * Reset the view to the internal values (refresh).
	 */
	void reset();

	void wipe();

	/**
	 * Resize the classes map and set all to no color and do a reset.
	 * @param newSize
	 * @param doWipe if true (default) will remove all pins.
	 */
	void resize(uint8_t newSize, bool doWipe = true);

	/**
	 * Return the number of pins.
	 * @return
	 */
	size_t getSize();

	/**
	 * Checks if a pin is valid.
	 * @param pin
	 * @return
	 */
	bool exists(const string& pin);

	/**
	 * Check if a pin is used.
	 * @param pin
	 * @return
	 */
	bool isUsed(const string& pin);

	/**
	 * Unset a pin.
	 * @param pin
	 */
	void unmarkPin(const string& pin);

	/**
	 * Sets a pin.
	 * @param pin
	 * @param color
	 */
	void markPin(const string& pin, const string& color);

protected:

	PinHandler(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) : Gtk::FlowBox(obj) {}

	std::map<uint8_t, string>* pinClasses = nullptr;

	static PinHandler* instance;

	const uint8_t findDivisor(uint8_t size);

};

} /* namespace */

#endif /* PINHANDLER_HPP_ */
