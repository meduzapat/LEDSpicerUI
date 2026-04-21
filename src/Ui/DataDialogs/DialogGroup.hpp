/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.hpp
 * @since     Feb 13, 2023
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
#include "DialogSelect.hpp"
#include "Storage/Group.hpp"
#include "Storage/Element.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogGroup
 * Dialog to create or edit groups.
 */
class DialogGroup : public DialogForm, public SingletonDialog<DialogGroup> {

	friend class Gtk::Builder;

public:

	virtual ~DialogGroup() = default;

	void load(XMLHelper* values) noexcept override;
	void isValid() const override;
	void clearForm()    noexcept override;
	void storeData()    noexcept override;
	void retrieveData() noexcept override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::Entry*  inputGroupName       = nullptr;
	Gtk::Button* btnGroupDefaultColor = nullptr;

	/// Box in this dialog where selected element links are displayed.
	OrdenableFlowBox* boxElements = nullptr;

	/// Static configuration for the element link selector.
	DialogSelect::SelectionRequest elementRequest;

	DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(XMLHelper* values) noexcept override;
	const string& getType() const noexcept override { return TYPE_GROUP; }
	Storage::Data* createData(StringUMap& rawData) const noexcept override;
	void wireChildrenDialogs()       noexcept override;
	void disconnectChildrenDialogs() noexcept override;
};

} // namespace
