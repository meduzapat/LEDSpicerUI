/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.hpp
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

#include "DialogForm.hpp"
#include "Storage/Device.hpp"

#ifndef UI_DIALOGDEVICE_HPP_
#define UI_DIALOGDEVICE_HPP_ 1

#define devicesHandler Storage::CollectionHandler::getInstance(COLLECTION_DEVICES)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogDevice
 */
class DialogDevice: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogDevice() = delete;

	virtual ~DialogDevice() = default;

	/**
	 * Instanciate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogDevice* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogDevice* instance;

	Gtk::ComboBox
		* comboBoxDevices  = nullptr,
		* comboBoxId       = nullptr;
	Gtk::ListStore
		* devicesListstore = nullptr,
		* idListstore      = nullptr;
	Gtk::Scale*      changePoint     = nullptr;
	Gtk::SpinButton* spinnerLeds     = nullptr;
	Gtk::Entry*      inputDevicePort = nullptr;
	Gtk::Label*      breaf           = nullptr;

	/// Stores the previous selected device, to be used when the user cancels a change action.
	string previousName;

	DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	/**
	 * Reset all fields except devices id.
	 */
	void clearFormOthers();

	/**
	 * Marks a device disabled if it depleted all its IDs.
	 */
	void markDevicesUsed();
};

} /* namespace */

#endif /* UI_DIALOGDEVICE_HPP_ */
