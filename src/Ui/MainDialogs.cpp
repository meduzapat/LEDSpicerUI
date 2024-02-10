/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainDialogs.cpp
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

#include "MainDialogs.hpp"

using namespace LEDSpicerUI::Ui;

MainDialogs::MainDialogs(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::ApplicationWindow* window) :
	// Set import dialog
	dialogImportConfig(DialogImport::Types::CONFIG, window),
	dialogImportInput(DialogImport::Types::INPUT, window),
	// Project directory opener.
	dialogSelectWorkingDirectory(
		"Select Project Directory",
		Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER,
		Gtk::DialogFlags::DIALOG_MODAL | Gtk::DialogFlags::DIALOG_DESTROY_WITH_PARENT | Gtk::DialogFlags::DIALOG_USE_HEADER_BAR
	),
	inputs(FILENAME)
{

	DialogColors::initialize(builder);

	// Initialize Pin Handler.
	Storage::PinHandler::initialize(builder);

	// Initialize secondary dialogs.
	DataDialogs::DialogElement::initialize(builder);
	DataDialogs::DialogSelectElements::initialize(builder);
	DataDialogs::DialogInputLinkMaps::initialize(builder);
	DataDialogs::DialogInputMap::initialize(builder);
	DataDialogs::DialogRestrictorMap::initialize(builder);
	// Initialize Primary Dialogs.
	DataDialogs::DialogDevice::initialize(builder);
	DataDialogs::DialogRestrictor::initialize(builder);
	DataDialogs::DialogProcess::initialize(builder);
	DataDialogs::DialogGroup::initialize(builder);
	DataDialogs::DialogInput::initialize(builder);

	// Connect primary dialogs with the collections.
	DataDialogs::DialogDevice::getInstance()->setOwner(&devices);
	DataDialogs::DialogRestrictor::getInstance()->setOwner(&restrictors);
	DataDialogs::DialogProcess::getInstance()->setOwner(&processes);
	DataDialogs::DialogGroup::getInstance()->setOwner(&groups);
	DataDialogs::DialogInput::getInstance()->setOwner(&inputs);

	// Create select work directory FileChooserDialog
	dialogSelectWorkingDirectory.set_transient_for(*window);
	dialogSelectWorkingDirectory.set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
	dialogSelectWorkingDirectory.set_default_size(900, 700);

	// Prepare open project dialog functionality.
	dialogSelectWorkingDirectory.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL)->get_style_context()->add_class("backgroundRed");
	dialogSelectWorkingDirectory.add_button("_Select", Gtk::ResponseType::RESPONSE_OK)->get_style_context()->add_class("backgroundGreen");
}

MainDialogs::~MainDialogs() {

	// data dialogs.
	delete DataDialogs::DialogSelectElements::getInstance();
	delete DataDialogs::DialogElement::getInstance();
	delete DataDialogs::DialogRestrictorMap::getInstance();
	delete DataDialogs::DialogRestrictor::getInstance();
	delete DataDialogs::DialogInputLinkMaps::getInstance();
	delete DataDialogs::DialogInputMap::getInstance();
	delete DataDialogs::DialogInput::getInstance();
	delete DataDialogs::DialogProcess::getInstance();
	delete DataDialogs::DialogGroup::getInstance();
	delete DataDialogs::DialogDevice::getInstance();

	// dialogs.
	delete DialogColors::getInstance();
	delete Storage::PinHandler::getInstance();

	// the order is important.
	processes.wipe();
	inputs.wipe();
	groups.wipe();
	devices.wipe();
	restrictors.wipe();
	Storage::CollectionHandler::wipe();

}
