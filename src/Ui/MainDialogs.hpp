/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainDialogs.hpp
 * @since     Nov 15, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "DialogImport.hpp"
#include "DataDialogs/DialogDevice.hpp"
#include "DataDialogs/DialogRestrictor.hpp"
#include "DataDialogs/DialogProcess.hpp"
#include "DataDialogs/DialogGroup.hpp"
#include "DataDialogs/DialogInput.hpp"

#ifndef UI_MAINDIALOGS_HPP_
#define UI_MAINDIALOGS_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::MainDialogs
 *
 * Main helper class to store all dialogs.
 *
 * This class manages and stores various dialogs used in the application.
 */
class MainDialogs {

public:

	MainDialogs(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::ApplicationWindow* window);

	virtual ~MainDialogs();

protected:

	/**
	 * @name Dialogs
	 * @{
	 */

	DialogImport
		/// Import configuration dialog.
		dialogImportConfig,
		/// Import input dialog.
		dialogImportInput;

	/// File chooser dialog.
	Gtk::FileChooserDialog dialogSelectWorkingDirectory;
	/**
	 * @}
	 */

	/**
	 * @name Storage Collections
	 *  @{
	 */
	Storage::BoxButtonCollection
		/// Created devices in the dialog devices.
		devices,
		/// Created restrictors in the dialog devices.
		restrictors,
		/// Created processes in the dialog devices.
		processes,
		/// Created groups in the dialog devices.
		groups,
		/// Created inputs in the dialog devices.
		inputs;
	/**
	 * @}
	 */
};

} /* namespace */

#endif /* UI_MAINDIALOGS_HPP_ */
