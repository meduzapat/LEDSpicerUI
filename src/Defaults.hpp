/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.hpp
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

#include <gtkmm.h>
#include <glibmm.h>
#include <giomm.h>

#include <unordered_map>
#include <unordered_set>

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::stringstream;

#include <string_view>
using std::string_view;

#include <functional>

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include <memory>
using std::unique_ptr;

// This looks cool.
#define abstract = 0

#pragma once

#define XML_FILE_PLAIN ""
#define INPUT_PATH "inputs/"
#define ANIMATION_PATH "animations/"
#define PROFILE_PATH "profiles/"

#define DEFAULT_MESSAGE "This is an auto-generated file by " PACKAGE_STRING "."

using StringUMap       = std::unordered_map<string, string>;
using StringMap        = std::map<string, string>;
using StringUMapVector = vector<StringUMap>;
using StringUSet       = std::unordered_set<string>;
using StringSet        = std::set<string>;
using StringVector     = vector<string>;

namespace LEDSpicerUI {

namespace Constants {

/// Commonly used empty string.
const string emptyString;

/// Separators.
constexpr char
	FIELD_SEPARATOR    = 30,  // Record Separator (RS)
	RECORD_SEPARATOR   = 31,  // Unit Separator (US)
	UNIT_SEPARATOR     = 32,  // Space
	ID_SEPARATOR       = ',', // Comma
	ID_GROUP_SEPARATOR = '|'; // Pipe

/// Default values for monochrome devices.
constexpr float DEFAULT_CHANGE_VALUE = 64.00f;

/// Default millisenconds for solenoids and motors.
constexpr unsigned int DEFAULT_SOLENOID  = 50;

/// Default speed for GZ40 restrictor.
constexpr int GZ40_DEFAULT_SPEED = 12;  // Integer

// all strings constants.
const string
/// LEDSpicer configuration file keys.
	DEFAULT_USERID   {"1000"},
	DEFAULT_GROUPID  {"1000"},
	DEFAULT_PORT     {"16161"},
	DEFAULT_FPS      {"30"},
	DEFAULT_COLORS   {"basicColors"},
	DEFAULT_LOGLEVEL {"Info"},

/// Emitter configuration file keys.
	DEFAULT_COLORSINFO   {"true"},
	DEFAULT_CRAFTPROFILE {"true"},
	DEFAULT_DATASOURCE   {"file,mame"},

/// ProcessLookup configuration file keys.
	PARAM_MILLISECONDS {"runEvery"},
	PARAM_PROCESS_NAME {"processName"},
	PARAM_PROCESS_POS  {"position"},
	PARAM_SYSTEM       {"system"},
	DEFAULT_RUNEVERY   {emptyString},

/// Profile configuration keys.
	DEFAULT_PROFILE                  {"default"},
	BACKGROUND_COLOR                 {"backgroundColor"},
	DEFAULT_PROFILE_BACKGROUND_COLOR {"Off"},

/// Hardware configuration keys.
	NAME {"name"},
	ID   {"boardId"},
	PORT {"port"},
	PINS {"leds"},
	PATH {"path"},

/// Special cases.
	FILENAME {"filename"},
	UID {"UID"}, /// Stable self-identifier, set at construction.
	PID {"PID"}, /// Parent UID, injected by createData() noexcept.
	SID {"SID"}, /// To be used with anything that has a S in the name.
	IID {"IID"}, /// To be used with anything that has a I in the name.

/// Element configuration keys.
	PIN           {"led"},
	RED_PIN       {"red"},
	GREEN_PIN     {"green"},
	BLUE_PIN      {"blue"},
	SOLENOID      {"solenoid"},
	POSITION      {"position"},
	POSITIONS     {"positions"},
	STRIPSIZE     {"stripSize"},
	TIME_ON       {"timeOn"},
	COLORFORMAT   {"colorFormat"},
	CHANGE_POINT  {"changePoint"},
	DEFAULT_COLOR {"defaultColor"},
	BRIGHTNESS    {"brightness"},

