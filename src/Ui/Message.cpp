/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Error.cpp
 * @since     Feb 12, 2023
 * @author    Patricio A. Rossi (MeduZa)
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

#include "Message.hpp"

using namespace LEDSpicerUI::Ui;

Gtk::MessageDialog* Message::errorDialog    = nullptr;
Gtk::MessageDialog* Message::infoDialog     = nullptr;
Gtk::MessageDialog* Message::questionDialog = nullptr;
Gtk::Window* Message::main                  = nullptr;

Message::Message(const string& errorMessage, Gtk::Window* transient) : error(errorMessage) {
	displayError(errorMessage, transient);
}

void Message::initialize(Glib::RefPtr<Gtk::Builder> const &builder, Gtk::Window* main) {
	builder->get_widget("DialogErrorMessage",    errorDialog);
	builder->get_widget("DialogInfoMessage",     infoDialog);
	builder->get_widget("DialogQuestionMessage", questionDialog);
	Message::main = dynamic_cast<Gtk::Window*>(main);
}

void Message::displayError(Gtk::Window* transient) {
	handleDialog(error, errorDialog, transient);
}

void Message::displayError(const string& errorMessage, Gtk::Window* transient) {
	handleDialog(errorMessage, errorDialog, transient);
}

void Message::displayInfo(const string& infoMessage, Gtk::Window* transient) {
	handleDialog(infoMessage, infoDialog, transient);
}

int Message::ask(const string& question, Gtk::Window* transient) {
	return handleDialog(question, questionDialog, transient);
}

string Message::getMessage() {
	return std::move(error);
}

int Message::handleDialog(const string& message, Gtk::MessageDialog* dialog, Gtk::Window* transient) {
	dialog->set_transient_for(transient ? *transient : *main);
	dialog->set_secondary_text(message);
	int r = dialog->run();
	dialog->hide();
	return r;
}


