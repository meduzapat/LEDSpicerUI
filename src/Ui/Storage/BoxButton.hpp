/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	  BoxButton.hpp
 * @since	 Feb 13, 2023
 * @author	Patricio A. Rossi (MeduZa)
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

#include "Data.hpp"

#ifndef UI_BOXBUTTON_HPP_
#define UI_BOXBUTTON_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::GroupBoxButton
 * @brief A class that contains a form with data, is used to handle and display that information.
 */
class BoxButton : public Gtk::HBox {

public:

	BoxButton() = delete;

	BoxButton(Data* form);

	/**
	 * @brief Move constructor for BoxButton.
	 * @param other The BoxButton instance to move from.
	 */
	BoxButton(BoxButton&& other) :
		Gtk::HBox(std::move(other)),
		data(std::exchange(other.data, nullptr)),
		label(std::move(other.label)) {}

	BoxButton& operator=(BoxButton&& other) {
		if (this != &other) {
			Gtk::HBox::operator=(std::move(other));
			data  = std::exchange(other.data, nullptr);
			label = std::move(other.label);
		}
		return *this;
	}

	bool operator==(const BoxButton& other) const {
		return (this == &other);
	}

	virtual ~BoxButton();

	/**
	 * Adds a button to the front of the buttons.
	 * @param button
	 */
	void packButtonStart(Gtk::Button& button);

	/**
	 * Returns the internal form that contains the values.
	 * @return
	 */
	Data* getData();

	/**
	 * Return the Form
	 * @return
	 */
	const Data* getData() const;

	/**
	 * Updates it own label that represents in a human way what is stored inside the box.
	 */
	void updateLabel();

protected:

	/// Access to the data storage.
	Data* data;

	/// Label to display.
	Gtk::Label* label;

};

} /* namespace */

#endif /* UI_BOXBUTTON_HPP_ */