	DEFAULT_BRIGHTNESS{"100"},

/// Special properties keys.
	PROP_EXPAND    {"p.e"},
	PROP_NO_SELECT {"p.ns"},
	PROP_SYSTEM    {"p.s"},
	PROP_STRIP     {"p.sd"},
	PROP_STRIP_UID {"p.sid"}, // strip descriptor UID

	NO_COLOR       {emptyString},

/// Restrictor configuration keys.
	US360_HAS_RESTRICTOR {"hasRestrictor"},
	US360_USE_MOUSE      {"handleMouse"},
	GZ49_WILLIAMS        {"williams"},
	GZ40_SPEED_ON        {"speedOn"},
	GZ40_SPEED_OFF       {"speedOff"},
	RESTRICTOR_INTERFACE {"id"},

/// Input mapping configuration keys.
	LINKED_ITEMS {"linkedTriggers"},
	TRIGGER      {"trigger"},
	TARGET       {"target"},
	TYPE         {"type"},
	COLOR        {"color"},
	FILTER       {"filter"},
	ELEMENT      {"Element"},
	GROUP        {"Group"},
	SOURCE       {"source"},     /// input attribute in XML
	INDEX        {"index"},      /// a property positional index.
	SPEED        {"speed"},      /// Input playback speed setting.
	BLINK        {"blink"},      /// Input blink switch.
	TIMES        {"times"},      /// Input repeat count.
	SOURCELESS   {"sourceless"},

/// UI-related constants.
	DEFAULT_ELEMENT_TYPE {"9"},
	PLAYER   {"player"},
	JOYSTICK {"joystick"},

/// Types
	TYPE_MAP             {"map"},
	TYPE_DIRECTORY       {"directory"},
	TYPE_DEVICE          {"device"},
	TYPE_RESTRICTOR      {"restrictor"},
	TYPE_ELEMENT         {"element"},
	TYPE_GROUP           {"group"},
	TYPE_PROCESS         {"process"},
	TYPE_RESTRICTOR_MAP  {"player mapping"},
	TYPE_INPUT           {"input"},
	TYPE_INPUT_SOURCE    {"input source"},
	TYPE_INPUT_MAP       {"input map"},
	TYPE_INPUT_LINKMAP   {"input linked map"},
	TYPE_INPUT_DIR       {"input directory"},
	TYPE_ANIMATION       {"animation"},
	TYPE_PROFILE         {"profile"},

/// Collection / families — dual-purpose string constants.
	COLLECTION_DEVICES         {"d"},
	COLLECTION_RESTRICTORS     {"r"},
	COLLECTION_ELEMENTS        {"e"},
	COLLECTION_GROUPS          {"g"},
	COLLECTION_PROCESSES       {"p"},
	COLLECTION_RESTRICTOR_MAPS {"r.m"},
	COLLECTION_INPUTS          {"i"},
	COLLECTION_INPUT_SOURCES   {"i.s"},
	COLLECTION_INPUT_MAPS      {"i.m"},
	COLLECTION_INPUT_LINKMAPS  {"i.l"},
	COLLECTION_ANIMATIONS      {"a"},
	COLLECTION_PROFILES        {"pr"},

/// Link families keys.
	COLLECTION_GROUP_LINKS        {"g.l"},
	COLLECTION_PROFILE_ELEMENTS   {"pr.e"},
	COLLECTION_PROFILE_GROUPS     {"pr.g"},
	COLLECTION_PROFILE_INPUTS     {"pr.i"},
	COLLECTION_PROFILE_ANIMATIONS {"pr.a"},
	COLLECTION_INPUT_MAP_LINKS    {"i.m.l"},
/// Directories
	COLLECTION_DIRECTORIES       {"ds"},
	COLLECTION_INPUT_DIRECTORIES {"ids"};

} // namespace

// CSS classes — CSS_SUBJECT_INTENTION

// Box backgrounds
#define CSS_BOX_BACKGROUND_DELETE     "BoxBackgroundDelete"
#define CSS_BOX_BACKGROUND_EDIT       "BoxBackgroundEdit"
#define CSS_BOX_BACKGROUND_COPY       "BoxBackgroundCopy"
#define CSS_BACKGROUND_RED            "backgroundRed"
#define CSS_BACKGROUND_GREEN          "backgroundGreen"

// Breadcrumb
#define CSS_BREADCRUMB_BUTTON         "BreadcrumbButton"
#define CSS_BREADCRUMB_SEPARATOR      "BreadcrumbSeparator"
#define CSS_BREADCRUMB_CURRENT        "BreadcrumbCurrent"

// Color picker
#define CSS_COLOR_BUTTON              "ColorButton"
#define CSS_COLOR_PIN                 "pinSingle"
#define CSS_COLOR_SOLENOID            "pinSolenoid"
#define CSS_COLOR_RED                 "pinRed"
#define CSS_COLOR_GREEN               "pinGreen"
#define CSS_COLOR_BLUE                "pinBlue"
#define CSS_COLOR_MULTIPLE            "pinMulti"

// Pin / connector
#define CSS_PIN_LABEL                 "pinLabel"
#define CSS_BOX_CONNECTOR             "connectorBox"

// Form / layout
#define CSS_FORM_CONTAINER            "formContainer"
#define CSS_SYSTEM                    "system"

// Storage box buttons
#define CSS_BOX_BUTTON                "BoxButton"
#define CSS_DEVICE_BOX_BUTTON         "DeviceBoxButton"
#define CSS_DIRECTORY_BOX_BUTTON      "DirectoryBoxButton"
#define CSS_ELEMENT_BOX_BUTTON        "ElementBoxButton"
#define CSS_GROUP_BOX_BUTTON          "GroupBoxButton"
#define CSS_INPUT_BOX_BUTTON          "InputBoxButton"
#define CSS_INPUT_MAP_BOX_BUTTON      "InputMapBoxButton"
#define CSS_INPUT_SOURCE_BOX_BUTTON   "InputSourceBoxButton"
#define CSS_LINK_BOX_BUTTON           "LinkBoxButton"
#define CSS_PROCESS_BOX_BUTTON        "ProcessBoxButton"
#define CSS_PROFILE_BOX_BUTTON        "ProfileBoxButton"
#define CSS_RESTRICTOR_BOX_BUTTON     "RestrictorBoxButton"
#define CSS_RESTRICTOR_MAP_BOX_BUTTON "RestrictorMapBoxButton"

// Icons — all symbolic
#define ICON_COPY   "edit-copy-symbolic"
#define ICON_DELETE "edit-delete-symbolic"
#define ICON_EDIT   "emblem-system-symbolic"
#define ICON_TRASH  "user-trash-symbolic"


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

