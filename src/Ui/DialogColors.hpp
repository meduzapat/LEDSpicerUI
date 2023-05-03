/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogColors.hpp
 * @since     Feb 22, 2023
 * @author    Patricio A. Rossi (MeduZa)
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

#include "Message.hpp"

#ifndef UI_DIALOGCOLORS_HPP_
#define UI_DIALOGCOLORS_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogColors
 * Handles the color selectors and other color features.
 */
class DialogColors: public Gtk::Dialog {

	friend class Gtk::Builder;

public:

	DialogColors() = delete;

	virtual ~DialogColors() = default;

	static DialogColors* getInstance();

	static DialogColors* getInstance(Glib::RefPtr<Gtk::Builder> const &builder);

	void setColorsFromFile(const string& colorsFilename);

	/**
	 * Colorize a button with the desired colors class.
	 * @param button
	 * @param name
	 */
	void colorizeButton(Gtk::Button* button, const string& className);

	/**
	 * Check if the color is on the list of colors.
	 * @param colorName
	 * @return
	 */
	bool isValidColor(const string& colorName);

	/**
	 * Converts a button into a color picker for the destination box.
	 * @param button
	 * @param destination
	 */
	void activateColorPicker(Gtk::Button* button, Gtk::FlowBox* destination);

	/**
	 * Converts a button into a dialog color opener.
	 * @param button
	 */
	void activateColorButton(Gtk::Button* button);

	/**
	 * Set no color for all registered color buttons.
	 */
	void resetColorButtons();

protected:

	DialogColors(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const &builder);

	static DialogColors* dc;

	/// Last Selected color.
	string selectedColor;

	/// Css provider for the current color file.
	Glib::RefPtr<Gtk::CssProvider> currentProvider;

	/// Where the colors are stored.
	Gtk::FlowBox* ContainerColorPicker = nullptr;

	/// Predefined colors and clear.
	Gtk::Button
		* BtnColorOn,
		* BtnColorOff,
		* BtnColorRandom,
		* BtnClearColor;

	/// Keep track of color buttons to clean when file is changed.
	static vector<Gtk::Button*> colorButtons;

	/// Keep track of color pickers to clean when file is changed.
	static vector<Gtk::FlowBox*> colorBoxes;

	/**
	 * Callback when a color is selected from the color picker.
	 * @param button
	 */
	void onColorSelected(Gtk::Button* button);

	/**
	 * Replaces the current colors with a new set it creates CSS classes for every new color.
	 * @param colors
	 * @return a CCS string representing the new color set.
	 */
	string setColors(unordered_map<string, string>& colors);

};

} /* namespace */

#endif /* UI_DIALOGCOLORS_HPP_ */
