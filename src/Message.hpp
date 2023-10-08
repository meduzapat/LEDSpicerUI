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

#include "Defaults.hpp"

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_ 1

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
