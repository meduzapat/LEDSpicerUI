/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictorMap.hpp
 * @since     Oct 14, 2023
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

#include "DialogForm.hpp"
#include "Storage/RestrictorMap.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogRestrictorMap
 */
class DialogRestrictorMap: public DialogForm, public SingletonDialog<DialogRestrictorMap> {

	friend class Gtk::Builder;

public:

	virtual ~DialogRestrictorMap() = default;

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void clearForm() noexcept override;
	void isValid() const override;
	void storeData() noexcept override;
	void retrieveData() noexcept override;
	const string createUniqueId() const noexcept override;

	/**
	 * This function will populate the interfaces combobox with values.
	 * @param currentData the form with data.
	 */
	void populateInterfacesCombobox();

protected:

	Gtk::Button* btnAdd = nullptr;

	Gtk::ComboBoxText
		* player    = nullptr,
		* joystick  = nullptr;
	Gtk::ComboBox
		* interface           = nullptr,
		* comboBoxRestrictors = nullptr;

	Gtk::ListStore * liststoreRestrictorMapId = nullptr;

	DialogRestrictorMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	/**
	 * Checks if the owner restrictor still have available interfaces spaces.
	 * @return true if there still room.
	 */
	bool checkAvailableInterfaces() const;

	string_view getType() const noexcept override;

	Storage::Data* createData(StringUMap& rawData) noexcept override;

	/**
	 * After add check if more can be added.
	 * @param boxButton
	 */
	void afterCreate(Storage::BoxButton& boxButton) override;

	/**
	 * Refresh some fields affected by delete.
	 * @param boxButton
	 */
	virtual void afterDeleteConfirmation(Storage::BoxButton& boxButton);
};

} // namespace
