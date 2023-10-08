/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PinHandler.hpp
 * @since     Mar 22, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#ifndef PINHANDLER_HPP_
#define PINHANDLER_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::PinHandler
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

	void setCurrentDevicePins(unordered_map<string, string>* pinClasses);

	void reset();

	size_t getSize();

	bool exists(const std::string& pin);

	bool isUsed(const std::string& pin);

	void unmarkPin(const std::string& pin);

	void markPin(const std::string& pin, const std::string& color);

protected:

	PinHandler(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder) : Gtk::FlowBox(obj) {}

	unordered_map<string, string>* pinClasses = nullptr;

	static PinHandler* instance;
};

} /* namespace */

#endif /* PINHANDLER_HPP_ */
