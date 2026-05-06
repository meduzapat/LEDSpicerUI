/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.hpp
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

#include "DialogForm.hpp"
#include "DialogSelect.hpp"
#include "DirectoryAware.hpp"
#include "Storage/Profile.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogProfile
 * Dialog to create or edit profiles.
 * Profile owns four independent link type selectors (elements, groups,
 * animations, inputs), each wired to DS immediately before use via setUpSelector().
 */
class DialogProfile :
	public DialogForm,
	public DirectoryAware,
	public SingletonDialog<DialogProfile>
{

	friend class Gtk::Builder;

public:

	virtual ~DialogProfile() = default;

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::Entry*  inputProfileName          = nullptr;
	Gtk::Button* btnProfileBackgroundColor = nullptr;

	OrdenableFlowBox
		* boxProfileAlwaysOnElements = nullptr,
		* boxProfileAlwaysOnGroups   = nullptr,
		* boxProfileAnimations       = nullptr,
		* boxProfileInputs           = nullptr;

	DialogProfile(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(DataMap& values) noexcept override;
	const string& getType() const noexcept override { return TYPE_PROFILE; }
	Storage::Data* createData(StringUMap& rawData) const noexcept override;

private:

	DialogSelect::SelectionRequest
		alwaysOnElementsRequest,
		alwaysOnGroupsRequest,
		animationsRequest,
		inputsRequest;

	/**
	 * Wires DialogSelect for one link type immediately before use.
	 * @param collection Key of the child collection on the current Profile.
	 * @param req        Configuration for this link type.
	 */
	void setUpSelector(
		const string& collection,
		const DialogSelect::SelectionRequest& req
	) noexcept;
};

} // namespace
