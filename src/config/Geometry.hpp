/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Geometry.hpp
 * @since     Jun 7, 2026
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

#include "XMLHelper.hpp"

#pragma once

#define UI_GEOMETRY_FILE "geometry.xml"
#define UI_GEOMETRY_TYPE "WindowGeometry"

namespace LEDSpicerUI::Config {

/**
 * LEDSpicerUI::Config::Geometry
 *
 * Remembers window sizes (and MainWindow's position) across sessions.
 *
 * Flow: load() → registerWindow() per window → terminate() at shutdown.
 */
class Geometry {

public:

	Geometry(const Geometry&)            = delete;
	Geometry& operator=(const Geometry&) = delete;

	static Geometry& get() noexcept { return instance; }

	/**
	 * Reads geometry.xml into the in-memory map.
	 */
	void load() noexcept;

	/**
	 * Enrols a resizable window and applies any stored size now.
	 * If id is the MainWindow id, also applies stored position.
	 *
	 * @param win the window to track.
	 * @param id  stable identifier (the glade widget id).
	 */
	void registerWindow(Gtk::Window* win, const string& id) noexcept;

	/**
	 * Enrols a widget whose persisted state is a single integer position.
	 * Currently used for GtkPaned dividers, extensible to any future
	 * widget that exposes a single position value. If a stored value
	 * exists, applies it; otherwise places the divider so the second
	 * pack takes ~25% of the MainWindow height.
	 *
	 * @param pane the paned to track.
	 * @param id   stable identifier (the glade widget id).
	 */
	void registerPosition(Gtk::Paned* pane, const string& id) noexcept;

	/**
	 * Captures current sizes from every registered window, compares
	 * against the loaded values, writes geometry.xml only if any
	 * value diverged. Call once at MainWindow dtor.
	 */
	void terminate() noexcept;

private:

	/// Main window id necessary only for its position tracking.
	static const string MAIN_WINDOW_ID;

	/// Registry record.
	struct Entry {
		Gtk::Window* win {nullptr};
		int          w   {-1};
		int          h   {-1};
	};

	static Geometry instance;

	/// Dialogs size data.
	std::unordered_map<string, Entry> entries;

	/// Single-value position registry (paned dividers today, extensible).
	std::unordered_map<string, std::pair<Gtk::Paned*, int>> positions;

	/// Captured / stored MainWindow position.
	std::pair<int, int> mainWindowPos {-1, -1};

	Geometry()  = default;
	~Geometry() = default;

	void save() noexcept;
};

} // namespace
