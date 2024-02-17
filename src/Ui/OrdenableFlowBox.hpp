/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      OrdenableFlowBox.hpp
 * @since     Apr 10, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#ifndef UI_ORDENABLEFLOWBOX_HPP_
#define UI_ORDENABLEFLOWBOX_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::OrdenableFlowBox
 */
class OrdenableFlowBox: public Gtk::FlowBox {

public:

	OrdenableFlowBox() = delete;

	OrdenableFlowBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::FlowBox(obj) {}

	OrdenableFlowBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder, const string& up, const string& dn);

	virtual ~OrdenableFlowBox() = default;

	size_t getSize();

	void wipe();

};

} /* namespace */

#endif /* UI_ORDENABLEFLOWBOX_HPP_ */
