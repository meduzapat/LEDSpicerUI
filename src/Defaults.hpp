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

#include <functional>

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include <memory>
using std::unique_ptr;

#include "Values.hpp"

#pragma once

namespace LEDSpicerUI {

using ValueVector = std::vector<Values>;
using DataMap     = std::unordered_map<string, ValueVector>;

namespace Constants {

/// Separators.
constexpr char
	FIELD_SEPARATOR    = 30,  // Record Separator (RS)
	RECORD_SEPARATOR   = 31,  // Unit Separator (US)
	UNIT_SEPARATOR     = 32,  // Space
	ID_SEPARATOR       = ',', // Comma
	ID_GROUP_SEPARATOR = '|'; // Pipe

/// The maximum number of attributes in a XML node before cutting attributes into rows.
constexpr unsigned int ATTRIBUTES_LIMIT_PER_ROW = 2;

/// Default values for monochrome devices.
constexpr float DEFAULT_CHANGE_VALUE = 64.00f;

/// Default milliseconds for solenoids and motors.
constexpr unsigned int DEFAULT_SOLENOID  = 50;

/// Milliseconds before an active LayoutElement returns to idle without further interaction.
constexpr unsigned int LAYOUT_TIMEOUT_MS = 20000;

/// Milliseconds the hardware stays lit for a single layout test command. No reset on re-click.
constexpr unsigned int LIGHT_TIMEOUT_MS  = 1500;

/// Default speed for GZ40 restrictor.
constexpr int GZ40_DEFAULT_SPEED = 12;  // Integer

// all strings constants.
inline const string

/// LEDSpicer configuration file keys.
	DEFAULT_USERID   {"1000"},
	DEFAULT_GROUPID  {"1000"},
	DEFAULT_PORT     {"16161"},
	DEFAULT_FPS      {"30"},
	DEFAULT_COLORS   {"basicColors"},
	DEFAULT_LOGLEVEL {"Info"},

/// Emitter configuration file keys.
	& DEFAULT_COLORSINFO   {HUMAN_TRUE},
	& DEFAULT_CRAFTPROFILE {HUMAN_TRUE},
	DEFAULT_DATASOURCE     {"file,mame"},

/// ProcessLookup configuration file keys.
	PARAM_MILLISECONDS {"runEvery"},
	PARAM_PROCESS_NAME {"processName"},
	PARAM_PROCESS_POS  {"position"},
	PARAM_SYSTEM       {"system"},
	& DEFAULT_RUNEVERY {emptyString},

/// Profile configuration keys.
	& DEFAULT_PROFILE  {DEFAULT},
	BACKGROUND_COLOR {"backgroundColor"},
	& DEFAULT_PROFILE_BACKGROUND_COLOR {HUMAN_OFF},
	TRANSITION       {"transition"},
	& DEFAULT_TRANSITION_SPEED {HUMAN_NORMAL},
	& DEFAULT_TRANSITION_COLOR {HUMAN_OFF},

/// Hardware configuration keys.

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
	LAYOUT_X      {"layoutX"},
	LAYOUT_Y      {"layoutY"},

	DEFAULT_BRIGHTNESS{"100"},

/// Runtime-only property keys.
/// Live on Data::properties. Never serialized to XML.

	/// Data is expandable into multiple runtime entries (e.g. a strip → N LEDs).
	PROP_EXPAND    {"p.e"},

	/// Hidden from selection pickers. Object still exists and can be referenced.
	PROP_NO_SELECT {"p.ns"},

	/// System-managed Data. Cannot be deleted, renamed, or duplicated.
	/// Lifecycle owned by UI bootstrap or an auto-handler.
	/// FlowBox visibility is independent — decided per creation site.
	PROP_SYSTEM    {"p.s"},

	/// Strip descriptor UID a child Element shares with its strip parent.
	PROP_STRIP     {"p.sd"},

	/// Stable identifier for a strip definition, shared by its expanded Elements.
	PROP_STRIP_UID {"p.sid"},

	& NO_COLOR {emptyString},

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
	SOURCE       {"source"},
	SPEED        {"speed"},
	BLINK        {"blink"},
	TIMES        {"times"},
	SOURCELESS   {"sl"},
	COINS_CREDIT {"coinsPerCredit"},
	MODE         {"mode"},
	ONCE         {"once"},
	ALWAYS_ON    {"alwaysOn"},

/// Animation actor configuration keys.
	ACTOR_GROUP    {"group"},
	COLORS         {"colors"},
	DIRECTION      {"direction"},
	BOUNCER        {"bouncer"},
	START_TIME     {"startTime"},
	END_TIME       {"endTime"},
	RESTART_TIME   {"restartTime"},
	REPEAT         {"repeat"},
	START_AT       {"startAt"},
	CYCLES         {"cycles"},
	TAIL_LENGTH    {"tailLength"},
	TAIL_COLOR     {"tailColor"},
	TAIL_INTENSITY {"tailIntensity"},
	TONES          {"tones"},
	CHANNEL        {"channel"},
	OFF            {"off"},
	LOW            {"low"},
	MID            {"mid"},
	HIGH           {"high"},
	PCM            {"pcm"},
	FORMAT         {"format"},

