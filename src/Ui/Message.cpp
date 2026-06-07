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

Gtk::Dialog*   Message::dialog   = nullptr;
Gtk::Image*    Message::icon     = nullptr;
Gtk::Label*    Message::primary  = nullptr;
Gtk::TextView* Message::body     = nullptr;
Gtk::Button
	* Message::btnNo    = nullptr,
	* Message::btnYes   = nullptr,
	* Message::btnClose = nullptr;
Gtk::Window*   Message::main     = nullptr;

void Message::initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* mainWindow) noexcept {
	builder->get_widget("DialogMessage",       dialog);
	builder->get_widget("ImageMessageIcon",    icon);
	builder->get_widget("LabelMessagePrimary", primary);
	builder->get_widget("TextViewMessageBody", body);
	builder->get_widget("ButtonMessageNo",     btnNo);
	builder->get_widget("ButtonMessageYes",    btnYes);
	builder->get_widget("ButtonMessageClose",  btnClose);
	Message::main = mainWindow;
}

void Message::displayError(Gtk::Window* transient, const string& heading) noexcept {
	handleDialog(error, Kind::Error, transient, heading);
}

void Message::displayError(const string& message, Gtk::Window* transient, const string& heading) noexcept {
	handleDialog(message, Kind::Error, transient, heading);
}

void Message::displayInfo(const string& message, Gtk::Window* transient, const string& heading) noexcept {
	handleDialog(message, Kind::Info, transient, heading);
}

Gtk::ResponseType Message::ask(const string& message, Gtk::Window* transient, const string& heading) noexcept {
	return handleDialog(message, Kind::Question, transient, heading);
}

string Message::takeMessage() noexcept {
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

Message::BatchReport Message::drainBatch() noexcept {
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
			r.text += " (×" + std::to_string(j - i) + ")";
		r.text += '\n';
		++r.count;
		i = j;
	}
	return r;
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

unsigned Message::countWrappedLines(const string& message) noexcept {
	if (message.empty()) return 1;
	unsigned lines = 0, col = 0;
	for (char c : message) {
		if (c == '\n') {
			++lines;
			col = 0;
		}
		else if (++col >= WRAP_COLS) {
			++lines;
			col = 0;
		}
	}
	if (col > 0) ++lines;
	return lines;
}

Gtk::ResponseType Message::handleDialog(const string& message, Kind kind, Gtk::Window* transient, const string& heading) noexcept {
	Glib::ustring iconName;
	Glib::ustring title;
	const bool    isQuestion {kind == Kind::Question};
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
	auto& parent {(transient and transient->is_visible()) ? *transient : *main};
	dialog->set_transient_for(parent);
	const unsigned shownLines {std::clamp(countWrappedLines(message), MIN_LINES, MAX_LINES)};
	dialog->resize(
		static_cast<int>(WRAP_COLS)  * CHAR_W_PX + CHROME_W,
		static_cast<int>(shownLines) * LINE_H_PX + CHROME_H
	);
	const auto response {static_cast<Gtk::ResponseType>(dialog->run())};
	dialog->hide();
	return response;
}
