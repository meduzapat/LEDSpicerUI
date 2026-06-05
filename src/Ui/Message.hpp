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
#include "StatusBar.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::Message
 *
 * Dual-purpose error/info/question channel:
 *
 *   1. Throwable — `throw Message("…")` from any layer that detects a user
 *      facing error; the caller catches with `catch (Message& e)` and calls
 *      `e.displayError(this)` to surface it. Used pervasively by
 *      `DialogForm::isValid()`, `XMLHelper`, and the config loaders.
 *
 *   2. Static dialog helpers — `Message::displayError/displayInfo/ask` for
 *      ad-hoc dialogs that do not flow through exception handling.
 *
 * Batch API — `beginBatch` / `collect` / `finishBatch` consolidates many
 * small errors raised during a load into a single deduplicated report
 * instead of spamming the user with one dialog per failure.
 */
class Message {

public:

	Message(const string& message) noexcept : error(message) {}

	virtual ~Message() = default;

	/**
	 * Binds the singleton dialog widgets from the builder.
	 * @param builder
	 * @param mainWindow Main window used as default transient parent and to
	 *                   center dialogs that have no other parent.
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* mainWindow) noexcept;

	/**
	 * Displays the carried message in an error dialog. Use after catching
	 * a `Message` thrown from a lower layer.
	 * @param transient
	 * @param heading Optional bold heading above the body; defaults to "Error".
	 */
	void displayError(Gtk::Window* transient = nullptr, const string& heading = emptyString) noexcept;

	/**
	 * Display an error message.
	 * @param message
	 * @param transient
	 * @param heading Optional bold heading above the body; defaults to "Error".
	 */
	static void displayError(
		const string& message,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	) noexcept;

	/**
	 * Display an informative message.
	 * @param message
	 * @param transient
	 * @param heading Optional bold heading above the body; defaults to "Information".
	 */
	static void displayInfo(
		const string& message,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	) noexcept;

	/**
	 * Ask a question that can be answered with yes or no.
	 * @param message
	 * @param transient
	 * @param heading Optional bold heading above the body; defaults to "Question".
	 * @return The user's response.
	 */
	static Gtk::ResponseType ask(
		const string& message,
		Gtk::Window* transient = nullptr,
		const string& heading = emptyString
	) noexcept;

	/**
	 * Starts collecting error messages instead of failing one-by-one.
	 * While batching, collect() appends to an internal buffer. Call endBatch()
	 * or finishBatch() to retrieve / report the accumulated text.
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
	 * Ends the batch and reports the result to the user. If anything was
	 * collected, pops an error dialog with the report and pushes a warning to
	 * the status bar; otherwise pushes @p action as a success status.
	 * @param action Past-tense verb describing the operation, e.g.
	 *               "Config imported", "Project loaded".
	 */
	static void finishBatch(const string& action) noexcept;

	/**
	 * @return true while a batch is active.
	 */
	static bool isBatching() noexcept;

	/**
	 * Returns and resets the carried message.
	 */
	string takeMessage() noexcept;

	static Gtk::Window& getMain() noexcept { return *main; }

protected:

	enum class Kind : uint8_t {Info, Error, Question};

	static Gtk::ResponseType handleDialog(
		const string& message,
		Kind kind,
		Gtk::Window* transient,
		const string& heading = emptyString
	) noexcept;

	/// Counts wrapped lines for `message`, assuming WRAP_COLS-wide hard wrap.
	static unsigned countWrappedLines(const string& message) noexcept;

	/// Drains the batch buffer into a sorted, deduplicated report. Returns the
	/// formatted text and the deduped line count so callers can present both
	/// without recounting.
	struct BatchReport {
		string text;
		size_t count = 0;
	};
	static BatchReport drainBatch() noexcept;

	string error;

	// Dialog widgets bound by initialize().
	static Gtk::Dialog*   dialog;
	static Gtk::Image*    icon;
	static Gtk::Label*    primary;
	static Gtk::TextView* body;
	static Gtk::Button
		* btnNo,
		* btnYes,
		* btnClose;

	/// Main window pointer, used to center dialogs that have no transient parent.
	static Gtk::Window* main;

	// Batch state.
	inline static bool   batching = false;
	inline static string batchBuffer;

	// Manual sizing constants — GtkTextView + GtkScrolledWindow do not cooperate
	// well with auto height-for-width, so the dialog is sized from a logical
	// wrap column and the resulting line count, clamped to a sane range.
	static constexpr unsigned
		WRAP_COLS = 80,
		MIN_LINES = 3,
		MAX_LINES = 20;
	static constexpr int
		CHAR_W_PX = 8,    // ≈ Cantarell 10pt advance width
		LINE_H_PX = 20,   // ≈ Cantarell 10pt line height
		CHROME_W  = 80,   // icon column + margins + scrollbar gutter
		CHROME_H  = 140;  // heading row + button row + margins
};

} // namespace
