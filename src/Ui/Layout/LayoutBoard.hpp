/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LayoutBoard.hpp
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

#include <gtkmm/builder.h>
#include <gtkmm/layout.h>

#pragma once

namespace LEDSpicerUI::Ui::Layout {

/**
 * LEDSpicerUI::Ui::Layout::LayoutBoard
 *
 * Gtk::Layout subclass that paints the snap grid under its children
 * when Settings::LAYOUT_GRID is non-zero. Repaints on grid changes
 * via Settings::onLayoutGridChanged.
 */
class LayoutBoard: public Gtk::Layout {

public:

	LayoutBoard(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>&) noexcept;

	virtual ~LayoutBoard() noexcept;

	LayoutBoard(const LayoutBoard&)            = delete;
	LayoutBoard& operator=(const LayoutBoard&) = delete;

protected:

	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

} // namespace
