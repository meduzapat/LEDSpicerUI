/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDevice.hpp
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

#include "DialogFormHost.hpp"
#include "Storage/Device.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogDevice
 *
 * Data Owner: MainDialog
 */
class DialogDevice : public DialogFormHost, public SingletonDialog<DialogDevice> {

	friend class Gtk::Builder;

public:

	virtual ~DialogDevice();

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

protected:

	Gtk::ComboBox
		* comboBoxDevices = nullptr,
		* comboBoxId      = nullptr;
	Gtk::ListStore
		* devicesListstore = nullptr,
		* idListstore      = nullptr;
	Gtk::Scale*      changePoint     = nullptr;
	Gtk::SpinButton* spinnerLeds     = nullptr;
	Gtk::Entry*      inputDevicePort = nullptr;
	Gtk::Label*      brief           = nullptr;

	DialogDevice(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	string_view getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;

	/**
	 * Marks a device row disabled once all its hardware IDs are in use.
	 */
	void markDevicesUsed();
};

} // namespace
