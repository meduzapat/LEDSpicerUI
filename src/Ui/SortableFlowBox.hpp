/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SortableFlowBox.hpp
 * @since     Apr 10, 2023
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

#include "Message.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::SortableFlowBox
 */
class SortableFlowBox: public Gtk::FlowBox {

public:

	SortableFlowBox() = delete;

	SortableFlowBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>&) : Gtk::FlowBox(obj) {}

	SortableFlowBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder, const string& up, const string& dn, const string& first = "", const string& last = "");

	virtual ~SortableFlowBox() = default;

	size_t getSize() const noexcept;

	void wipe();

};

} // namespace
