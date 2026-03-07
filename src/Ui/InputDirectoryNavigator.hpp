/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputDirectoryNavigator.hpp
 * @since     Feb 16, 2026
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

#include "DirectoryNavigator.hpp"
#include "DataDialogs/DialogDirectory.hpp"
#include "DataDialogs/DialogInput.hpp"
#include "DataDialogs/DialogInputMap.hpp"
#include "config/InputFile.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::InputDirectoryNavigator
 * Specialized navigator for Input files.
 * Owns the inputs collection, handles loading/saving, manages DialogInput.
 */
class InputDirectoryNavigator : public DirectoryNavigator {

public:

	/**
	 * Creates navigator and sets up DialogInput.
	 * @param builder Glade builder to fetch widgets.
	 */
	InputDirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder, Gtk::Window* parentWindow);

	virtual ~InputDirectoryNavigator();

	void clear() override;

protected:

	/// Import input dialog.
	DialogImport dialogImportInput;

	Gtk::Button
		* btnHome            = nullptr,
		* btnNewInputFolder  = nullptr;
	Gtk::Box* boxBreadcrumb  = nullptr;

	void wireDialogs(Storage::DirectoryEntry* dir) override;

};

} // namespace
