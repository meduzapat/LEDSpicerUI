/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StatusBar.hpp
 * @since     Jun 2026
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

#include <gtkmm.h>
#include <deque>
#include <string>

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::StatusBar
 *
 * Singleton wrapper around GtkStatusbar with auto-dismiss, FIFO queueing,
 * hover-pause and a persistent idle message.
 *
 * Channel policy — when to use the status bar vs. a Message dialog:
 *   Status bar : transient feedback the user can ignore (save confirmations,
 *                connection state, welcome/ready text, low-severity info),
 *                or a persistent idle message describing current state.
 *   Message    : requires user acknowledgment, contains copyable detail,
 *                represents a destructive intent, or aggregates a report.
 *
 * Status bar messages must be short and informative. Long text is ellipsized
 * (no tooltip); if a message does not fit, rewrite it or route it to a dialog.
 */
class StatusBar {

public:

	enum class Severity {Info, Success, Warning, Error};

	StatusBar(const StatusBar&)            = delete;
	StatusBar& operator=(const StatusBar&) = delete;

	static StatusBar& getInstance() noexcept { return instance; }

	/**
	 * Binds the singleton to the GtkStatusbar widget from the builder.
	 * Must be called once by MainWindow before any other use.
	 */
	static void initialize(const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/**
	 * Display a status message.
	 *
	 * Transient messages (persistent=false) are queued FIFO and shown each for
	 * a duration proportional to their length (clamped 3–8 s, doubled for
	 * warnings/errors). The timer pauses while the pointer is over the bar.
	 * When the transient queue empties, the persistent message (if any) is
	 * restored as the idle text.
	 *
	 * Persistent messages (persistent=true) replace the previous persistent
	 * text and become the bar's idle text. Only one is stored at a time.
	 *
	 * @param message    Short, single-line text.
	 * @param severity   Affects display duration.
	 * @param persistent If true, sets the idle text instead of queueing a transient.
	 */
	void push(
		const std::string& message,
		Severity severity = Severity::Info,
		bool persistent = false
	) noexcept;

	/**
	 * Clears the transient queue and any currently displayed transient
	 * message. The persistent message (if any) is restored.
	 */
	void clear() noexcept;

private:

	StatusBar() = default;

	struct PendingMessage {
		std::string text;
		Severity    severity;
	};

	static StatusBar instance;

	void showNext() noexcept;
	void displayTransient(const PendingMessage& msg) noexcept;
	void renderPersistent() noexcept;
	void applySeverity(Severity severity) noexcept;
	static unsigned durationFor(const std::string& msg, Severity sev) noexcept;

	bool onTimeout() noexcept;
	bool onPointerEnter(GdkEventCrossing* event) noexcept;
	bool onPointerLeave(GdkEventCrossing* event) noexcept;

	Gtk::Statusbar* bar       = nullptr;
	Gtk::Label*     label     = nullptr;
	guint           contextId = 0;

	std::deque<PendingMessage> queue;
	bool            showingTransient = false;

	std::string     persistentText;
	Severity        persistentSeverity {Severity::Info};
	bool            hasPersistent    = false;

	sigc::connection timeoutConn;
	unsigned         currentDurationMs  = 0;
	gint64           displayStartUs     = 0;
	unsigned         remainingMsOnHover = 0;
	bool             paused             = false;
};

} // namespace