	DEFAULT_ACTOR_TOMES {"10"},

/// Animation actor type names (also the XML `type` attribute values).
	ANIM_TYPE_FILLER      {"Filler"},
	ANIM_TYPE_PULSE       {"Pulse"},
	ANIM_TYPE_GRADIENT    {"Gradient"},
	ANIM_TYPE_SERPENTINE  {"Serpentine"},
	ANIM_TYPE_RANDOM      {"Random"},
	ANIM_TYPE_FILE_READER {"FileReader"},
	ANIM_TYPE_ALSA_AUDIO  {"AlsaAudio"},
	ANIM_TYPE_PULSE_AUDIO {"PulseAudio"},

/// Direction values (also relabelled to Outward/Inward for audio actors).
	DIRECTION_FORWARD  {"Forward"},
	DIRECTION_BACKWARD {"Backward"},
	DIRECTION_OUTWARD  {"Outward"},
	DIRECTION_INWARD   {"Inward"},

/// Filter values.
	FILTER_NORMAL  {"Normal"},
	FILTER_COMBINE {"Combine"},

/// Animation actor mode values (per-type domain).
	MODE_NORMAL      {"Normal"},
	MODE_RANDOM      {"Random"},
	MODE_WAVE        {"Wave"},
	MODE_CURTAIN     {"Curtain"},
	MODE_LINEAR      {"Linear"},
	MODE_EXPONENTIAL {"Exponential"},
	MODE_ALL         {"All"},
	MODE_CYCLIC      {"Cyclic"},
	MODE_VU_METER    {"VuMeter"},
	MODE_SINGLE      {"Single"},
	MODE_DISCO       {"Disco"},

/// Audio channel values.
	CHANNEL_BOTH  {"Both"},
	CHANNEL_MONO  {"Mono"},
	CHANNEL_LEFT  {"Left"},
	CHANNEL_RIGHT {"Right"},

/// Audio palette defaults.
	AUDIO_DEFAULT_OFF  {"Off"},
	AUDIO_DEFAULT_LOW  {"Green"},
	AUDIO_DEFAULT_MID  {"Yellow"},
	AUDIO_DEFAULT_HIGH {"Red"},

/// FileReader format values.
	FORMAT_RGBA {"rgba"},

/// UI-related constants.
	DEFAULT_ELEMENT_TYPE {"9"},
	PLAYER   {"player"},
	JOYSTICK {"joystick"},

/// Types
	TYPE_MAP             {"map"},
	TYPE_MAPS            {"maps"},
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
	TYPE_ANIMATION       {"animation"},
	TYPE_ACTOR           {"actor"},
	TYPE_PROFILE         {"profile"},
	TYPE_TRANSITION      {"transition"},
	TYPE_CONFIGURATION   {"configuration"},

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
	COLLECTION_ACTORS          {"ac"},
	COLLECTION_PROFILES        {"pr"},

/// Reserved name of the system "All" group.
	GROUP_ALL_NAME {"All"},

/// Link families keys.
	COLLECTION_GROUP_LINKS        {"g.l"},
	COLLECTION_PROFILE_ELEMENTS   {"pr.e"},
	COLLECTION_PROFILE_GROUPS     {"pr.g"},
	COLLECTION_PROFILE_INPUTS     {"pr.i"},
	COLLECTION_PROFILE_ANIMATIONS {"pr.a"},
	COLLECTION_INPUT_MAP_LINKS    {"i.m.l"},

/// Directories
	COLLECTION_DIRECTORIES {"ds"},

/// Path related.
	PATH_BASE {"pb"},

/// Suffix appended to a project directory during the rename-based save transaction.
	BACKUP_SUFFIX {".bak"},

// CSS classes.
/// Storage box buttons
	CSS_BOX_BUTTON                {"BoxButton"},
	CSS_ACTOR_BOX_BUTTON          {"ActorBoxButton"},
	CSS_ANIMATION_BOX_BUTTON      {"AnimationBoxButton"},
	CSS_DEVICE_BOX_BUTTON         {"DeviceBoxButton"},
	CSS_DIRECTORY_BOX_BUTTON      {"DirectoryBoxButton"},
	CSS_ELEMENT_BOX_BUTTON        {"ElementBoxButton"},
	CSS_GROUP_BOX_BUTTON          {"GroupBoxButton"},
	CSS_INPUT_BOX_BUTTON          {"InputBoxButton"},
	CSS_INPUT_MAP_BOX_BUTTON      {"InputMapBoxButton"},
	CSS_INPUT_SOURCE_BOX_BUTTON   {"InputSourceBoxButton"},
	CSS_LINK_BOX_BUTTON           {"LinkBoxButton"},
	CSS_PROCESS_BOX_BUTTON        {"ProcessBoxButton"},
	CSS_PROFILE_BOX_BUTTON        {"ProfileBoxButton"},
	CSS_RESTRICTOR_BOX_BUTTON     {"RestrictorBoxButton"},
	CSS_RESTRICTOR_MAP_BOX_BUTTON {"RestrictorMapBoxButton"},

/// Color names (canonical, used in test dispatch and CSS class lookup).
	COLOR_RED     {"Red"},
	COLOR_GREEN   {"Green"},
	COLOR_BLUE    {"Blue"},
	COLOR_YELLOW  {"Yellow"},
	COLOR_MAGENTA {"Magenta"},
	COLOR_CYAN    {"Cyan"},
	COLOR_WHITE   {"White"};

} // namespace

// CSS classes.

// Box backgrounds
#define CSS_BOX_BACKGROUND_DELETE     "BoxBackgroundDelete"
#define CSS_BOX_BACKGROUND_EDIT       "BoxBackgroundEdit"
#define CSS_BOX_BACKGROUND_COPY       "BoxBackgroundCopy"

// Breadcrumb
#define CSS_BREADCRUMB_BUTTON         "BreadcrumbButton"
#define CSS_BREADCRUMB_SEPARATOR      "BreadcrumbSeparator"
#define CSS_BREADCRUMB_CURRENT        "BreadcrumbCurrent"

// Color picker
#define CSS_COLOR_BUTTON              "ColorButton"
#define CSS_COLOR_PIN                 "pinSingle"
#define CSS_COLOR_SOLENOID            CSS_LED_M
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

// Status bar severities
#define CSS_STATUS_INFO               "status-info"
#define CSS_STATUS_SUCCESS            "status-success"
#define CSS_STATUS_WARNING            "status-warning"
#define CSS_STATUS_ERROR              "status-error"

// Icons — all symbolic
#define ICON_COPY   "edit-copy-symbolic"
#define ICON_DELETE "edit-delete-symbolic"
#define ICON_EDIT   "emblem-system-symbolic"
#define ICON_TRASH  "user-trash-symbolic"

// Layout
#define CSS_LAYOUT_ELEMENT        "layout-element"
#define CSS_LAYOUT_ELEMENT_ACTIVE "layout-element-active"
#define CSS_LAYOUT_ELEMENT_ICON   "layout-element-icon"
#define CSS_LAYOUT_ELEMENT_FIRED  "layout-element-icon-fired"
#define CSS_STRIP_LED             "strip-led"
#define CSS_STRIP_RENDERER        "strip-renderer"

// LED swatches (shared by StripRenderer cells, RGB toggles, fire glow).
#define CSS_LED_PREFIX "led-"
#define CSS_LED_OFF    "led-off"
#define CSS_LED_R      "led-r"
#define CSS_LED_G      "led-g"
#define CSS_LED_B      "led-b"
#define CSS_LED_Y      "led-y"
#define CSS_LED_M      "led-m"
#define CSS_LED_C      "led-c"
#define CSS_LED_W      "led-w"


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

