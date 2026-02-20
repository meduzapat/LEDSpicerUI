/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.hpp
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

#include <gtkmm.h>
#include <glibmm.h>
#include <giomm.h>

#include <unordered_map>
using std::unordered_map;

#include <unordered_set>
using std::unordered_set;
using std::set;

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::stringstream;

#include <functional>

#include <sys/stat.h>

#include <memory>
using std::unique_ptr;

#pragma once

#define XML_FILE_PLAIN ""
#define INPUT_PATH "inputs/"
#define ANIMATION_PATH "animations/"
#define PROFILE_PATH "profiles/"

#define DEFAULT_MESSAGE "This is an auto-generated file by " PACKAGE_STRING "."

using StringUMap       = unordered_map<string, string>;
using StringUMapVector = vector<StringUMap>;
using StringUSet       = unordered_set<string>;
using StringSet        = set<string>;
using StringVector     = vector<string>;

namespace LEDSpicerUI {

namespace Constants {

/// Separators.
constexpr char FIELD_SEPARATOR    = 30;  // Record Separator (RS)
constexpr char RECORD_SEPARATOR   = 31;  // Unit Separator (US)
constexpr char UNIT_SEPARATOR     = 32;  // Space
constexpr char ID_SEPARATOR       = ','; // Comma
constexpr char ID_GROUP_SEPARATOR = '|'; // Pipe

/// LEDSpicer configuration file keys.
constexpr const char* DEFAULT_USERID   = "1000";
constexpr const char* DEFAULT_GROUPID  = "1000";
constexpr const char* DEFAULT_PORT     = "16161";
constexpr const char* DEFAULT_FPS      = "30";
constexpr const char* DEFAULT_COLORS   = "basicColors";
constexpr const char* DEFAULT_LOGLEVEL = "Info";

/// Emitter configuration file keys.
constexpr const char* DEFAULT_COLORSINFO   = "true";
constexpr const char* DEFAULT_CRAFTPROFILE = "true";
constexpr const char* DEFAULT_DATASOURCE   = "file,mame";

/// ProcessLookup configuration file keys.
constexpr const char* PARAM_MILLISECONDS = "runEvery";
constexpr const char* PARAM_PROCESS_NAME = "processName";
constexpr const char* PARAM_PROCESS_POS  = "position";
constexpr const char* PARAM_SYSTEM       = "system";
constexpr const char* DEFAULT_RUNEVERY   = "";

/// Profile configuration keys.
constexpr const char* DEFAULT_PROFILE                  = "default";
constexpr const char* BACKGROUND_COLOR                 = "backgroundColor";
constexpr const char* DEFAULT_PROFILE_BACKGROUND_COLOR = "Off";

/// Hardware configuration keys.
constexpr const char* NAME     = "name";
constexpr const char* ID       = "boardId";
constexpr const char* FILENAME = "filename";
constexpr const char* PORT     = "port";
constexpr const char* PINS     = "leds";
constexpr const char* PATH     = "path";

constexpr float DEFAULT_CHANGE_VALUE    = 64.00f;

/// Element configuration keys.
constexpr const char* PIN           = "led";
constexpr const char* RED_PIN       = "red";
constexpr const char* GREEN_PIN     = "green";
constexpr const char* BLUE_PIN      = "blue";
constexpr const char* SOLENOID      = "solenoid";
constexpr const char* POSITION      = "position";
constexpr const char* POSITIONS     = "positions";
constexpr const char* STRIPSIZE     = "stripSize";
constexpr const char* TIME_ON       = "timeOn";
constexpr const char* COLORFORMAT   = "colorFormat";
constexpr const char* CHANGE_POINT  = "changePoint";
constexpr const char* DEFAULT_COLOR = "defaultColor";
constexpr const char* BRIGHTNESS    = "brightness";

constexpr const char* DEFAULT_BRIGHTNESS = "100";
constexpr unsigned int DEFAULT_SOLENOID  = 50;

/// CSS-related constants
constexpr const char* COLOR_PIN      = "pinSingle";
constexpr const char* COLOR_SOLENOID = "pinSolenoid";
constexpr const char* COLOR_RED      = "pinRed";
constexpr const char* COLOR_GREEN    = "pinGreen";
constexpr const char* COLOR_BLUE     = "pinBlue";
constexpr const char* COLOR_MULTIPLE = "pinMulti";
constexpr const char* PIN_LABEL      = "pinLabel";
constexpr const char* CONNECTOR_BOX  = "connectorBox";
constexpr const char* NO_COLOR       = "";

/// Restrictor configuration keys.
constexpr const char* US360_HAS_RESTRICTOR = "hasRestrictor";
constexpr const char* US360_USE_MOUSE      = "handleMouse";
constexpr const char* GZ49_WILLIAMS        = "williams";
constexpr const char* GZ40_SPEED_ON        = "speedOn";
constexpr const char* GZ40_SPEED_OFF       = "speedOff";
constexpr int GZ40_DEFAULT_SPEED           = 12;  // Integer
constexpr const char* RESTRICTOR_INTERFACE = "id";

/// Input mapping configuration keys.
constexpr const char* LINKED_ITEMS = "linkedTriggers";
constexpr const char* TRIGGER      = "trigger";
constexpr const char* TARGET       = "target";
constexpr const char* TYPE         = "type";
constexpr const char* COLOR        = "color";
constexpr const char* FILTER       = "filter";
constexpr const char* ELEMENT      = "Element";
constexpr const char* GROUP        = "Group";

/// UI-related constants.
constexpr const char* DEFAULT_ELEMENT_TYPE = "9";
constexpr const char* PLAYER               = "player";
constexpr const char* JOYSTICK             = "joystick";

/// Collection names.
constexpr const char* COLLECTION_DEVICES        = "devices";
constexpr const char* COLLECTION_RESTRICTORS    = "restrictors";
constexpr const char* COLLECTION_ELEMENT        = "elements";
constexpr const char* COLLECTION_GROUP          = "groups";
constexpr const char* COLLECTION_PROCESS        = "processes";
constexpr const char* COLLECTION_RESTRICTOR_MAP = "playerCombinations";
constexpr const char* COLLECTION_INPUT          = "inputs";
constexpr const char* COLLECTION_INPUT_EVENTS   = "listenEvents";
constexpr const char* COLLECTION_INPUT_MAPS     = "linkedMaps";
constexpr const char* COLLECTION_ANIMATIONS     = "animations";
constexpr const char* COLLECTION_PROFILES       = "profiles";

/// Types
constexpr const char* TYPE_DEVICE         = "device";
constexpr const char* TYPE_RESTRICTOR     = "restrictor";
constexpr const char* TYPE_ELEMENT        = "element";
constexpr const char* TYPE_GROUP          = "group";
constexpr const char* TYPE_PROCESS        = "process";
constexpr const char* TYPE_RESTRICTOR_MAP = "playerCombination";
constexpr const char* TYPE_INPUT          = "input";
constexpr const char* TYPE_INPUT_EVENT    = "listenEvent";
constexpr const char* TYPE_INPUT_MAP      = "linkedMap";
constexpr const char* TYPE_ANIMATION      = "animation";
constexpr const char* TYPE_PROFILE        = "profile";


} // namespace

using namespace Constants;

/**
 * LEDSpicerUI::Defaults
 * Static class that contains definitions, helper functions, and other generic utilities.
 */
class Defaults {

public:

