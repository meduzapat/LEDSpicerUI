/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.hpp
 * @since     Feb 26, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogDevice
 *
 * Data Owner: MainDialog
 *
 */
class DialogDevice: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogDevice() = delete;

	virtual ~DialogDevice() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);
	static DialogDevice* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
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
	Gtk::Label*      brief           = nullptr;

	/// Stores the previous selected device, to be used when the user cancels a change action.
	string previousName;

	DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;

	/**
	 * Destroys and creates the data again.
	 */
	void recreateData();

	/**
	 * Marks a device disabled if it depleted all its IDs.
	 */
	void markDevicesUsed();
};

} /* namespace */

#endif /* UI_DIALOGDEVICE_HPP_ */
