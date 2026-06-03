/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Feb 12, 2023
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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Error
 */
class Message {

public:

	Message(const string& errorMessage) : error(errorMessage) {}

	virtual ~Message() = default;

	/**
	 * Connects dialogs.
	 * @param builder
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* main);

	/**
	 * Displays the current message in a error dialog.
	 * @param transient
	 * @param heading optional bold heading shown above the body; defaults to "Error".
	 */
	void displayError(Gtk::Window* transient = nullptr, const string& heading = emptyString);

	/**
	 * Display an error message.
	 * @param errorMessage
	 * @param transient
	 * @param heading optional bold heading shown above the body; defaults to "Error".
	 */
	static void displayError(
		const string& errorMessage,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	);

	/**
	 * Display and informative message.
	 * @param errorMessage
	 * @param transient
	 * @param heading optional bold heading shown above the body; defaults to "Information".
	 */
	static void displayInfo(
		const string& infoMessage,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	);

	/**
	 * Ask a question that can be answered with yes or now.
	 * @param question
	 * @param transient
	 * @param heading optional bold heading shown above the body; defaults to "Question".
	 * @return the answer.
	 */
	static Gtk::ResponseType ask(
		const string& question,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	);

	/**
	 * Starts collecting error messages instead of failing one-by-one.
	 * While batching, collect() appends to an internal buffer. Call endBatch()
	 * to retrieve the accumulated text and turn batching off.
	 *
	 * Intended for load paths where many small errors should be reported as a
	 * single consolidated message rather than spamming the user with dialogs.
	 */
	static void beginBatch() noexcept;

	/**
	 * Appends a line to the batch buffer. No-op if batching is off, so callers
	 * can use it unconditionally inside catch blocks during a load.
	 */
	static void collect(const string& line) noexcept;

	/**
	 * Returns the accumulated batch text, clears the buffer and turns batching
	 * off. Empty string if nothing was collected.
	 */
	static string endBatch() noexcept;

	/**
	 * @return true while a batch is active.
	 */
	static bool isBatching() noexcept;

	/**
	 * Returns and resets the error message.
	 * @return
	 */
	string takeMessage();

	static Gtk::Window& getMain() noexcept { return *main; }

protected:

	enum class Kind {Info, Error, Question};

	static Gtk::ResponseType handleDialog(
		const string& message,
		Kind kind,
		Gtk::Window* transient,
		const string& heading = emptyString
	);

	string error;

	static Gtk::Dialog*   dialog;
	static Gtk::Image*    icon;
	static Gtk::Label*    primary;
	static Gtk::TextView* body;
	static Gtk::Button*   btnNo;
	static Gtk::Button*   btnYes;
	static Gtk::Button*   btnClose;

	// Keeps a pointer to the main screen to center.
	static Gtk::Window* main;
};

} // namespace
