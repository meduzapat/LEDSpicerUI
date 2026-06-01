/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StatusBar.hpp
 * @since     Jun 2026
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

#include <gtkmm.h>
#include <string>

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::StatusBar
 *
 * Singleton wrapper around GtkStatusbar.
 * MainWindow calls initialize() once with the builder; everywhere else uses getInstance().
 */
class StatusBar {

public:

	StatusBar(const StatusBar&)            = delete;
	StatusBar& operator=(const StatusBar&) = delete;

	static StatusBar& getInstance() noexcept { return instance; }

	/**
	 * Binds the singleton to the GtkStatusbar widget from the builder.
	 * Must be called once by MainWindow before any other use.
	 */
	static void initialize(const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/**
	 * Replaces the current message with a new one.
	 * @param message Text to display.
	 */
	void push(const std::string& message) noexcept;

	/**
	 * Clears the current message.
	 */
	void clear() noexcept;

private:

	StatusBar() = default;

	static StatusBar instance;

	Gtk::Statusbar* bar       = nullptr;
	guint           contextId = 0;
};

} // namespace
