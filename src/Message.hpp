/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Feb 12, 2023
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

#include <gtkmm.h>

#include <unordered_map>
using std::unordered_map;

#include <vector>
using std::vector;

#include <string>
using std::string;

// Compiler settings.
#include "config.h"

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_ 1

#define NAME      "name"
#define ID        "boardId"

#define PIN       "led"
#define RED_PIN   "red"
#define GREEN_PIN "green"
#define BLUE_PIN  "blue"
#define SOLENOID  "solenoid"
#define TIME_ON   "timeOn"

#define DEFAULT_COLOR  "defaultColor"
#define DEFAULT_SOLENOID 50

#define PIN_COLOR      "pin_white"
#define SOLENOID_COLOR "pin_solenoid"
#define RED_COLOR      "pin_red"
#define GREEN_COLOR    "pin_green"
#define BLUE_COLOR     "pin_blue"
#define NO_COLOR       ""

#define DEFAULT_ELEMENT_TYPE "9"
namespace LEDSpicerUI {

/**
 * LEDSpicerUI::Error
 */
class Message {

public:

	Message(const string& errorMessage) : error(errorMessage) {}

	/**
	 * Throws an error and displays a error message dialog over transient window.
	 * @param errorMessage text message to display.
	 * @param transient the window to be over.
	 */
	Message(const string& errorMessage, Gtk::Window* transient);

	virtual ~Message() = default;

	/**
	 * Connects dialogs.
	 * @param builder
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* main);

	/**
	 * Displays the current message in a error dialog.
	 * @param transient
	 */
	void displayError(Gtk::Window* transient = nullptr);

	/**
	 * Display an error message.
	 * @param errorMessage
	 * @param transient
	 */
	static void displayError(const string& errorMessage, Gtk::Window* transient = nullptr);

	/**
	 * Display and informative message.
	 * @param errorMessage
	 * @param transient
	 */
	static void displayInfo(const string& infoMessage, Gtk::Window* transient = nullptr);

	/**
	 * Ask a question that can be answered with yes or now.
	 * @param question
	 * @param transient
	 * @return the answer.
	 */
	static int ask(const string& question, Gtk::Window* transient = nullptr);

	/**
	 * Returns and resets the error message.
	 * @return
	 */
	string getMessage();

	/**
	 * Check if a string stores a numeric value
	 * @param number
	 * @return true if the content is a number
	 */
	static bool isNumber(const string& number);

	/**
	 * Check if a string number is between two values (inclusive)
	 * @param number
	 * @param low
	 * @param hight
	 * @return
	 */
	static bool isBetween(const string& number, int low, int hight);

	/**
	 * Returns the color luminance.
	 * @param color
	 * @return
	 */
	static double getLiminance(const string& color);

protected:

	string error;

	static Gtk::MessageDialog* errorDialog;

	static Gtk::MessageDialog* infoDialog;

	static Gtk::MessageDialog* questionDialog;

	// Keeps a pointer to the main screen to center.
	static Gtk::Window* main;

	static int handleDialog(const string& message, Gtk::MessageDialog* dialog, Gtk::Window* transient);
};

} /* namespace LEDSpicerUI */

#endif /* MESSAGE_HPP_ */
