/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.cpp
 * @since     May 6, 2023
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

using namespace LEDSpicerUI;

const std::unordered_map<string, Defaults::DeviceInfo> Defaults::devicesInfo = {
	{"UltimarcUltimate", {
		"Ultimarc Ipac Ultimate IO",
		// Description
		"USB RGB controller with 96 pins and 256 intensity levels. Supports up to 4 boards.",
		4,              // Max Interfaces
		false,          // Monochrome
		false,          // Variable number of pins
		true,           // Layout RGB
		false,          // Supports RGB strip
		96,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"UltimarcPacLed64", {
		"Ultimarc PacLed 64",
		// Description
		"USB RGB controller with 64 pins and 256 intensity levels. Supports up to 4 boards.",
		4,              // Max Interfaces
		false,          // Monochrome
		false,          // Variable number of pins
		true,           // Layout RGB
		false,          // Supports RGB strip
		64,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"UltimarcPacDrive", {
		"Ultimarc Pac Drive",
		// Description
		"USB monochrome controller with 16 pins (On/Off only). Supports up to 4 boards.",
		4,              // Max Interfaces
		true,           // Monochrome
		false,          // Variable number of pins
		false,          // Layout RGB
		false,          // Supports RGB strip
		16,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"UltimarcNanoLed", {
		"Ultimarc NanoLed",
		// Description
		"USB RGB strip controller with up to 60 variable pins.",
		4,              // Max Interfaces
		false,          // Monochrome
		true,           // Variable number of pins
		true,           // Layout RGB
		true,           // Supports RGB strip
		60,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"LedWiz32", {
		"Groovy Game Gear Led-Wiz 32",
		// Description
		"USB controller with 32 pins and 49 PWM intensity levels. Supports up to 16 boards.",
		16,             // Max Interfaces
		false,          // Monochrome
		false,          // Variable number of pins
		false,          // Layout RGB
		false,          // Supports RGB strip
		32,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"Howler", {
		"Wolfware Howler",
		// Description
		"USB RGB controller with 96 pins and 256 intensity levels.",
		4,              // Max Interfaces
		false,          // Monochrome
		false,          // Variable number of pins
		true,           // Layout RGB
		false,          // Supports RGB strip
		96,             // Maximum number of Pins
		Connection::USB // Connection type
	}},
	{"Adalight", {
		"Adalight Compatible",
		// Description
		"Serial RGB LED strip controller. Variable pin count, up to 128 boards.",
		127,               // Max Interfaces
		false,             // Monochrome
		true,              // Variable number of pins
		true,              // Layout RGB
		true,              // Supports RGB strip
		3000,              // Maximum number of Pins
		Connection::SERIAL // Connection type
	}},
	{"RaspberryPi", {
		"Raspberry Pi GPIO",
		// Description
		"Direct GPIO control for Raspberry Pi. 28 pins via software PWM.",
		1,               // Max Interfaces
		false,           // Monochrome
		false,           // Variable number of pins
		false,           // Layout RGB
		false,           // Supports RGB strip
		28,              // Maximum number of Pins
		Connection::NONE // Connection type
	}},
};

const std::unordered_map<string, Defaults::RestrictorInfo> Defaults::restrictorsInfo = {
	{"UltraStik360", {
		"Ultimarc UltraStik360", // Name
		"Logical restrictor, supports multiple digital modes",
		4,                       // Maximum Ids
		Connection::USB,         // Connection type
		1,                       // Interfaces
		// Ways
		{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse},
	}},
	{"ServoStik", {
		"Ultimarc ServoStik", // Name
		"Mechanical restrictor, switches between 4-way and 8-way control",
		4,                    // Maximum Ids
		Connection::USB,      // Connection type
		2,                    // Interfaces
		{Ways::w4, Ways::w8}, // Ways
	}},
	{"GPWiz40RotoX", {
		"Groovy Game Gear GPWiz40 RotoX", // Name
		"Mechanical rotator, supports two independent rotary switches (8-way and 12-way)",
		4,                                // Maximum Ids
		Connection::USB,                  // Connection type
		2,                                // Interfaces
		{Ways::rotary8, Ways::rotary12}   // Ways
	}},
	{"GPWiz49", {
		"Groovy Game Gear GPWiz49", // Name
		"Logical restrictor, supports multiple digital and analog joystick modes",
		4,                          // Maximum Ids
		Connection::USB,            // Connection type
		1,                          // Interfaces
		// Ways
		{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse}
	}},
	{"TOS428", {
		"TOS GRS Gate Restrictor", // Name
		"Mechanical restrictor, supports up to four independent gate mechanisms",
		127,                       // Maximum Ids
		Connection::SERIAL,        // Connection type
		4,                         // Interfaces
		{Ways::w4, Ways::w8}       // Ways
	}}
};

const std::unordered_map<string, Defaults::InputInfo> Defaults::inputInfo = {
	{"Actions", {
		"Input Actions",
		"Creates interactive guided events.",
		INPUT_NEEDS_SOURCE | INPUT_DEV_LISTENER | INPUT_LINKED_MAPS | INPUT_HAS_SPEED | INPUT_HAS_BLINK
	}},
	{"Blinker", {
		"Input Blinker",
		"Blinks an element or group.",
		INPUT_NEEDS_SOURCE | INPUT_DEV_LISTENER | INPUT_HAS_SPEED | INPUT_HAS_TIMES
	}},
	{"Credits", {
		"Player's Credits",
		"Manages coin and start button interactions.",
		INPUT_NEEDS_SOURCE | INPUT_DEV_LISTENER | INPUT_LINKED_MAPS | INPUT_HAS_CREDITS
	}},
	{"Impulse", {
		"Keyboard Impulse",
		"Lights an element or group.",
		INPUT_NEEDS_SOURCE | INPUT_DEV_LISTENER
	}},
	{"Mame", {
		"MAME Output",
		"Listens for output from MAME on port 8000.",
		0
	}},
	{"Network", {
		"Network Listener",
		"Receives named trigger strings over a network socket.",
		0
	}},
};

const std::unordered_map<string, Defaults::AnimationInfo> Defaults::animationsInfo = {
	{"Filler", {
		"Filler",
		"Fills the group with a color(s)",
		ANIM_USES_FRAME | ANIM_USES_DIRECTION | ANIM_HAS_COLOR | ANIM_HAS_COLORS
	}},
	{"Gradient", {
		"Gradient",
		"Paints a smooth gradient across the group.",
		ANIM_USES_FRAME | ANIM_USES_DIRECTION | ANIM_HAS_COLORS
	}},
	{"Pulse", {
		"Pulse",
		"Pulses the group on and off.",
		ANIM_USES_FRAME | ANIM_USES_DIRECTION | ANIM_HAS_COLOR | ANIM_HAS_COLORS
	}},
	{"Serpentine", {
		"Serpentine",
		"Moves a lit element across the group like a snake.",
		ANIM_USES_FRAME | ANIM_USES_DIRECTION | ANIM_HAS_COLOR
	}},
	{"Random", {
		"Random",
		"Randomly lights elements of the group.",
		ANIM_USES_FRAME | ANIM_HAS_COLORS
	}},
	{"FileReader", {
		"File Reader",
		"Plays back a pre-rendered animation from a file.",
		ANIM_USES_FRAME | ANIM_USES_DIRECTION
	}},
	{"AlsaAudio", {
		"ALSA Audio",
		"Drives the group from an ALSA audio capture device.",
		ANIM_USES_AUDIO
	}},
	{"PulseAudio", {
		"PulseAudio",
		"Drives the group from a PulseAudio capture stream.",
		ANIM_USES_AUDIO
	}},
};

const std::unordered_map<string, Defaults::TransitionInfo> Defaults::transitionsInfo = {
	{"FadeOutIn", {
		"Fade Out / In",
		"Fades the current profile out to a color, then fades the new profile in.",
		TRANS_HAS_SPEED | TRANS_HAS_COLOR
	}},
	{"Crossfade", {
		"Crossfade",
		"Smoothly blends the current profile into the new one.",
		TRANS_HAS_SPEED
	}},
	{"Curtain", {
		"Curtain",
		"A curtain closes over the current profile then opens to reveal the new one.",
		TRANS_HAS_SPEED | TRANS_HAS_COLOR
	}},
};

const std::unordered_map<string, Defaults::Ways> Defaults::wayIds {
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

const StringVector Defaults::elementTypes{
	"",
	"button",
	"joystick",
	"trackball",
	"spinner",
	"credit",
	"light",
	"bar",
	"knocker",
	"misc"
};

const vector<Defaults::ColorInfo> Defaults::legalColors {
	// Grays — brightness scale from full on to full off.
	{"White",          "Full brightness, maximum intensity.",              "FFFFFF"},
	{"LightGray",      "Soft gray, noticeably dimmer than white.",         "C0C0C0"},
	{"Gray",           "Even 50% midpoint gray.",                          "808080"},
	{"DimGray",        "Low-intensity gray, subtle ambient.",              "696969"},
	{"AlmostBlack",    "Barely on — useful when full off is unavailable.", "1A1A1A"},
	{"Black",          "Full off, zero output.",                           "000000"},

	// Reds — scale plus warm-hue variants.
	{"Red",            "Pure unmistakable red, traffic-light red.",        "FF0000"},
	{"LightRed",       "Soft pastel red, clearly lighter than Red.",       "FF8080"},
	{"DarkRed",        "Deep red, roughly half brightness.",               "8B0000"},
	{"VeryDarkRed",    "Near-off red, dim ambient.",                       "2D0000"},
	{"Tomato",         "Warm red with an orange tint, ripe-tomato hue.",   "FF6347"},
	{"Salmon",         "Pale pinkish-orange red, soft warm tone.",         "FA8072"},

	// Greens — scale plus hue variants.
	{"Green",          "Pure saturated green, classic arcade green.",      "00FF00"},
	{"LightGreen",     "Soft pastel green, clearly lighter than Green.",   "90EE90"},
	{"DarkGreen",      "Deep forest green.",                               "006400"},
	{"VeryDarkGreen",  "Near-off green, dim ambient.",                     "003000"},
	{"Lime",           "Bright yellow-green, citrus lime.",                "BFFF00"},
	{"Olive",          "Muted desaturated yellow-green.",                  "808000"},

	// Blues — scale plus purple-hue variants.
	{"Blue",           "Pure saturated blue.",                             "0000FF"},
	{"LightBlue",      "Soft sky blue, clearly lighter than Blue.",        "ADD8E6"},
	{"DarkBlue",       "Deep navy blue.",                                  "00008B"},
	{"VeryDarkBlue",   "Near-off blue, dim ambient.",                      "00002D"},
	{"Violet",         "Blue-purple, between blue and purple.",            "8B00FF"},
	{"Purple",         "Classic red-purple.",                              "800080"},

	// Yellows — scale plus warm variants.
	{"Yellow",         "Pure saturated yellow.",                           "FFFF00"},
	{"LightYellow",    "Very pale yellow, near-white.",                    "FFFFE0"},
	{"DarkYellow",     "Muted mid-brightness yellow.",                     "9B9B00"},
	{"Gold",           "Rich warm yellow with a golden glow.",             "FFD700"},
	{"VeryDarkYellow", "Near-off yellow, dim amber.",                      "2D2D00"},
	{"YellowGreen",    "Clean bridge between yellow and green families.",  "9ACD32"},

	// Magentas — scale plus pink variants.
	{"Magenta",        "Pure saturated magenta, fuchsia.",                 "FF00FF"},
	{"LightMagenta",   "Soft pastel magenta.",                             "FF99FF"},
	{"DarkMagenta",    "Deep saturated magenta.",                          "8B008B"},
	{"VeryDarkMagenta","Near-off magenta, dim ambient.",                   "2D002D"},
	{"Pink",           "Classic warm pink, rose-like.",                    "FFC0CB"},
	{"HotPink",        "Vivid intense pink, clearly warmer than magenta.", "FF69B4"},

	// Cyans — scale plus cyan-green variants.
	{"Cyan",           "Pure saturated cyan, aqua.",                       "00FFFF"},
	{"LightCyan",      "Very pale cyan, near-white aqua.",                 "E0FFFF"},
	{"DarkCyan",       "Deep teal-cyan.",                                  "008B8B"},
	{"VeryDarkCyan",   "Near-off cyan, dim ambient.",                      "002D2D"},
	{"Teal",           "Cyan-green bridge, deep and rich.",                "008080"},
	{"Turquoise",      "Bright cyan-green, tropical water hue.",           "40E0D0"},

	// Oranges and browns — warm low-hue variants.
	{"Orange",         "Vivid warm orange.",                               "FFA500"},
	{"LightOrange",    "Soft pale orange, peachy tone.",                   "FFB347"},
	{"Brown",          "Dark earthy orange-brown.",                        "964B00"},
	{"DarkOrange",     "Deep burnt orange, darker than Brown.",            "8B4513"},
	{"Chocolate",      "Rich warm dark brown with a reddish tone.",        "D2691E"},
	{"Coral",          "Warm red-orange, bridge between red and orange.",  "FF7F50"},

	// Miscellaneous — borderline hues that span two families.
	{"SlateBlue",      "Gray-blue with a purple undertone.",               "6A5ACD"},
	{"Maroon",         "Very dark wine-red, sits between red and purple.", "800000"},
};

bool Defaults::isLegalColor(const string& name) noexcept {
	for (const auto& c : legalColors)
		if (c.name == name) return true;
	return false;
}

// This can be wrong if a weird device came in, but at this moment only USB is ID user.
bool Defaults::isIdUser(const string& name, bool isDevice) {
	if (isDevice)
		return (devicesInfo.at(name).connection == Connection::USB);
	else
		return (restrictorsInfo.at(name).connection == Connection::USB);
}

bool Defaults::isMonochrome(const string& name) {
	return (devicesInfo.at(name).monochrome);
}

bool Defaults::isVariable(const string& name) {
	return (devicesInfo.at(name).variable);
}

bool Defaults::isSerial(const string& name, bool isDevice) {
	if (isDevice)
		return (devicesInfo.at(name).connection == Connection::SERIAL);

	return (restrictorsInfo.at(name).connection == Connection::SERIAL);
}

bool Defaults::isMulti(const string& name) {
	return (restrictorsInfo.at(name).interfaces > 1);
}

string Defaults::createHardwareUniqueId(const Values& data, bool isDevice) {
	const string& name {data.getValue(NAME)};
	if (Defaults::isIdUser(name, isDevice)) {
		return (name + FIELD_SEPARATOR + data.getValue(ID, "1"));
	}
	if (Defaults::isSerial(name, isDevice)) {
		return (name + FIELD_SEPARATOR + data.getValue(PORT));
	}
	return name;
}

bool Defaults::inputHasFlag(const string& input, uint8_t flag) {
	auto it = inputInfo.find(input);
	return it != inputInfo.end() and (it->second.flags & flag);
}

bool Defaults::needSource(const string& input) {
	return inputHasFlag(input, INPUT_NEEDS_SOURCE);
}

bool Defaults::isDevInputListener(const string& input) {
	return inputHasFlag(input, INPUT_DEV_LISTENER);
}

bool Defaults::hasLinkedMaps(const string& input) {
	return inputHasFlag(input, INPUT_LINKED_MAPS);
}

bool Defaults::transitionHasFlag(const string& transition, uint8_t flag) {
	auto it = transitionsInfo.find(transition);
	return it != transitionsInfo.end() and (it->second.flags & flag);
}

string Defaults::createCommonUniqueId(const StringVector& fieldsData) {
	return implode(fieldsData, FIELD_SEPARATOR);
}

bool Defaults::isNumber(const string& number) {
	if (number.empty()) return false;
	for (char const &ch : number) {
		if (not std::isdigit(ch))
			return false;
	}
	return true;
}

bool Defaults::isBetween(const string& number, int low, int high) {
	if (number.empty())
		return false;
	if (not isNumber(number))
		return false;
	int n = std::stoi(number);
	if (high == -1)
		return (n >= low);
	return (n >= low and n <= high);
}

string Defaults::addUnitSeparator(const string& unit) {
	return UNIT_SEPARATOR + unit + UNIT_SEPARATOR;
}

double Defaults::getLuminance(const string& color) {
	Gdk::RGBA rgba;
	if (not rgba.set("#" + color)) return 0.0;

	double
		r = rgba.get_red(),
		g = rgba.get_green(),
		b = rgba.get_blue();

	return 0.299 * r + 0.587 * g + 0.114 * b;
}

StringVector Defaults::explode(const string& text, const char delimiter, const size_t limit) {

	StringVector result;
	if (text.empty()) {
		return result;
	}

	stringstream ss(text);
	string chunk {text};

	// If delimiter is not found in string, return whole string as single element
	if (text.find(delimiter) == string::npos) {
		trim(chunk);
		result.push_back(chunk);
		return result;
	}

	size_t count = 0;
	while (std::getline(ss, chunk, delimiter)) {
		// If limit is 0 (no limit) or we haven't reached the limit yet
		if (not limit or count < limit - 1) {
			trim(chunk);
			result.push_back(chunk);
			count++;
		}
		else {
			// For the last chunk when limit is reached, take the rest of the string
			string remaining;
			std::getline(ss, remaining);
			chunk = chunk + (remaining.empty() ? "" : delimiter + remaining);
			trim(chunk);
			result.push_back(chunk);
			break;
		}
	}

	return result;
}

string Defaults::implode(const StringVector& values, const char& delimiter) {
	string r;
	if (values.empty())
		return r;
	for (const string& s : values) {
		r += s + delimiter;
	}
	r.resize(r.size() - 1);
	return r;
}

string Defaults::implode(const StringVector& values, const string& delimiter) {
	string r;
	if (values.empty())
		return r;
	for (const string& s : values) {
		r += s + delimiter;
	}
	r.resize(r.size() - delimiter.size());
	return r;
}

string Defaults::implode(const StringUSet& values, const char& delimiter) {
	string r;
	if (values.empty())
		return r;
	for (const string& s : values) {
		r += s + delimiter;
	}
	r.resize(r.size() - 1);
	return r;
}

void Defaults::ltrim(string& text) {
	if (text.empty()) return;
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
	text.erase(
		std::find_if(text.rbegin(), text.rend(),
		[](unsigned char c) {
			return c > 32;
		}).base(),
		text.end()
	);
}

void Defaults::trim(string& text) {
	ltrim(text);
	rtrim(text);
}

string Defaults::detectElementType(const Glib::ustring& name) {
	for (size_t c = 1; c < Defaults::elementTypes.size(); ++c)
		if (name.lowercase().find(Defaults::elementTypes[c]) != name.npos)
			return std::to_string(c);
	return DEFAULT_ELEMENT_TYPE;
}

void Defaults::initialize(Gtk::HeaderBar* header, Gtk::Button* btnSave) {
	Defaults::header  = header;
	Defaults::btnSave = btnSave;
	btnSave->set_sensitive(false);
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
	if (ignoreChanges) return;
	dirty = true;
	auto t(header->get_title());
	if (t.size() and t[0] == '*') return;
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

void Defaults::setSubtitle(const string& text) {
	header->set_subtitle(text);
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

void Defaults::populateComboBoxTextWithNumbers(Gtk::ComboBoxText* comboBox, int from, int to, const StringVector& ignoreList) {
	comboBox->remove_all();
	for (int c = from; c <= to; c++) {
		string v(std::to_string(c));
		if (std::find(ignoreList.begin(), ignoreList.end(), v) == ignoreList.end()) {
			comboBox->append(v);
		}
	}
}

void Defaults::populateComboBoxWithIds(
	Gtk::ListStore* store,
	uint max,
	std::function<bool(const string&)> isUsedFn,
	const string& emptyLabel,
	const string& label
) {
	// clean id combobox.
	auto children {store->children()};
	std::vector<Gtk::TreeIter> rowsToRemove;
	for (auto iter = children.begin(); iter != children.end(); ++iter)
		rowsToRemove.push_back(*iter);
	for (const auto& iter : rowsToRemove)
		store->erase(*iter);

	if (not emptyLabel.empty()) {
		auto row {*(store->append())};
		row.set_value(0, emptyString);
		row.set_value(1, emptyLabel);
		row.set_value(2, false);
	}

	for (size_t c = 0; c < max; ++c) {
		auto row {*(store->append())};
		const auto& id {std::to_string(c + 1)};
		row.set_value(0, id);
		row.set_value(1, label + id);
		row.set_value(2, not isUsedFn(id));
	}
}

void Defaults::selectFirstAvailableId(
	Gtk::ComboBox* combo,
	uint max,
	std::function<bool(const string&)> isUsedFn
) noexcept {
	for (uint c = 0; c < max; ++c) {
		const string id {std::to_string(c + 1)};
		if (not isUsedFn(id)) {
			combo->set_active_id(std::move(id));
			break;
		}
	}
}

void Defaults::setFilter(Gtk::SearchEntry* filterEntry, Gtk::FlowBox* box, Gtk::Dialog* dialog) {
	dialog->signal_show().connect([filterEntry]() {
		filterEntry->set_text("");
	});
	filterEntry->signal_changed().connect([filterEntry, box]() {
		const auto filterText(filterEntry->get_text().lowercase());
		for (auto child : box->get_children()) {
			auto c = static_cast<Gtk::FlowBoxChild*>(child);
			auto b = static_cast<Gtk::Button*>(c->get_child());
			auto t = b->get_label().lowercase();
			if (t.find(filterText) != string::npos)
				child->show();
			else
				child->hide();
		}
	});
	filterEntry->signal_stop_search().connect([dialog, filterEntry]() {
		if (filterEntry->get_text().empty()) {
			dialog->response(Gtk::RESPONSE_CANCEL);
		}
		else {
			filterEntry->set_text("");
		}
	});
}

void Defaults::linkSwitchToWidget(Gtk::Switch* sw, Gtk::Widget* widget, bool invert) noexcept {
	sw->property_active().signal_changed().connect([sw, widget, invert]() {
		widget->set_sensitive(sw->get_active() != invert);
	});
	widget->set_sensitive(sw->get_active() != invert);
}

void Defaults::linkToggleToWidget(Gtk::ToggleButton* toggle, Gtk::Widget* widget, bool invert) noexcept {
	toggle->signal_toggled().connect([toggle, widget, invert]() {
		widget->set_sensitive(toggle->get_active() != invert);
	});
	widget->set_sensitive(toggle->get_active() != invert);
}

void Defaults::setIgnoreChanges(bool state) {
	ignoreChanges = state;
}

void Defaults::indexDuplicateLabels(StringMap& items) {
	std::unordered_map<string, uint8_t>
		count,
		index;
	for (const auto& [id, label] : items) ++count[label];
	for (auto& [id, label] : items) {
		if (count[label] > 1)
			label += " #" + std::to_string(++index[label]);
	}
}

string Defaults::extractName(const string& fullFileName, const string& rootPath) {

	// Get fullFileName without extension.
	string
		filename(Glib::path_get_basename(fullFileName)),
		dir(Glib::path_get_dirname(fullFileName));
	if (filename.find('.') != string::npos) {
		filename = filename.substr(0, filename.find_last_of('.'));
	}

	if (dir == rootPath or rootPath.size() > dir.size()) {
		return filename;
	}

	// Check if file is within root.
	auto pos = dir.find(rootPath);
	if (pos == string::npos) {
		return filename;
	}

	// Get the relative path portion.
	dir = dir.substr(rootPath.length() + 1);
	return dir + "/" + filename;
}

string Defaults::extractAfter(const string& line, const string& prefix) {
	auto pos = line.find(prefix);
	if (pos == string::npos)
		return "";
	string result = line.substr(pos + prefix.length());
	trim(result);
	return result;
}

bool Defaults::runCommand(const string& command, string& output) {
	try {
		int exitStatus;
		Glib::spawn_command_line_sync(command, &output, nullptr, &exitStatus);
		return (exitStatus == 0);
	}
	catch (const Glib::Error&) {
		return false;
	}
}

string Defaults::sanitizeFilename(const string& text, size_t maxLen) {
	string result;
	result.reserve(text.size());
	bool leadingDot = true;
	for (unsigned char ch : text) {
		if (ch < 0x20 or ch == 0x7F) continue; // C0 control chars and DEL
		switch (ch) {
			case '/': case '\\': case ':': case '*':
			case '?': case '"': case '<': case '>': case '|':
				continue;
			default: break;
		}
		if (leadingDot and ch == '.') continue;
		leadingDot = false;
		result += static_cast<char>(ch);
	}
	if (result.size() > maxLen)
		result.resize(maxLen);
	return result;
}

string Defaults::sanitizeName(const string& text) noexcept {
	string result;
	result.reserve(text.size());
	for (unsigned char ch : text) {
		if (ch < 0x20 or ch == 0x7F) continue; // C0 control chars and DEL
		switch (ch) {
			case '<': case '>': case '&': case '"':
				continue;
			default: break;
		}
		result += static_cast<char>(ch);
	}
	return result;
}

string Defaults::escapeXmlValue(const string& s) noexcept {
	string r;
	r.reserve(s.size());
	for (char c : s) {
		switch (c) {
			case '&': r += "&amp;";  break;
			case '<': r += "&lt;";   break;
			case '>': r += "&gt;";   break;
			case '"': r += "&quot;"; break;
			default:  r += c;        break;
		}
	}
	return r;
}

void Defaults::attachFilenameFilter(Gtk::Entry* entry, size_t maxLen) noexcept {
	entry->set_max_length(static_cast<int>(maxLen));
	entry->signal_changed().connect([entry, maxLen]() {
		string text(entry->get_text()), filtered(sanitizeFilename(text, maxLen));
		if (filtered != text) {
			entry->set_text(filtered);
			entry->set_position(-1);
		}
	});
}

void Defaults::attachNameFilter(Gtk::Entry* entry, size_t maxLen) noexcept {
	entry->set_max_length(static_cast<int>(maxLen));
	entry->signal_changed().connect([entry]() {
		string text(entry->get_text()), filtered(sanitizeName(text));
		if (filtered != text) {
			entry->set_text(filtered);
			entry->set_position(-1);
		}
	});
}
