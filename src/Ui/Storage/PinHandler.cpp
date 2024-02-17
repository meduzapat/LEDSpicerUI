/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PinHandler.cpp
 * @since     Mar 22, 2023
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

#include "PinHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;

PinHandler* PinHandler::instance = nullptr;

PinHandler* PinHandler::getInstance() {
	return instance;
}

void PinHandler::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance)
		builder->get_widget_derived("FlowBoxPinLayout", instance);
}

void PinHandler::setCurrentDevicePins(std::map<uint8_t, string>* pinClasses) {
	PinHandler::pinClasses = pinClasses;
}

void PinHandler::reset() {
	for (auto& child : get_children()) {
		remove(*child);
	}
	// Fill pins
	for (auto c = 1; c <= pinClasses->size(); ++c) {
		auto label = Gtk::make_managed<Gtk::Label>(std::to_string(c));
		if (pinClasses->at(c).empty()) {
			label->get_style_context()->remove_class(NO_COLOR);
		}
		else {
			label->get_style_context()->add_class(pinClasses->at(c));
		}
		add(*label);
	}
	show_all();
}

void PinHandler::wipe() {
	for (auto& child : get_children()) {
		remove(*child);
	}
	pinClasses->clear();
}

void PinHandler::resize(uint8_t newSize, bool doWipe) {
	if (doWipe) {
		wipe();
		for (uint8_t c = 1; c <= newSize; ++c) {
			pinClasses->emplace(c, NO_COLOR);
		}
	}
	else {
		// if bigger, add empties.
		if (newSize > pinClasses->size()) {
			for (auto c = pinClasses->size() + 1; c <= newSize; ++c) {
				pinClasses->emplace(c, NO_COLOR);
			}
		}
		// if smaller remove from the back.
		else {
			auto lastToDelete = pinClasses->end();
			auto firstToDelete = std::prev(lastToDelete, pinClasses->size() - newSize);
			pinClasses->erase(firstToDelete, lastToDelete);
		}
	}
	set_max_children_per_line(findDivisor(newSize));
	show_all();
}

size_t PinHandler::getSize() {
	return pinClasses->size();
}

bool PinHandler::exists(const string& pin) {
	return pinClasses->count(std::stoi(pin));
}

bool PinHandler::isUsed(const string& pin) {
	if (exists(pin) and not pinClasses->at(std::stoi(pin)).empty())
		return true;
	return false;
}

void PinHandler::unmarkPin(const string& pin) {
	if (not pin.empty() and exists(pin))
		pinClasses->at(std::stoi(pin)) = NO_COLOR;
}

void PinHandler::markPin(const string& pin, const string& color) {
	if (exists(pin))
		pinClasses->at(std::stoi(pin)) = color;
}

const uint8_t PinHandler::findDivisor(uint8_t size) {
	for (uint8_t c = MAX_COLUMNS; c > MIN_COLUMNS; --c) {
		if (size % c == 0) {
			return c;
		}
	}
	return MAX_COLUMNS;
}
