/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Selection.cpp
 * @since     Mar 24, 2026
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

#include "Selection.hpp"
#include "DirNode.hpp"

using namespace LEDSpicerUI::Ui::Storage;

namespace {

// Full path for DirNode-backed items; bare pretty name otherwise.
string pickerLabel(Data* data) noexcept {
	if (data->getProperties().isSet(LEDSpicerUI::Constants::FILENAME))
		if (auto* node = dynamic_cast<DirNode*>(data))
			return node->getFullPath();
	return data->createPrettyName();
}

} // namespace

Selection::Selection(Data* data) noexcept : Gtk::Button(pickerLabel(data)), data(data) {
	set_halign(Gtk::Align::ALIGN_FILL);
	set_margin_top(2);
	set_margin_bottom(2);
	set_margin_start(2);
	set_margin_end(2);
}
