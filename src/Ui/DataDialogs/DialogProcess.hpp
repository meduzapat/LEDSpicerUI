/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProcess.hpp
 * @since     May 03, 2023
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
#include "Storage/Process.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogProcess
 */
class DialogProcess: public DialogForm, public SingletonDialog<DialogProcess> {

	friend class Gtk::Builder;

public:

	virtual ~DialogProcess() = default;

	void load(XMLHelper* values)  noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::Entry
		* inputProcessName = nullptr,
		* inputSystemType  = nullptr,
		* inputRomPosition = nullptr;

	DialogProcess(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	const string& getType() const noexcept override { return TYPE_MAP; }

	Storage::Data* createData(StringUMap& rawData) const noexcept override;
};

} // namespace
