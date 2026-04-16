/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.hpp
 * @since     Feb 14, 2023
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

#include "Storage/Input.hpp"
#include "DialogFileForm.hpp"
#include "DialogInputSource.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInput
 * Dialog to create and edit Inputs.
 */
class DialogInput : public DialogFileForm, public SingletonDialog<DialogInput> {

	friend class Gtk::Builder;

public:

	static constexpr const char* BOX_INPUTS = "BoxInputs";
	static constexpr string_view noInput{"Select Input Type"};

	virtual ~DialogInput() = default;

	void load(XMLHelper* values) noexcept override;
	void isValid() const override;
	void resetForm()   noexcept override;
	void storeData()   noexcept override;
	void retrieveData() noexcept override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::ComboBox* comboBoxInputSpeed = nullptr;

	Gtk::Entry*      entryInputName   = nullptr;
	Gtk::Switch*     switchInputBlink = nullptr;
	Gtk::SpinButton* spinInputTimes   = nullptr;
	Gtk::Box
		* boxInputSourcesBox         = nullptr,
		* boxInputMapsBox            = nullptr,
		* boxInputCreditsSettings    = nullptr,
		* boxLinkedElementsAndGroups = nullptr;

	Gtk::Button
		* btnAddInputSource = nullptr, // Only to hide/show when source is selected.
		* btnAddInputMap    = nullptr, // Only to hide/show when source is selected.
		* btnAddInput       = nullptr; // Form shooter.

	Gtk::Label* brief = nullptr;

	// The box to display linked maps.
//	OrdenableFlowBox* boxDirectMaps = nullptr;

	DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(XMLHelper* values) noexcept override;
	string_view getType() const noexcept override { return TYPE_INPUT; }
	Storage::Data* createData(StringUMap& rawData) const noexcept override;

	void onEmpty() noexcept override;

	void onSelected() noexcept override;
};

} // namespace
