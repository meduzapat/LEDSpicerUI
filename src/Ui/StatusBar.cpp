/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StatusBar.cpp
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

#include "StatusBar.hpp"

using namespace LEDSpicerUI::Ui;

StatusBar StatusBar::instance;

void StatusBar::initialize(const Glib::RefPtr<Gtk::Builder>& builder) noexcept {
	builder->get_widget("StatusBar", instance.bar);
	if (instance.bar)
		instance.contextId = instance.bar->get_context_id("main");
}

void StatusBar::push(const std::string& message) noexcept {
	if (not bar) return;
	bar->pop(contextId);
	bar->push(message, contextId);
}

void StatusBar::clear() noexcept {
	if (bar) bar->pop(contextId);
}
