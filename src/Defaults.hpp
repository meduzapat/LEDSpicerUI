/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.hpp
 * @since     May 6, 2023
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

#include <gtkmm.h>

#include <unordered_map>
using std::unordered_map;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <functional>

// Compiler settings.
#include "config.h"

#ifndef DEFAULTS_HPP_
#define DEFAULTS_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR "ledspicer.conf"
#define XML_FILE_FOREIGN ""

#define DEFAULT_MESSAGE "This is an auto-generated file by " PACKAGE_STRING "."

// Separators.
#define FIELD_SEPARATOR  static_cast<char>(30)
#define RECORD_SEPARATOR static_cast<char>(31)
#define UNIT_SEPARATOR   static_cast<char>(32)
#define ID_SEPARATOR       ','
#define ID_GROUP_SEPARATOR '|'

#define DEFAULT_USERID   "1000"
#define DEFAULT_GROUPID  "1000"
#define DEFAULT_PORT     "16161"
#define DEFAULT_FPS      "30"
#define DEFAULT_COLORS   "basicColors"
#define DEFAULT_LOGLEVEL "Info"

#define DEFAULT_COLORSINFO   "true"
#define DEFAULT_CRAFTPROFILE "true"
#define DEFAULT_DATASOURCE   "file,mame"

#define DEFAULT_RUNEVERY ""
#define DEFAULT_PROFILE  "default"

#define NAME     "name"
#define ID       "boardId"
#define FILENAME "filename"
#define PORT     "port"
#define PINS     "leds"

#define PIN       "led"
#define RED_PIN   "red"
#define GREEN_PIN "green"
#define BLUE_PIN  "blue"
#define SOLENOID  "solenoid"
#define TIME_ON   "timeOn"

#define CHANGE_POINT "changePoint"
#define DEFAULT_CHANGE_VALUE 64.00

#define DEFAULT_COLOR  "defaultColor"
#define DEFAULT_SOLENOID 50

#define PIN_COLOR      "pin_white"
#define SOLENOID_COLOR "pin_solenoid"
#define RED_COLOR      "pin_red"
#define GREEN_COLOR    "pin_green"
#define BLUE_COLOR     "pin_blue"
#define NO_COLOR       ""

#define PARAM_MILLISECONDS "runEvery"
#define PARAM_PROCESS_NAME "processName"
#define PARAM_PROCESS_POS  "position"
#define PARAM_SYSTEM       "system"

#define DEFAULT_ELEMENT_TYPE "9"

#define US360_HAS_RESTRICTOR "hasRestrictor"
#define US360_USE_MOUSE      "handleMouse"
#define GZ49_WILLIAMS        "williams"
#define GZ40_SPEED_ON        "speedOn"
#define GZ40_SPEED_OFF       "speedOff"
#define GZ40_DEFAULT_SPEED   12
#define RESTRICTOR_INTERFACE "id"

#define PLAYER               "player"
#define JOYSTICK             "joystick"

#define LINKED_ITEMS         "linkedTriggers"
#define TRIGGER              "trigger"
#define TARGET               "target"
#define TYPE                 "type"
#define COLOR                "color"
#define FILTER               "filter"
#define ELEMENT              "Element"
#define GROUP                "Group"

#define IMPORT_CONFIG      1
#define IMPORT_DEVICES     2
#define IMPORT_RESTRICTORS 4
#define IMPORT_MAPPINGS    8
#define IMPORT_INPUTS      16
#define IMPORT_ALL         15

// Collections
#define COLLECTION_DEVICES           "devices"
#define COLLECTION_RESTRICTORS       "restrictors"
#define COLLECTION_ELEMENT           "elements"
#define COLLECTION_GROUP             "groups"
#define COLLECTION_PROCESS           "processes"
#define COLLECTION_RESTRICTOR_MAP    "playerCombinations"
#define COLLECTION_INPUT             "inputNames"
#define COLLECTION_INPUT_MAPS        "triggers"
#define COLLECTION_INPUT_LINKED_MAPS "linkedMaps"

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::Defaults
 * Static class that contains definitions, helper functions, and other generic utilities.
 */
class Defaults {

public:

	/// Taken from Restrictors in ledspicer.
	enum class Ways : uint8_t {invalid, w2, w2v, w4, w4x, w8, w16, w49, analog, mouse, rotary8, rotary12};

	enum class Connection : uint8_t {NONE, USB, SERIAL};

	/**
	 * Structure with LED devices information.
	 */
	struct DeviceInfo {
		const string     name;
		/// Max number of devices that can be connected at the same time.
		const uint8_t    maxIds;
		const bool       monochrome;
		/// The number of pins should be defined.
		const bool       variable;
		/// Max number of pins.
		const uint8_t    pins;
		const Connection connection;
	};

	/**
	 * Structure with restrictors and rotators information.
	 */
	struct RestrictorInfo {
		const string       name;
		/// Max number of devices that can be connected at the same time.
		const uint8_t      maxIds;
		const Connection   connection;
		/// The number of interfaces that this controller handles.
		const uint8_t      interfaces;
		const vector<Ways> ways;
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
	static bool isMonocrome(const string& name);

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
	static string createHardwareUniqueId(const unordered_map<string, string>& fieldsData, bool isDevice = true);

	/**
	 * For most data fields that uses one or more strings this will just work.
	 * Only the necessary information in necessary.
	 * @param fieldsData
	 * @return
	 */
	static string createCommonUniqueId(const vector<string>& fieldsData);

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
	static vector<string> explode(
		const string& text,
		const char delimiter,
		const size_t limit = 0
	);

	/**
	 * Merge an array into a string using a delimiter.
	 * @param values
	 * @param delimiter
	 * @return
	 */
	static string implode(const vector<string>& values, const string& delimiter);

	static string implode(const vector<string>& values, const char& delimiter);

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
	static double getLiminance(const string& color);

	/**
	 * Detects the element type from a string.
	 * @param name
	 * @return Returns misc if not found.
	 */
	static string detectElementType(const Glib::ustring& name);

	/// A list of all posible restrictors and rotators Ways (positions).
	static const vector<Ways> allWays;

	/// A list of device to their information.
	static const unordered_map<string, DeviceInfo> devicesInfo;

	/// A list of restrictor to their information.
	static const unordered_map<string, RestrictorInfo> restrictorsInfo;

	/// A List of string names to its internal enumerated type.
	static const unordered_map<string, Ways> wayIds;

	/// A list of different element types.
	static const vector<string> elementTypes;

	/**
	 * Set a new subtitle.
	 * @param text
	 */
	static void setSubtitle(const string& text);

	/**
	 * Init stuff here
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
	 * Populates a combobox with values from array of text.
	 * @param combobox
	 * @param values
	 */
	static void populateComboBoxText(Gtk::ComboBoxText* comboBox, const vector<string>& values);

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
		const vector<string>& ignoreList = {}
	);

	/**
	 * Cleans and populates a combobox with IDs and mark used elements.
	 * @param store the ListStore with to fill
	 * @param max the maximun ID
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

protected:

	static string tabs;

	/// Dirty Flag
	static bool dirty;

	/// Pointer to the header.
	static Gtk::HeaderBar* header;

	static Gtk::Button* btnSave;
};

} /* namespace */

#endif /* DEFAULTS_HPP_ */
