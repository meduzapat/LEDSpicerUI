/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.cpp
 * @since     May 6, 2023
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

using namespace LEDSpicerUI;

bool Defaults::dirty = false;
string Defaults::tabs;
Gtk::HeaderBar* Defaults::header = nullptr;
Gtk::Button* Defaults::btnSave   = nullptr;

const unordered_map<string, Defaults::DeviceInfo> Defaults::devicesInfo = {
//        ID                        NAME                MI  B/W    ML
	{"UltimarcUltimate", {"Ultimarc Ipac Ultimate IO",   4, false, 96}},
	{"UltimarcPacLed64", {"Ultimarc PacLed 64",          4, false, 64}},
	{"UltimarcPacDrive", {"Ultimarc Pac Drive",          4, true,  16}},
	{"UltimarcNanoLed",  {"Ultimarc NanoLed",            4, false, 60}},
	{"LedWiz32",         {"Groovy Game Gear Led-Wiz 32", 4, false, 32}},
	{"Howler",           {"Wolfware Howler",             4, false, 96}},
	{"Adalight",         {"Adalight (ESP8266)",          1, false, 60}},
	{"RaspberryPi",      {"Raspberry Pi GPIO pins",      1, false, 28}}
};

const unordered_map<string, Defaults::RestrictorInfo> Defaults::restrictorsInfo = {
//        ID                        NAME
	{"UltraStik360", {"Ultimarc UltraStik360",          4, {Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse}}},
	{"ServoStik",    {"Ultimarc ServoStik",             4, {Ways::w4, Ways::w8}}},
	{"GPWiz40RotoX", {"Groovy Game Gear GPWiz40 RotoX", 4, {Ways::rotary8, Ways::rotary12}}},
	{"GPWiz49",      {"Groovy Game Gear GPWiz49",       4, {Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse}}},
};

const vector<Defaults::Ways> Defaults::allWays{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse, Ways::rotary8, Ways::rotary12};

const unordered_map<string, Defaults::Ways> Defaults::wayIds {
	{"w2",       Ways::w2},
	{"w2v",      Ways::w2v},
	{"w4",       Ways::w4},
	{"w4x",      Ways::w4x},
	{"w8",       Ways::w8},
	{"w16",      Ways::w16},
	{"w49",      Ways::w49},
	{"analog",   Ways::analog},
	{"mouse",    Ways::mouse},
	{"rotary8",  Ways::rotary8},
	{"rotary12", Ways::rotary12},
};

const vector<string> Defaults::elementTypes{
	"",
	"button",
	"joystick",
	"trackball",
	"spinner",
	"credit",
	"light",
	"bar",
	"knocker"
	"misc"
};

bool Defaults::isMultiple(const string& name) {
	return (devicesInfo.at(name).maxIds > 1);
}

bool Defaults::isMonocrome(const string& name) {
	return (devicesInfo.at(name).monochrome);
}

bool Defaults::isNumber(const string& number) {
	for (char const &ch : number) {
		if (not std::isdigit(ch))
			return false;
	}
	return true;
}

bool Defaults::isBetween(const string& number, int low, int hight) {
	if (not isNumber(number))
		return false;
	int n = std::stoi(number);
	if (hight == -1)
		return (n >= low);
	return (n >= low and n <= hight);
}

double Defaults::getLiminance(const string& color) {
	Gdk::Color c("#" + color);
	double r = c.get_red_p() * 255;
	double g = c.get_green_p() * 255;
	double b = c.get_blue_p() * 255;
	return (0.299 * r + 0.587 * g + 0.114 * b) / 255;
}

vector<string> Defaults::explode(const string& text, const char delimiter, const size_t limit) {

	vector<string> temp;

	if (text.empty())
		return temp;

	size_t start = 0, end = 0;

	for (size_t c = 0; limit ? c < limit : true; c++) {
		end = text.find(delimiter, start);
		if (end == string::npos) {
			string t(text.substr(start));
			trim(t);
			temp.push_back(t);
			break;
		}
		string t(text.substr(start, end - start));
		trim(t);
		temp.push_back(t);
		start = end + 1;
	}

	return temp;
}

string Defaults::implode(const vector<string>& values, const char delimiter) {
	string r;
	for (const string& s : values) {
		r += s + delimiter;
	}
	r.resize(r.size() -1);
	return r;
}

void Defaults::ltrim(string& text) {
	size_t chars = 0;
	for (size_t c = 0; c < text.size(); c++) {
		if (text[c] > 32)
			break;
		chars ++;
	}
	if (chars)
		text.erase(0, chars);
}

void Defaults::rtrim(string& text) {
	size_t chars = 0;
	for (size_t c = text.size(); c > 0; c--) {
		if (text[c] > 32)
			break;
		chars ++;
	}
	if (chars)
		text.resize(text.size() - chars + 1);
}

void Defaults::trim(string& text) {
	ltrim(text);
	rtrim(text);
}

string Defaults::detectElementType(const Glib::ustring& name) {
	for (int c = 1; c < Defaults::elementTypes.size(); ++c)
		if (name.lowercase().find(Defaults::elementTypes[c]) != name.npos)
			return std::to_string(c);
	return DEFAULT_ELEMENT_TYPE;
}

void Defaults::setSubtitle(const string& text) {
	header->set_subtitle(text);
}

void Defaults::initialize(Gtk::HeaderBar* header, Gtk::Button* btnSave) {
	Defaults::header  = header;
	Defaults::btnSave = btnSave;
}

void Defaults::registerWidget(Gtk::Editable* widget) {
	widget->signal_changed().connect(&Defaults::markDirty);
}

void Defaults::registerWidget(Gtk::ComboBox* widget) {
	widget->signal_changed().connect(&Defaults::markDirty);
}

void Defaults::registerWidget(Gtk::ToggleButton* widget) {
	widget->signal_toggled().connect(&Defaults::markDirty);
}

void Defaults::markDirty() {
	dirty = true;
	auto t(header->get_title());
	if (t.size() and t[0] == '*')
		return;
	header->set_title('*' + t);
	btnSave->set_sensitive(true);
}

bool Defaults::isDirty() {
	return dirty;
}

void Defaults::cleanDirty() {
	dirty = false;
	auto t(header->get_title());
	if (t.size() and t[0] == '*')
		header->set_title(t.substr(1));
	btnSave->set_sensitive(false);
}

void Defaults::increaseTab() {
	tabs += '\t';
}

void Defaults::reduceTab() {
	auto z(tabs.size());
	if (z)
		tabs.resize(z -1);
}

string Defaults::tab() {
	return tabs;
}