	/// LEDSpicerd mode.
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

	/// Input capability flags.
	enum InputFlags : uint8_t {
		INPUT_NEEDS_SOURCE    = 1 << 0, /// Has multiple hardware sources.
		INPUT_DEV_LISTENER    = 1 << 1, /// Reads from /dev/input/ kernel devices.
		INPUT_LINKED_MAPS     = 1 << 2, /// Supports linked map triggers.
		INPUT_HAS_SPEED       = 1 << 3, /// Has speed setting.
		INPUT_HAS_TIMES       = 1 << 4, /// Has repeat times setting.
		INPUT_HAS_BLINK       = 1 << 5, /// Has blink switch.
		INPUT_HAS_CREDITS     = 1 << 6, /// Has credits-specific settings.
	};

	struct BaseInfo {
		const string name;  /// Human-readable display name.
		const string brief; /// A brief description of the device.
	};

	/**
	 * Structure with LED devices information.
	 */
	struct DeviceInfo : public BaseInfo {
		const uint8_t    maxIds;       /// Max number of devices that can be connected at the same time.
		const bool       monochrome;   /// Some devices only support ON/OFF
		const bool       variable;     /// If the hardware supports a variable number of pins.
		const bool       layoutRGB;    /// If the hardware have the pins in groups of 3.
		const bool       supportStrip; /// If the hardware supports addressable RGB strips.
		const uint16_t   pins;         /// Max number of pins.
		const Connection connection;   /// What connection uses.
	};

