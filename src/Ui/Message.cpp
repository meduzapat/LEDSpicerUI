/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.cpp
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

#include "Message.hpp"

using namespace LEDSpicerUI::Ui;

namespace {

constexpr unsigned WRAP_COLS = 80;
constexpr unsigned MIN_LINES = 3;
constexpr unsigned MAX_LINES = 20;
constexpr int      CHAR_W_PX = 8;    // ≈ Cantarell 10pt advance width
constexpr int      LINE_H_PX = 20;   // ≈ Cantarell 10pt line height
constexpr int      CHROME_W  = 80;   // icon column + margins + scrollbar gutter
constexpr int      CHROME_H  = 140;  // heading row + button row + margins

unsigned countWrappedLines(const string& s) noexcept {
	if (s.empty()) return 1;
	unsigned lines = 0, col = 0;
	for (char c : s) {
		if (c == '\n') {
			++lines;
			col = 0;
		}
		else
			if (++col >= WRAP_COLS) {
				++lines;
				col = 0;
		}
	}
	if (col > 0 or s.empty()) ++lines;
	return lines;
}

}

Gtk::Dialog*   Message::dialog   = nullptr;
Gtk::Image*    Message::icon     = nullptr;
Gtk::Label*    Message::primary  = nullptr;
Gtk::TextView* Message::body     = nullptr;
Gtk::Button*   Message::btnNo    = nullptr;
Gtk::Button*   Message::btnYes   = nullptr;
Gtk::Button*   Message::btnClose = nullptr;
Gtk::Window*   Message::main     = nullptr;

namespace {
	bool   batching = false;
	string batchBuffer;
}

void Message::initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* main) {
	builder->get_widget("DialogMessage",       dialog);
	builder->get_widget("ImageMessageIcon",    icon);
	builder->get_widget("LabelMessagePrimary", primary);
	builder->get_widget("TextViewMessageBody", body);
	builder->get_widget("ButtonMessageNo",     btnNo);
	builder->get_widget("ButtonMessageYes",    btnYes);
	builder->get_widget("ButtonMessageClose",  btnClose);
	Message::main = main;
}

void Message::displayError(Gtk::Window* transient, const string& heading) {
	handleDialog(error, Kind::Error, transient, heading);
}

void Message::displayError(const string& errorMessage, Gtk::Window* transient, const string& heading) {
	handleDialog(errorMessage, Kind::Error, transient, heading);
}

void Message::displayInfo(const string& infoMessage, Gtk::Window* transient, const string& heading) {
	handleDialog(infoMessage, Kind::Info, transient, heading);
}

Gtk::ResponseType Message::ask(const string& question, Gtk::Window* transient, const string& heading) {
	return handleDialog(question, Kind::Question, transient, heading);
}

string Message::takeMessage() {
	return std::move(error);
}

void Message::beginBatch() noexcept {
	batching = true;
	batchBuffer.clear();
}

void Message::collect(const string& line) noexcept {
	if (not batching) return;
	batchBuffer += line + '\n';
}

namespace {

// Drains the batch buffer into a sorted, deduplicated report. Returns the
// formatted text and the deduped line count so callers can present both
// without recounting.
struct BatchReport { string text; size_t count; };

BatchReport drainBatch() noexcept {
	batching = false;
	if (batchBuffer.empty())
		return {emptyString, 0};

	// Split into lines, sort so entries from the same source cluster, then
	// collapse duplicates into "<line> (×N)" so a single bad source does
	// not flood the report.
	auto lines {Defaults::explode(batchBuffer, '\n')};
	batchBuffer.clear();
	std::sort(lines.begin(), lines.end());

	BatchReport r;
	for (size_t i = 0; i < lines.size(); ) {
		if (lines[i].empty()) { ++i; continue; }
		size_t j {i + 1};
		while (j < lines.size() and lines[j] == lines[i])
			++j;
		r.text += lines[i];
		if (j - i > 1)
			r.text += " (\xc3\x97" + std::to_string(j - i) + ")";
		r.text += '\n';
		++r.count;
		i = j;
	}
	return r;
}

}

string Message::endBatch() noexcept {
	return drainBatch().text;
}

void Message::finishBatch(const string& action) noexcept {
	const auto report {drainBatch()};
	if (report.count == 0) {
		StatusBar::getInstance().push(action, StatusBar::Severity::Success);
		return;
	}
	const string summary {
		action + " with " + std::to_string(report.count) +
		(report.count == 1 ? " issue" : " issues")
	};
	displayError(report.text, nullptr, summary);
	StatusBar::getInstance().push(summary, StatusBar::Severity::Warning);
}

bool Message::isBatching() noexcept {
	return batching;
}

Gtk::ResponseType Message::handleDialog(const string& message, Kind kind, Gtk::Window* transient, const string& heading) {
	const char*    iconName;
	Glib::ustring  title;
	const bool     isQuestion {kind == Kind::Question};
	switch (kind) {
	case Kind::Info:
		iconName = "dialog-information";
		title    = "Information";
		break;
	case Kind::Error:
		iconName = "dialog-error";
		title    = "Error";
		break;
	case Kind::Question:
		iconName = "dialog-question";
		title    = "Question";
		break;
	}
	icon->set_from_icon_name(iconName, Gtk::ICON_SIZE_DIALOG);
	primary->set_markup(
		"<b>" + Glib::Markup::escape_text(heading.empty() ? title : Glib::ustring{heading}) + "</b>"
	);
	body->get_buffer()->set_text(message);
	btnNo->set_visible(isQuestion);
	btnYes->set_visible(isQuestion);
	btnClose->set_visible(not isQuestion);
	dialog->set_default_response(isQuestion ? Gtk::RESPONSE_NO : Gtk::RESPONSE_CLOSE);
	dialog->set_title(title);
	dialog->set_transient_for(transient ? (transient->is_visible() ? *transient : *main) : *main);
	// Manual sizing: GtkTextView + GtkScrolledWindow do not cooperate well
	// with auto height-for-width, so compute a target size from a logical
	// wrap column and the resulting line count, clamped to a sane range.
	const unsigned shownLines {std::clamp(countWrappedLines(message), MIN_LINES, MAX_LINES)};
	dialog->resize(
		static_cast<int>(WRAP_COLS)  * CHAR_W_PX + CHROME_W,
		static_cast<int>(shownLines) * LINE_H_PX + CHROME_H
	);
	const auto r {static_cast<Gtk::ResponseType>(dialog->run())};
	dialog->hide();
	return r;
}
