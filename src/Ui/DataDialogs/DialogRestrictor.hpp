/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.hpp
 * @since     Apr 30, 2023
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
#include "Storage/Restrictor.hpp"
#include "DialogRestrictorMap.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogRestrictor
 */
class DialogRestrictor : public DialogFormHost, public SingletonDialog<DialogRestrictor> {

	friend class Gtk::Builder;

public:

	static constexpr string_view noRestrictor {"Select Restrictor"};

	virtual ~DialogRestrictor() = default;

	void load(XMLHelper* values) noexcept override;
	void isValid() const override;
	void resetForm()    noexcept override;
	void storeData()    noexcept override;
	void retrieveData() noexcept override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::ComboBox* comboBoxId = nullptr;

	Gtk::ToggleButton
		* williamsMode  = nullptr,
		* hasRestrictor = nullptr,
		* handleMouse   = nullptr;
	Gtk::SpinButton
		* speedOn  = nullptr,
		* speedOff = nullptr;
	Gtk::Entry* serialPort = nullptr;

	/// Restrictor ways icon map keyed by Ways enum value.
	std::unordered_map<Defaults::Ways, Gtk::FlowBoxChild*> waysIcons;

	Gtk::ListStore* idListstore      = nullptr;
	Gtk::Label*     brief            = nullptr;
	Gtk::Button*    btnAddRestrictorMap = nullptr;

	DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(XMLHelper* values) noexcept override;
	string_view getType() const noexcept override { return TYPE_RESTRICTOR; }
	Storage::Data* createData(StringUMap& rawData) const noexcept override;
	void onEmpty() noexcept override;
	void onSelected() noexcept override;
};

} // namespace
