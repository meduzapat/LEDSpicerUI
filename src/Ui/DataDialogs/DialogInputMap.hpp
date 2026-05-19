/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.hpp
 * @since     Sep 30, 2023
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
#include "DialogColors.hpp"
#include "Storage/InputMap.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInputMap
 * Handles creation and editing of InputMap entries for a given InputSource.
 * The ownerData is always an InputSource — its createUniqueId() provides
 * the source prefix stored as a property on each InputMap.
 */
class DialogInputMap : public DialogForm, public SingletonDialog<DialogInputMap> {

	friend class Gtk::Builder;

public:

	virtual ~DialogInputMap() = default;

	/**
	 * Changes the box where the items will be displayed.
	 * @param flag 1 sources box, 0 input box.
	 */
	void setNormalBox(const bool flag) noexcept;

	void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) noexcept override;
	void removeOwner() noexcept override;

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::ComboBoxText
		* comboBoxInputMapElement = nullptr,
		* comboBoxInputMapGroup   = nullptr;

	Gtk::ComboBox
		* comboBoxInputMapFilter  = nullptr;

	Gtk::Entry*  inputInputMapTrigger = nullptr;
	Gtk::Button* inputMapDefaultColor = nullptr;
	Gtk::Stack*  stackElementAndGroup = nullptr;

	OrdenableFlowBox
		/// Maps box inside DialogInputSource — used by real (hardware) sources.
		* boxSourceMaps = nullptr,
		/// Maps box inside DialogInput — used by sourceless inputs.
		* boxInputMaps = nullptr;

	DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	const string& getType() const noexcept override { return TYPE_INPUT_MAP; }

	Storage::Data* createData(Values& rawData) const noexcept override;

};

} // namespace
