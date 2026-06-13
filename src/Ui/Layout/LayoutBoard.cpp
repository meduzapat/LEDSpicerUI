/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LayoutBoard.cpp
 * @since     Jun 13, 2026
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

#include "LayoutBoard.hpp"
#include "config/Settings.hpp"

using namespace LEDSpicerUI::Ui::Layout;

LayoutBoard::LayoutBoard(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>&) noexcept :
	Gtk::Layout(obj)
{
	Config::Settings::get().onLayoutGridChanged([this]() { queue_draw(); });
}

LayoutBoard::~LayoutBoard() noexcept {
	Config::Settings::get().onLayoutGridChanged(nullptr);
}

bool LayoutBoard::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
	const int g {Config::Settings::get().getLayoutGrid()};
	if (g > 0) {
		const auto a {get_allocation()};
		const int w {a.get_width()}, h {a.get_height()};
		cr->save();
		cr->set_source_rgba(0.5, 0.5, 0.5, 0.25);
		cr->set_line_width(1.0);
		for (int x {0}; x < w; x += g) {
			cr->move_to(x + 0.5, 0);
			cr->line_to(x + 0.5, h);
		}
		for (int y {0}; y < h; y += g) {
			cr->move_to(0, y + 0.5);
			cr->line_to(w, y + 0.5);
		}
		cr->stroke();
		cr->restore();
	}
	return Gtk::Layout::on_draw(cr);
}