	/**
	 * Structure with restrictors and rotators information.
	 */
	struct RestrictorInfo : public BaseInfo {
		const uint8_t      maxIds;     /// Max number of devices that can be connected at the same time.
		const Connection   connection; /// What connection uses.
		const uint8_t      interfaces; /// The number of individual interfaces that this controller handles.
		const vector<Ways> ways;       /// Supported restrictions.
	};

	/**
	 * Structure with input information.
	 */
	struct InputInfo : public BaseInfo {
		const uint8_t flags; /// Bitwise capability flags.
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
	 * @param input The input name to check.
	 * @param flag The flag to check for.
	 * @return True if the current input has the specified flag.
	 */
	static bool inputHasFlag(const string& input, uint8_t flag);

	/**
	 * @param input The input name to check.
	 * @return True if the current input needs a source.
	 */
	static bool needSource(const string& input);

	/**
	 * Returns true if the input type supports linked map triggers.
	 * @param input name.
	 * @returns True if the input uses linked maps.
	 */
	static bool hasLinkedMaps(const string& input);

	/**
	 * @param input The input name to check.
	 * @return True if the current input is a dev/input listener.
	 */
	static bool isDevInputListener(const string& input);

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
	static const std::unordered_map<string, DeviceInfo> devicesInfo;

	/// A list of restrictor to their information.
	static const std::unordered_map<string, RestrictorInfo> restrictorsInfo;

	/// A list of input to their information.
	static const std::unordered_map<string, InputInfo> inputInfo;

	/// A List of string names to its internal enumerated type.
	static const std::unordered_map<string, Ways> wayIds;

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
	 * Sets the subtitle in the header bar.
	 * @param text The text to display, or empty to clear.
	 */
	static void setSubtitle(const string& text);

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
	 * Selects the first available (unused) ID in a combo box.
	 * @param combo the combo box to update.
	 * @param max the maximum ID to check.
	 * @param isUsedFn a function that returns true if the ID is already in use.
	 */
	static void selectFirstAvailableId(
		Gtk::ComboBox* combo,
		uint max,
		std::function<bool(const string&)> isUsedFn
	) noexcept;

	/**
	 * Handles the filter for a flowbox.
	 * @param filterEntry
	 * @param box
	 * @param dialog The parent dialog, used to cancel the dialog when the filter is empty and the user tries to stop searching.
	 */
	static void setFilter(Gtk::SearchEntry* filterEntry, Gtk::FlowBox* box, Gtk::Dialog* dialog);

	/**
	 * Links a Gtk::Switch to a widget, controlling its sensitivity:
	 * switch on enables the widget, switch off disables it.
	 * @param sw the controlling switch.
	 * @param widget the controlled widget.
	 */
	static void linkSwitchToWidget(Gtk::Switch* sw, Gtk::Widget* widget) noexcept;

	/**
	 * Links a Gtk::ToggleButton to a widget, controlling its sensitivity:
	 * toggle active enables the widget, inactive disables it.
	 * @param toggle the controlling toggle button.
	 * @param widget the controlled widget.
	 */
	static void linkToggleToWidget(Gtk::ToggleButton* toggle, Gtk::Widget* widget) noexcept;

	/**
	 * Sets the state to ignore Changes.
	 * @param state True to ignore changes, false otherwise.
	 */
	static void setIgnoreChanges(bool state);

	/**
	 * Appends a numeric index to duplicate display labels in a StringMap.
	 * Entries with unique labels are unchanged; duplicates become "Label 1", "Label 2", etc.
	 * @param items id → display label map to process in-place.
	 */
	static void indexDuplicateLabels(StringMap& items);

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
