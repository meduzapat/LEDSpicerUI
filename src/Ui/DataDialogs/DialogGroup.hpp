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
#include "DialogPrompt.hpp"
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

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

	void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) noexcept override;

	/**
	 * Links target into the group identified by groupId. No-op if the group
	 * doesn't exist, the target has PROP_NO_SELECT, or the link already
	 * exists. DS is configured before the call.
	 */
	void linkInto(const string& groupId, Storage::Data* target) noexcept;

protected:

	Gtk::Entry*  inputGroupName       = nullptr;
	Gtk::Button* btnGroupDefaultColor = nullptr;

	/// Box in this dialog where selected element links are displayed.
	OrdenableFlowBox* boxElements = nullptr;

	/// Configuration for the element link selector.
	DialogSelect::SelectionRequest elementRequest;

	/// System "All" group, constructed and owned by this dialog.
	Storage::Group* allGroup = nullptr;

	DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(DataMap& values) noexcept override;
	const string& getType()          const noexcept override { return TYPE_GROUP; }
	Storage::Data* createData(Values& rawData) const noexcept override;

	void wireChildrenDialogs()       noexcept override;
	void disconnectChildrenDialogs() noexcept override;
};

} // namespace
