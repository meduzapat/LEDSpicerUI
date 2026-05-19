/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProjectFile.cpp
 * @since     May 14, 2026
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

#include "ProjectFile.hpp"

using namespace LEDSpicerUI::Config;

void ProjectFile::saveFile(const string& path, const string& content) {

	// Debug files.
	if (Settings::get().shouldDebugFiles()) {
		Gtk::Dialog dialog("DEBUG: " + path);
		dialog.set_default_size(700, 500);
		dialog.set_resizable(true);
		dialog.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
		dialog.set_transient_for(Message::getMain());

		auto scrolled {Gtk::manage(new Gtk::ScrolledWindow())};
		scrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled->set_hexpand(true);
		scrolled->set_vexpand(true);

		auto textView {Gtk::manage(new Gtk::TextView())};
		textView->get_buffer()->set_text(content);
		textView->set_editable(false);
		textView->set_monospace(true);
		scrolled->add(*textView);

		dialog.get_content_area()->pack_start(*scrolled, true, true, 0);
		dialog.show_all();
		dialog.run();
		return;
	}
	// Normal save.
	Glib::file_set_contents(path, content);
}