	/// Taken from Restrictors in ledspicer.
	enum class Ways : uint8_t {invalid, w2, w2v, w4, w4x, w8, w16, w49, analog, mouse, rotary8, rotary12};

	enum class Connection : uint8_t {NONE, USB, SERIAL};

	enum class Mode {
		Local,     /// LEDSpicerd daemon detected correctly but iterations are OFF.
		Iterative, /// LEDSpicerd daemon detected and Iterative mode is enabled.
		Portable   /// LEDSpicerd daemon not set or not detected.
	};

	/// Import flags, use IMPORT_ALL for all configurations
	enum ImportFlags : uint8_t {
		CONFIG      = 1,
		DEVICES     = 2,
		RESTRICTORS = 4,
		MAPPINGS    = 8
	};

	/**
	 * Structure with LED devices information.
	 */
	struct DeviceInfo {
		const string     name;
		/// Max number of devices that can be connected at the same time.
		const uint8_t    maxIds;
		/// Some devices only support ON/OFF
		const bool       monochrome;
		/// If the hardware supports a variable number of pins.
		const bool       variable;
		/// If the hardware have the pins in groups of 3.
		const bool       layoutRGB;
		/// If the hardware supports addressable RGB strips.
		const bool       supportStrip;
		/// Max number of pins.
		const uint16_t   pins;
		/// What connection uses.
		const Connection connection;
		/// A brief description of the device.
		const string     brief;
	};

