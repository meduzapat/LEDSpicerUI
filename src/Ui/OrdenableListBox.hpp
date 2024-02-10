/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      OrdenableListBox.hpp
 * @since     May 4, 2023
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

#ifndef UI_ORDENABLELISTBOX_HPP_
#define UI_ORDENABLELISTBOX_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::OrdenableListBox
 */
class OrdenableListBox: public Gtk::ListBox {

public:

	OrdenableListBox() = delete;

	OrdenableListBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::ListBox(obj) {}

	OrdenableListBox(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder, const string& up, const string& dn);

	virtual ~OrdenableListBox() = default;

	size_t getSize();

	void wipe();

	/**
	 * for listbox with checkboxes, will sort and mark the checkboxes based on the array.
	 * @param values the list of values with the order to set.
	 */
	void sortAndMark(vector<string> values);

	/**
	 * for listbox with checkboxes.
	 * @return Returns a list of selected checkbox.
	 */
	vector<string> getCheckedValues();

};

} /* namespace */

#endif /* UI_ORDENABLELISTBOX_HPP_ */
