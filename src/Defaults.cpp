/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.cpp
 * @since     May 6, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

const unordered_map<string, Defaults::DeviceInfo> Defaults::devicesInfo = {
	{"UltimarcUltimate", {
		"Ultimarc Ipac Ultimate IO",
		4,                    // Max Interfaces
		false,                // Monochrome
		false,                // Variable number of pins
		true,                 // Layout RGB
		false,                // Supports RGB strip
		96,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"UltimarcPacLed64", {
		"Ultimarc PacLed 64",
		4,                    // Max Interfaces
		false,                // Monochrome
		false,                // Variable number of pins
		true,                 // Layout RGB
		false,                // Supports RGB strip
		64,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"UltimarcPacDrive", {
		"Ultimarc Pac Drive",
		4,                    // Max Interfaces
		true,                 // Monochrome
		false,                // Variable number of pins
		false,                // Layout RGB
		false,                // Supports RGB strip
		16,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"UltimarcNanoLed", {
		"Ultimarc NanoLed",
		4,                    // Max Interfaces
		false,                // Monochrome
		true,                 // Variable number of pins
		true,                 // Layout RGB
		true,                 // Supports RGB strip
		60,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"LedWiz32", {
		"Groovy Game Gear Led-Wiz 32",
		16,                   // Max Interfaces
		false,                // Monochrome
		false,                // Variable number of pins
		false,                // Layout RGB
		false,                // Supports RGB strip
		32,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"Howler", {
		"Wolfware Howler",
		4,                    // Max Interfaces
		false,                // Monochrome
		false,                // Variable number of pins
		true,                 // Layout RGB
		false,                // Supports RGB strip
		96,                   // Maximum number of Pins
		Connection::USB,      // Connection type
		""                    // Description
	}},
	{"Adalight", {
		"Adalight Compatible",
		127,                  // Max Interfaces
		false,                // Monochrome
		true,                 // Variable number of pins
		true,                 // Layout RGB
		true,                 // Supports RGB strip
		1000,                 // Maximum number of Pins
		Connection::SERIAL,   // Connection type
		""                    // Description
	}},
	{"RaspberryPi", {
		"Raspberry Pi GPIO",
		1,                    // Max Interfaces
		false,                // Monochrome
		false,                // Variable number of pins
		false,                // Layout RGB
		false,                // Supports RGB strip
		28,                   // Maximum number of Pins
		Connection::NONE,     // Connection type
		""                    // Description
	}},
};

const unordered_map<string, Defaults::RestrictorInfo> Defaults::restrictorsInfo = {
	{"UltraStik360", {
			"Ultimarc UltraStik360", // Name
			4,                       // Maximum Ids
			Connection::USB,         // Connection type
			1,                       // Interfaces
			{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse}, // Ways
			"Logical restrictor, supports multiple digital modes; allows mechanical attachments like ServoStik" // Description (clarified)
	}},
	{"ServoStik", {
			"Ultimarc ServoStik", // Name
			4,                    // Maximum Ids
			Connection::USB,      // Connection type
			2,                    // Interfaces
			{Ways::w4, Ways::w8}, // Ways
			"Mechanical restrictor, switches between 4-way and 8-way control" // Description (clarified)
	}},
	{"GPWiz40RotoX", {
			"Groovy Game Gear GPWiz40 RotoX", // Name
			4,                                // Maximum Ids
			Connection::USB,                  // Connection type
			2,                                // Interfaces
			{Ways::rotary8, Ways::rotary12},  // Ways
			"Mechanical rotator, supports two independent rotary switches (8-way and 12-way)" // Description (clarified)
	}},
	{"GPWiz49", {
			"Groovy Game Gear GPWiz49", // Name
			4,                          // Maximum Ids
			Connection::USB,            // Connection type
			1,                          // Interfaces
			{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse}, // Ways
			"Logical restrictor, supports multiple digital and analog joystick modes" // Description (clarified)
	}},
	{"TOS428", {
			"TOS GRS Gate Restrictor", // Name
			127,                       // Maximum Ids
			Connection::SERIAL,        // Connection type
			4,                         // Interfaces
			{Ways::w4, Ways::w8},      // Ways
			"Mechanical restrictor, supports up to four independent gate mechanisms" // Description (clarified)
	}}
};

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

const StringVector Defaults::elementTypes{
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
	else
		return (restrictorsInfo.at(name).connection == Connection::SERIAL);
}

bool Defaults::isMulti(const string& name) {
	return (restrictorsInfo.at(name).interfaces > 1);
}

string Defaults::createHardwareUniqueId(const StringUMap& data, bool isDevice) {
	string name(data.at(NAME));
	if (Defaults::isIdUser(name, isDevice)) {
		return (name + FIELD_SEPARATOR + data.at(ID));
	}
	if (Defaults::isSerial(name, isDevice)) {
		return (name + FIELD_SEPARATOR + data.at(PORT));
	}
	return name;
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
	if (not rgba.set("#" + color)) {
		return 0.0;
	}
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
	string chunk;

	// If delimiter is not found in string, return whole string as single element
	if (text.find(delimiter) == string::npos) {
		trim(chunk);
		result.push_back(text);
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
	auto children = store->children();
	std::vector<Gtk::TreeIter> rowsToRemove;
	for (auto iter = children.begin(); iter != children.end(); ++iter)
		rowsToRemove.push_back(*iter);
	for (const auto& iter : rowsToRemove)
		store->erase(*iter);

	if (not emptyLabel.empty()) {
		auto row = *(store->append());
		row.set_value(0, string());
		row.set_value(1, string(emptyLabel));
		row.set_value(2, false);
	}

	for (size_t c = 0; c < max; ++c) {
		auto row = *(store->append());
		const auto& id(std::to_string(c + 1));
		row.set_value(0, id);
		row.set_value(1, label + id);
		row.set_value(2, not isUsedFn(id));
	}
}

void Defaults::setFilter(Gtk::SearchEntry* filterEntry, Gtk::FlowBox* box) {
	filterEntry->signal_show().connect([filterEntry]() {
		filterEntry->set_text("");
	});
	filterEntry->signal_changed().connect([filterEntry, box]() {
		const auto filterText(filterEntry->get_text().lowercase());
		for (auto child : box->get_children()) {
			auto c = dynamic_cast<Gtk::FlowBoxChild*>(child);
			auto b = dynamic_cast<Gtk::Button*>(c->get_child());
			auto t = b->get_label().lowercase();
			if (t.find(filterText) != string::npos)
				child->show();
			else
				child->hide();
		}
	});
}

void Defaults::setIgnoreChanges(bool state) {
	ignoreChanges = state;
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

string Defaults::capToDirectory(const string& fullFileName, const string& baseDir) {
	string
		filename(Glib::path_get_basename(fullFileName)),
		dir(Glib::path_get_dirname(fullFileName));

	if (filename.find('.') != string::npos) {
		filename = filename.substr(0, filename.find_last_of('.'));
	}

	if (dir == baseDir or baseDir.size() > dir.size()) {
		return filename;
	}

	auto pos = dir.find(baseDir);
	if (pos == string::npos) {
		return filename;
	}

	dir = dir.substr(baseDir.length() + 1);
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

string Defaults::sanitizeFilename(const string& text) {
	string result;
	result.reserve(text.size());
	for (auto ch : text) {
		switch (ch) {
			case '/':
			case '\\':
			case ':':
			case '*':
			case '?':
			case '"':
			case '<':
			case '>':
			case '|':
				continue;
			default:
				result += ch;
		}
	}
	return result;
}

string& Defaults::getProjectsDir() {
	return Defaults::projectDir;
}

void Defaults::setProjectsDir(const string& dir) {
	projectDir = dir;
}

Defaults::Mode Defaults::getMode() {
	return currentMode;
}

void Defaults::setMode(Mode mode) {
	currentMode = mode;
}