	/**
	 * Structure with restrictors and rotators information.
	 */
	struct RestrictorInfo {
		const string       name;
		/// Max number of devices that can be connected at the same time.
		const uint8_t      maxIds;
		/// What connection uses.
		const Connection   connection;
		/// The number of individual interfaces that this controller handles.
		const uint8_t      interfaces;
		/// Supported restrictions.
		const vector<Ways> ways;
		/// A brief description of the restrictor.
		const string       brief;
	};

	Defaults() = delete;
	
	virtual ~Defaults() = default;

	/**
	 * To be use with devices and restrictors.
	 * @param name
	 * @param isDevice true for devices (default), false for restrictors
	 * @return true if the device uses IDs.
	 */
	static bool isIdUser(const string& name, bool isDevice = true);

	/**
	 * Devices only.
	 * @param name
	 * @return true if the device is multiple and monochrome.
	 */
	static bool isMonochrome(const string& name);

	/**
	 * Devices only.
	 * @param name
	 * @return true if the device can have a variable number of LEDS.
	 */
	static bool isVariable(const string& name);

	/**
	 * Devices and Restrictors.
	 * @param name
	 * @param isDevice true for devices (default), false for restrictors
	 * @return true if the device is a serial connected device.
	 */
	static bool isSerial(const string& name, bool isDevice = true);

	/**
	 * Restrictors only.
	 * @param name
	 * @return if the hardware have more than one end-point.
	 */
	static bool isMulti(const string& name);

	/**
	 * Common hardware unique ID generator.
	 * @param fieldsData
	 * @return
	 */
	static string createHardwareUniqueId(const StringUMap& fieldsData, bool isDevice = true);

	/**
	 * For most data fields that uses one or more strings this will just work.
	 * Only the necessary information in necessary.
	 * @param fieldsData
	 * @return
	 */
	static string createCommonUniqueId(const StringVector& fieldsData);

	/**
	 * Check if a string stores a numeric value
	 * @param number
	 * @return true if the content is a number
	 */
	static bool isNumber(const string& number);

	/**
	 * Check if a string number is between two values (inclusive)
	 * @param number
	 * @param low
	 * @param high
	 * @return
	 */
	static bool isBetween(const string& number, int low, int high);

	/**
	 * Encloses the unit with unit separator char.
	 * @param unit
	 * @return
	 */
	static string addUnitSeparator(const string& unit);

	/**
	 * Explodes a string into chunks using a delimiter.
	 *
	 * @param text
	 * @param delimiter
	 * @param limit
	 * @return an array with the string chunks.
	 */
	static StringVector explode(
		const string& text,
		const char delimiter,
		const size_t limit = 0
	);

	/**
	 * Merge an array into a string using a string delimiter.
	 * @param values
	 * @param delimiter
	 * @return
	 */
	static string implode(const StringVector& values, const string& delimiter);

	/**
	 * Merge an array into a string using a char delimiter.
	 * @param values
	 * @param delimiter
	 * @return
	 */
	static string implode(const StringVector& values, const char& delimiter);

	/**
	 * Merge a set into a string using a char delimiter.
	 * @param values Set of strings to merge.
	 * @param delimiter Character used to separate elements.
	 * @return Merged string with elements separated by the delimiter.
	 */
	static string implode(const StringUSet& values, const char& delimiter);

	/**
	 * Removes spaces from the left
	 * @param text
	 */
	static void ltrim(string& text);

	/**
	 * Removes spaces from the right.
	 * @param text
	 */
	static void rtrim(string& text);

	/**
	 * Removes spaces from both sides.
	 * @param text
	 */
	static void trim(string& text);

	/**
	 * Returns the color luminance.
	 * @param color
	 * @return
	 */
	static double getLuminance(const string& color);

	/**
	 * Detects the element type from a string.
	 * @param name
	 * @return Returns default if not found.
	 */
	static string detectElementType(const Glib::ustring& name);

	/// A list of all possible restrictors and rotators Ways (positions).
	static constexpr std::array<Ways, 11> allWays{Ways::w2, Ways::w2v, Ways::w4, Ways::w4x, Ways::w8, Ways::w16, Ways::w49, Ways::analog, Ways::mouse, Ways::rotary8, Ways::rotary12};

	/// A list of device to their information.
	static const unordered_map<string, DeviceInfo> devicesInfo;

	/// A list of restrictor to their information.
	static const unordered_map<string, RestrictorInfo> restrictorsInfo;

	/// A List of string names to its internal enumerated type.
	static const unordered_map<string, Ways> wayIds;

	/// A list of different element types.
	static const StringVector elementTypes;

