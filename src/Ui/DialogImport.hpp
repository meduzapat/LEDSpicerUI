/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogImport.hpp
 * @since     Feb 14, 2023
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

#ifndef UI_DIALOGIMPORT_HPP_
#define UI_DIALOGIMPORT_HPP_ 1

#include "Message.hpp"

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogImport
 * Dialog to create select files to import.
 */
class DialogImport: public Gtk::FileChooserDialog {

public:

	enum class Types : uint8_t {CONFIG};

	DialogImport(const Types type, Gtk::Window* parent);

	virtual ~DialogImport() = default;

	uint8_t getConfigParameters();

protected:

	Gtk::Button* btbOk = nullptr;

	uint8_t flags = 0;

	static const unordered_map<const Types, const string> setups;

	void setFlags(uint8_t value, bool on);

	void setConfigBox(Gtk::Box* box);
};

} /* namespace */

#endif /* UI_DIALOGIMPORT_HPP_ */
