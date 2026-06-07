/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SingletonDialog.hpp
 * @since     Feb 23, 2026
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

#include <stdexcept>

#include "config/Geometry.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

template <typename Derived>
class SingletonDialog {

public:

	static Derived* getInstance() {
		return instance;
	}

	static void buildInstance(const Glib::RefPtr<Gtk::Builder>& builder, std::string_view widgetId) {
		if (not instance) {
			builder->get_widget_derived(widgetId.data(), instance);
			if (not instance) {
				throw std::runtime_error("Failed to load widget: " + std::string(widgetId));
			}
			Config::Geometry::get().registerWindow(instance, std::string(widgetId));
		}
	}

protected:

	static Derived* instance;

	SingletonDialog() noexcept = default;

};

template <typename Derived>
Derived* SingletonDialog<Derived>::instance = nullptr;

} // namespace

