/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.hpp
 * @since     Feb 26, 2023
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

#include "DialogForm.hpp"
// Icludes Device
#include "Forms/Device.hpp"

#ifndef UI_DIALOGDEVICE_HPP_
#define UI_DIALOGDEVICE_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogDevice
 */
class DialogDevice: public DialogForm {

public:

	DialogDevice() = delete;

	DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	virtual ~DialogDevice() = default;

protected:

	Forms::Device::DeviceFields fields;

	virtual string getType();

	virtual Forms::Form* getForm(unordered_map<string, string>& rawData);
};

} /* namespace */

#endif /* UI_DIALOGDEVICE_HPP_ */