	/// Profile transition capability flags.
	enum TransitionFlags : uint8_t {
		TRANS_HAS_SPEED = 1 << 0, /// Has a speed setting.
		TRANS_HAS_COLOR = 1 << 1, /// Has a color setting.
	};

	/// Animation actor capability flags.
	enum AnimationFlags : uint8_t {
		ANIM_USES_FRAME     = 1 << 0, /// Frame-family actor: speed, startAt, cycles.
		ANIM_USES_DIRECTION = 1 << 1, /// Direction-family actor: direction + bouncer.
		ANIM_USES_AUDIO     = 1 << 2, /// Audio-family actor: mode/channel/off/low/mid/high; direction is Outward/Inward.
		ANIM_HAS_COLOR      = 1 << 3, /// Uses a single `color` attribute.
		ANIM_HAS_COLORS     = 1 << 4, /// Uses a multi `colors` attribute. Both COLOR and COLORS = XOR choice.
		// Reserved: ANIM_CAN_FADE = 1 << 5
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

	/**
	 * Structure with animation actor information.
	 */
	struct AnimationInfo : public BaseInfo {
		const uint8_t flags; /// Bitwise capability flags (AnimationFlags).
	};

	/**
	 * Structure with color reference information.
	 * name  — contract key, must match every valid color file.
	 * brief — tooltip / calibration dialog hint.
	 * pure  — ideal hex on a calibrated display (RRGGBB, no #).
	 */
	struct ColorInfo : public BaseInfo {
		const string pure;
	};

