/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
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

#include "Form.hpp"
#include "../DialogElement.hpp"

#ifndef DEVICE_HPP_
#define DEVICE_HPP_ 1

#define CHANGE_POINT "changePoint"
#define DEFAULT_CHANGE_VALUE 64.00

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Form::Devices::Device
 * A class that adds functionality for Devices form.
 */
class Device : public Form {

public:

	struct DeviceInfo {
		const string  name;
		const uint8_t maxIds;
		const bool    monochrome;
		const uint8_t pins;
	};

	struct DeviceFields {
		Gtk::ComboBox*  comboBoxDevices  = nullptr;
		Gtk::ComboBox*  comboBoxId       = nullptr;
		Gtk::Scale*     changePoint      = nullptr;
		Gtk::ListStore* devicesListstore = nullptr;
		Gtk::ListStore* idListstore      = nullptr;
	};

	Device() = delete;

	Device(unordered_map<string, string>& data);

	virtual ~Device();

	/**
	 * @brief Populates the given device store with available devices.
	 * @param model Device names model.
	 * @param fields pointer to the device fields.
	 */
	static void initialize(DeviceFields* fields);

	/**
	 * @param name
	 * @return true if the device support multiple connected boards.
	 */
	static bool isMultiple(const string& name);

	/**
	 * @param name
	 * @return true if the device is multiple and monocrome.
	 */
	static bool isMonocrome(const string& name);

	/**
	 * @param name
	 * @return returns true if the device can be used.
	 */
	static bool isAvailable(const string& name);

	virtual const string createPrettyName() const;

	virtual const string createName() const;

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

protected:

	/**
	 * This combobox is shared between all the ID devices, I cannot share the model
	 */
	static DeviceFields* fields;

	/**
	 * Keeps control of duplicated devices.
	 */
	static CollectionHandler* devices;

	/// A list of device to their information.
	static const unordered_map<string, DeviceInfo> devicesInfo;

	/// Shared dialog for elements.
	static DialogElement* dialogElement;

	/// Device's pin usage, stores the pin and the class used.
	unordered_map<string, string> pinClasses;

	/// Store a copy of elements from the elements dialog.
	Forms::BoxButtonCollection elements;

	virtual void onActivate();

	virtual void onDeActivate();

	/**
	 * Populates the Combobox.
	 */
	void processIdComboBox();

	void markDevicesUsed();

};

} /* namespace */

#endif /* DEVICE_HPP_ */
