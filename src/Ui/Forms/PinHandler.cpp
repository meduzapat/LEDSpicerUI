/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PinHandler.cpp
 * @since     Mar 22, 2023
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

#include "PinHandler.hpp"

using namespace LEDSpicerUI::Ui::Forms;

PinHandler* PinHandler::ph = nullptr;

PinHandler* PinHandler::getInstance() {
	return ph ? ph : throw Message("Pin Handler not initialized.");
}

PinHandler* PinHandler::getInstance(Glib::RefPtr<Gtk::Builder> const &builder) {
	builder->get_widget_derived("FlowBoxPinLayout", ph);
	return getInstance();
}

void PinHandler::setCurrentDevicePins(unordered_map<string, string>* pinClasses) {
	PinHandler::pinClasses = pinClasses;
}

void PinHandler::reset() {
	for (auto& child : get_children()) {
		remove(*child);
	}
	// Fill pins
	for (auto c = 1; c <= pinClasses->size(); ++c) {
		string n(std::to_string(c));
		auto l = Gtk::make_managed<Gtk::Label>(n);
		if (pinClasses->at(n).empty()) {
			l->get_style_context()->remove_class(NO_COLOR);
		}
		else {
			l->get_style_context()->add_class(pinClasses->at(n));
		}
		add(*l);
	}
	show_all();
}

size_t PinHandler::getSize() {
	return pinClasses->size();
}

bool PinHandler::isUsed(const std::string& pin) {
	for (auto& p : *pinClasses)
		if (not p.second.empty() && p.first == pin)
			return true;
	return false;
}

void PinHandler::unmarkPin(const std::string& pin) {
	if (not pin.empty() and pinClasses->count(pin))
		pinClasses->at(pin) = NO_COLOR;
}

void PinHandler::markPin(const std::string& pin, const std::string& color) {
	pinClasses->at(pin) = color;
}