	/**
	 * Initialize stuff here
	 * @param header to update the * when dirty.
	 */
	static void initialize(Gtk::HeaderBar* header, Gtk::Button* btnSave);

	/**
	 * Registers an Editable widget, so when it change the dirty flag is raised
	 * @param widget
	 */
	static void registerWidget(Gtk::Editable* widget);

	/**
	 * Registers a ComboBox widget, so when it change the dirty flag is raised
	 * @param widget
	 */
	static void registerWidget(Gtk::ComboBox* widget);

	/**
	 * Registers a ToggleButton widget, so when it change the dirty flag is raised
	 * @param widget
	 */
	static void registerWidget(Gtk::ToggleButton* widget);

	/**
	 * Set the state to dirty
	 */
	static void markDirty();

	/**
	 * Check for dirty state
	 * @return true if dirty
	 */
	static bool isDirty();

	/**
	 * Removes the dirty state.
	 */
	static void cleanDirty();

	/**
	 * Add tabulation.
	 */
	static void increaseTab();

	/**
	 * Reduce tabulation.
	 */
	static void reduceTab();

	/**
	 * @return return the tabulation.
	 */
	static string tab();

	/**
	 * Populates a combobox with numerical entries.
	 * @param combobox
	 * @param from
	 * @param to
	 */
	static void populateComboBoxTextWithNumbers(
		Gtk::ComboBoxText* comboBox,
		int from,
		int to,
		const StringVector& ignoreList = {}
	);

	/**
	 * Cleans and populates a combobox with IDs and mark used elements.
	 * @param store the ListStore with to fill
	 * @param max the maximum ID
	 * @param isUsedFn a function that returns true if the ID is used.
	 * @param emptyLabel The label for the first option for the empty value, if left empty, will be ignored.
	 * @param label the label to use for every ID, the ID number will be added at the end.
	 */
	static void populateComboBoxWithIds(
		Gtk::ListStore* store,
		uint max,
		std::function<bool(const string&)> isUsedFn,
		const string& emptyLabel,
		const string& label
	);

	/**
	 * Handles the filter for a flowbox.
	 * @param filterEntry
	 * @param box
	 */
	static void setFilter(Gtk::SearchEntry* filterEntry, Gtk::FlowBox* box);

	/**
	 * Sets the state to ignore Changes.
	 * @param state True to ignore changes, false otherwise.
	 */
	static void setIgnoreChanges(bool state);

	/**
	 * Extracts the name from a file and a root.
	 * @param inputFile
	 * @param root
	 * @return the plain file name with the root extracted.
	 */
	static string extractName(const string& filename, const string& root);

	/**
	 * Extracts relative path from a base directory, without extension.
	 * @param fullFileName The full file path.
	 * @param baseDir The base directory to cap to.
	 * @return Relative path without extension.
	 */
	static string capToDirectory(const string& fullFileName, const string& baseDir);

	/**
	 * Extracts substring after a prefix.
	 * @param line The source string.
	 * @param prefix The prefix to find.
	 * @return Trimmed string after prefix, or empty if not found.
	 */
	static string extractAfter(const string& line, const string& prefix);

	/**
	 * Executes a command and captures stdout.
	 * @param command The command to run.
	 * @param output Captured stdout.
	 * @return true if command succeeded.
	 */
	static bool runCommand(const string& command, string& output);

	/**
	 * Sanitizes a string for use as a filename/directory name.
	 * Removes invalid characters: / \ : * ? " < > |
	 * @param text The input string.
	 * @return Sanitized string.
	 */
	static string sanitizeFilename(const string& text);

	/**
	 * Sets the project directory.
	 * @param dir
	 */
	static void setProjectsDir(const string& dir);

	/**
	 * Gets the project directory.
	 * @return
	 */
	static string& getProjectsDir();

	/**
	 * Gets the current application mode.
	 * @return Current mode.
	 */
	static Mode getMode();

	/**
	 * Sets the current application mode.
	 * @param mode New mode to apply.
	 */
	static void setMode(Mode mode);

protected:

	/// Keeps track of the number of tabulations for XML files.
	inline static string tabs;

	/// Dirty Flag
	inline static bool dirty = false;
	inline static bool ignoreChanges = false;

	/// Current application mode.
	inline static Mode currentMode = Mode::Portable;

	/// Pointer to the header.
	inline static Gtk::HeaderBar* header = nullptr;

	inline static Gtk::Button* btnSave = nullptr;

	inline static string projectDir = "";

};

} // namespace
