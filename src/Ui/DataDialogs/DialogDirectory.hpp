/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDirectory.hpp
 * @since     Mar 2, 2026
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
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogDirectory
 * Generic dialog for creating and renaming directory nodes.
 * Never serializes — DirectoryEntry objects are runtime-only.
 */
class DialogDirectory : public DialogForm, public SingletonDialog<DialogDirectory> {

	friend class Gtk::Builder;

public:

	/**
	 * Holds all section-specific context for this dialog.
	 * Declare as a const member of the owning navigator; call setSettings() once
	 * after buildInstance() to wire the dialog to the correct box and collection.
	 */
	struct SettingRequest {
		/// Reference to the navigator's own box pointer.
		OrdenableFlowBox*& box;
		/// Section type label Fed into getType().
		const string& typeLabel;

		/// Function to call to enter into a dir, used by BoxButtons
		std::function<void(Storage::DirectoryEntry*)> enterDir;
	};

	virtual ~DialogDirectory() = default;

	/**
	 * Wires the dialog to a specific section.
	 * Must be called once after buildInstance(), before any setOwner() call.
	 * @param req SettingRequest owned by the calling navigator.
	 */
	void setSettings(const SettingRequest& req) noexcept;

	void load(DataMap& values)    noexcept override {}
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	/// Active settings. Set once by setSettings(); never null after that.
	const SettingRequest* setting = nullptr;

	/// Directory name input.
	Gtk::Entry* entryDirectoryName = nullptr;

	DialogDirectory(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	Storage::Data* createData(StringUMap& rawData) const noexcept override;

	const string& getType() const noexcept override;

	void addButtons(Storage::BoxButton& boxButton) noexcept override;

};

} // namespace
