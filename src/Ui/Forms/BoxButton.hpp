/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	  BoxButton.hpp
 * @since	 Feb 13, 2023
 * @author	Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include "Form.hpp"

#ifndef UI_BOXBUTTON_HPP_
#define UI_BOXBUTTON_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::GroupBoxButton
 * @brief A class that contains a form with data, is used to handle and display that information.
 */
class BoxButton : public Gtk::HBox {

public:

	BoxButton() = delete;

	BoxButton(string type, Form* form);

	/**
	 * @brief Move constructor for BoxButton.
	 * @param other The BoxButton instance to move from.
	 */
	BoxButton(BoxButton&& other) :
		Gtk::HBox(std::move(other)),
		type(std::move(other.type)),
		form(std::exchange(other.form, nullptr)),
		label(std::move(other.label)),
		btnE(std::exchange(other.btnE, nullptr)),
		btnD(std::exchange(other.btnD, nullptr)) {}

	BoxButton& operator=(BoxButton&& other) {
		if (this != &other) {
			Gtk::HBox::operator=(std::move(other));
			type  = std::move(other.type);
			form  = std::exchange(other.form, nullptr);
			label = std::move(other.label);
			btnE  = std::exchange(other.btnE, nullptr);
			btnD  = std::exchange(other.btnD, nullptr);
		}
		return *this;
	}

	bool operator==(const BoxButton& other) const {
		return (this == &other);
	}

	virtual ~BoxButton();

	/**
	 * Set the function to edit itself.
	 * @param ediCallback
	 */
	void setEditFn(std::function<void()> ediCallback);

	/**
	 * Set the function to delete itself.
	 * @param delCallback
	 */
	void setDelFn(std::function<void()> delCallback);

	/**
	 * Creates a XML representation of the data using the type and the values stored inside the form.
	 * @param contents if set will add this into the tabs body, if empty will create an inline tag
	 * @return
	 */
	string toXML(const string& contents) const;

	/**
	 * Returns the internal form that contains the values.
	 * @return
	 */
	Form* getForm();

	const Form* getForm() const;

	/**
	 * Updates it own label that represents in a human way what is stored inside the box.
	 */
	void updateLabel();

protected:

	/// This it the type of data stored, will be used when creating the xml node.
	string type;

	/// Access to the data form.
	Form* form;

	/// Label to display.
	Gtk::Label* label;

	/// Button delete and edit.
	Gtk::Button
		* btnE,
		* btnD;
};

} /* namespace */

#endif /* UI_BOXBUTTON_HPP_ */
