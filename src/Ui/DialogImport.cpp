/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogImport.cpp
 * @since     May 7, 2023
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

using namespace LEDSpicerUI::Ui;

const unordered_map<const DialogImport::Types, const string> DialogImport::setups {
	{Types::CONFIG, "Select a config file"},
	{Types::INPUT,  "Select one or more input plugin files"}
};

DialogImport::DialogImport(const Types type, Gtk::Window* parent) :
	FileChooserDialog(
		setups.at(type),
		Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN,
		Gtk::DialogFlags::DIALOG_MODAL | Gtk::DialogFlags::DIALOG_DESTROY_WITH_PARENT | Gtk::DialogFlags::DIALOG_USE_HEADER_BAR
	)
{

	set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
	set_transient_for(*parent);
	set_default_size(900, 700);

	Glib::RefPtr<Gtk::FileFilter> filter(Gtk::FileFilter::create());
	filter->add_mime_type("application/xml");

	// Set buttons.
	add_button("_Cancel", Gtk::RESPONSE_CANCEL)->get_style_context()->add_class("backgroundRed");
	btbOk = add_button("_Open",   Gtk::RESPONSE_OK);
	btbOk->get_style_context()->add_class("backgroundGreen");
	auto box = get_content_area();
	switch (type) {
	case Types::CONFIG:
		setConfigBox(box);
		set_select_multiple(false);
		filter->set_name("LEDSpicer config");
		filter->add_pattern("*.conf");
		break;
	default:
		set_select_multiple(true);
		filter->set_name("LEDSpicer data Files");
		filter->add_pattern("*.xml");
		break;
	}
	add_filter(filter);
	box->show_all();
}

uint8_t DialogImport::getConfigParameters() {
	return flags;
}

void DialogImport::setConfigBox(Gtk::Box* box) {
	box->add(*Gtk::make_managed<Gtk::Label>("Select what to import"));
	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL, 2);
	box->add(*hbox);
	hbox->get_style_context()->add_class("formContainer");

	Gtk::ToggleButton
		* importConfig      = Gtk::make_managed<Gtk::ToggleButton>("Import Config"),
		* importDevices     = Gtk::make_managed<Gtk::ToggleButton>("Import Devices"),
		* importRestrictors = Gtk::make_managed<Gtk::ToggleButton>("Import Restrictors"),
		* importMappings    = Gtk::make_managed<Gtk::ToggleButton>("Import Process Mappings");

	importConfig->signal_toggled().connect([&, importConfig]() {
		setFlags(IMPORT_CONFIG, importConfig->get_active());
	});
	importDevices->signal_toggled().connect([&, importDevices]() {
		setFlags(IMPORT_DEVICES, importDevices->get_active());
	});
	importRestrictors->signal_toggled().connect([&, importRestrictors]() {
		setFlags(IMPORT_RESTRICTORS, importRestrictors->get_active());
	});
	importMappings->signal_toggled().connect([&, importMappings]() {
		setFlags(IMPORT_MAPPINGS, importMappings->get_active());
	});
	signal_show().connect([=]() {
		importConfig->set_active(true);
		importDevices->set_active(true);
		importRestrictors->set_active(true);
		importMappings->set_active(true);
	});
	hbox->add(*importConfig);
	hbox->add(*importDevices);
	hbox->add(*importRestrictors);
	hbox->add(*importMappings);
}

void DialogImport::setFlags(uint8_t value, bool on) {
	if (on)
		flags |= value;
	else
		flags &= ~value;
	btbOk->set_sensitive(flags);
};