	/**
	 * Structure with profile transition information.
	 */
	struct TransitionInfo : public BaseInfo {
		const uint8_t flags; /// Bitwise capability flags (TransitionFlags).
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
	 * @param transition The transition name to check.
	 * @param flag The flag to check for.
	 * @return True if the current transition has the specified flag.
	 */
	static bool transitionHasFlag(const string& transition, uint8_t flag);

	/**
	 * Common hardware unique ID generator.
	 * @param fieldsData
	 * @return
	 */
	static string createHardwareUniqueId(const Values& fieldsData, bool isDevice = true);

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
	 * Uppercases the first character of each whitespace-delimited word.
	 * Non-letter characters act as word boundaries.
	 * @param text
	 * @return Title-cased copy of text.
	 */
	static string titleCase(const string& text) noexcept;

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

	/// A list of animation actor types to their information.
	static const std::unordered_map<string, AnimationInfo> animationsInfo;

	/// A list of profile transition types to their information.
	static const std::unordered_map<string, TransitionInfo> transitionsInfo;

	/// A List of string names to its internal enumerated type.
	static const std::unordered_map<string, Ways> wayIds;

	/// A list of different element types.
	static const StringVector elementTypes;

	/// Standard colors in family order — defines the name contract for color files.
	static const vector<ColorInfo> legalColors;

	/// Returns true if name belongs to the standard color set.
	static bool isLegalColor(const string& name) noexcept;

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
	 * @param invert if true, the logic is inverted: switch on disables the widget, switch off enables it.
	 */
	static void linkSwitchToWidget(
		Gtk::Switch* sw,
		Gtk::Widget* widget,
		bool invert = false
	) noexcept;

	/**
	 * Links a Gtk::ToggleButton to a widget, controlling its sensitivity:
	 * toggle active enables the widget, inactive disables it.
	 * @param toggle the controlling toggle button.
	 * @param widget the controlled widget.
	 * @param invert if true, the logic is inverted: active disables the widget, inactive enables it.
	 */
	static void linkToggleToWidget(
		Gtk::ToggleButton* toggle,
		Gtk::Widget* widget,
		bool invert = false
	) noexcept;

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
	 * Sanitizes a string for use as a filename or directory name.
	 * Removes control characters, filesystem-unsafe characters (/ \ : * ? " < > |),
	 * and leading dots. Truncates to maxLen bytes.
	 * @param text   The input string.
	 * @param maxLen Maximum allowed length (default 64).
	 * @return Sanitized string.
	 */
	static string sanitizeFilename(const string& text, size_t maxLen = 64);

	/**
	 * Sanitizes a string for use as a general name (element, group, process, etc.).
	 * Removes control characters (including internal separator bytes) and XML
	 * attribute-unsafe characters (< > & "). Spaces and other printable ASCII are kept.
	 * @param text The input string.
	 * @return Sanitized string.
	 */
	static string sanitizeName(const string& text) noexcept;

	/**
	 * Escapes a string for safe embedding as an XML attribute value.
	 * Replaces & < > " with their XML entity equivalents.
	 * @param s The raw attribute value.
	 * @return Escaped string.
	 */
	static string escapeXmlValue(const string& s) noexcept;

	/**
	 * Attaches a filename sanitization filter to a Gtk::Entry.
	 * Connects a signal_changed handler that strips control characters,
	 * filesystem-unsafe characters, and leading dots on every change.
	 * Also sets the widget's maximum input length.
	 * @param entry  The entry widget to filter.
	 * @param maxLen Maximum allowed length (default 64).
	 */
	static void attachFilenameFilter(Gtk::Entry* entry, size_t maxLen = 64) noexcept;

	/**
	 * Attaches a name sanitization filter to a Gtk::Entry.
	 * Connects a signal_changed handler that strips control characters and
	 * XML-unsafe characters on every change.
	 * Also sets the widget's maximum input length.
	 * @param entry  The entry widget to filter.
	 * @param maxLen Maximum allowed length (default 128).
	 */
	static void attachNameFilter(Gtk::Entry* entry, size_t maxLen = 128) noexcept;

protected:

	/// Keeps track of the number of tabulations for XML files.
	inline static string tabs;

	/// Dirty Flag
	inline static bool dirty = false;
	inline static bool ignoreChanges = false;

	/// Pointer to the header.
	inline static Gtk::HeaderBar* header = nullptr;

	inline static Gtk::Button* btnSave = nullptr;

};

} // namespace
