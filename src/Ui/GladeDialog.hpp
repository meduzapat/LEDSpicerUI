/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GladeDialog.hpp
 * @since     Feb 1, 2026
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

/**
 * Template that initialize Dialogs from Glade using CRTP (Curiously Recurring Template Pattern)
 * @tparam Derived
 */
template<typename Derived>
class GladeDialog : public Gtk::Dialog {

public:

	GladeDialog() = delete;

	virtual ~GladeDialog() = default;

	static Derived* getInstance() {
		return instance;
	}

	static void initialize(
		const Glib::RefPtr<Gtk::Builder>& builder,
		const string& widgetId
	) {
		if (not instance)
			builder->get_widget_derived(widgetId, instance);
	}

protected:

	static Derived* instance;

	GladeDialog(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>&) : Gtk::Dialog(obj) {}
};

template<typename Derived>
Derived* GladeDialog<Derived>::instance = nullptr;

