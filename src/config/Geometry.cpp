/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Geometry.cpp
 * @since     Jun 7, 2026
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

#include "Geometry.hpp"

using namespace LEDSpicerUI::Config;

Geometry Geometry::instance;
const string Geometry::MAIN_WINDOW_ID {"MainWindow"};

void Geometry::load() noexcept {

	try {
		XMLHelper doc(XMLHelper::getAppConfigDir() + UI_GEOMETRY_FILE, UI_GEOMETRY_TYPE);
		for (auto el = doc.getRoot()->FirstChildElement("Window"); el; el = el->NextSiblingElement("Window")) {
			const char* id {el->Attribute("id")};
			if (not id) continue;
			int w {-1}, h {-1};
			if (el->QueryIntAttribute("w", &w) != tinyxml2::XML_SUCCESS) continue;
			if (el->QueryIntAttribute("h", &h) != tinyxml2::XML_SUCCESS) continue;
			entries[id] = Entry{nullptr, w, h};
			if (id == MAIN_WINDOW_ID) {
				el->QueryIntAttribute("x", &mainWindowPos.first);
				el->QueryIntAttribute("y", &mainWindowPos.second);
			}
		}
	}
	catch (...) {
		entries.clear();
		mainWindowPos = {-1, -1};
	}
}

void Geometry::registerWindow(Gtk::Window* win, const string& id) noexcept {

	if (not win->get_resizable()) return;

	auto& entry = entries[id];
	entry.win = win;
	if (entry.w > 0 and entry.h > 0)
		win->set_default_size(entry.w, entry.h);

	if (id == MAIN_WINDOW_ID and mainWindowPos.first >= 0 and mainWindowPos.second >= 0)
		win->move(mainWindowPos.first, mainWindowPos.second);
}

void Geometry::terminate() noexcept {

	bool dirty {false};

	for (auto it = entries.begin(); it != entries.end();) {
		auto& [id, entry] = *it;
		// Stale id from file, remove record and mark for in file removal.
		if (not entry.win) {
			it = entries.erase(it);
			dirty = true;
			continue;
		}
		int w {-1}, h {-1};
		entry.win->get_size(w, h);
		if (w != entry.w or h != entry.h) {
			entry.w = w;
			entry.h = h;
			dirty = true;
		}
		if (id == MAIN_WINDOW_ID) {
			int x {-1}, y {-1};
			entry.win->get_position(x, y);
			if (x != mainWindowPos.first or y != mainWindowPos.second) {
				mainWindowPos = {x, y};
				dirty = true;
			}
		}
		++it;
	}

	if (dirty) save();
}

void Geometry::save() noexcept {

	string body;
	for (const auto& [id, entry] : entries) {
		if (entry.w <= 0 or entry.h <= 0) continue;
		body += Defaults::tab() + "<Window id=\"" + id +
			"\" w=\"" + std::to_string(entry.w) +
			"\" h=\"" + std::to_string(entry.h) + "\"";
		if (id == MAIN_WINDOW_ID and mainWindowPos.first >= 0 and mainWindowPos.second >= 0) {
			body += " x=\"" + std::to_string(mainWindowPos.first) +
				"\" y=\"" + std::to_string(mainWindowPos.second) + "\"";
		}
		body += "/>\n";
	}

	if (body.empty()) return;

	string xml {XMLHelper::xmlHeader(UI_GEOMETRY_TYPE)};
	xml += body;
	Defaults::reduceTab();
	xml += XMLHelper::xmlFooter();

	try {
		Glib::file_set_contents(XMLHelper::getAppConfigDir() + UI_GEOMETRY_FILE, xml);
	}
	catch (...) {}
}
