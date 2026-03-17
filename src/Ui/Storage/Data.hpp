/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.hpp
 * @since     Feb 28, 2023
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

#include "Message.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Data
 *
 * Data will store information to be moved around.
 */
class Data {

public:

	Data() = delete;

	/**
	 * Creates an object pre-populated.
	 * @param data Iten's data.
	 */
	Data(StringUMap& data) : fieldsData(std::move(data)) {}

	/**
	 * Compares two Data objects for equality.
	 * @param other The other Data object to compare with.
	 * @return True if the objects are equal, false otherwise.
	 */
	virtual bool operator==(const Data& other) const;

	virtual ~Data();

	/**
	 * @returns a CSS class that identifies the object and the data.
	 */
	virtual constexpr string_view getCssClass() const noexcept = 0;

	/**
	 * Creates a human readable name for the form.
	 *
	 * @return pretty name for the form.
	 */
	virtual const string createPrettyName() const;

	/**
	 * Creates a tooltip.
	 *
	 * @return the tooltip text.
	 */
	virtual const string createTooltip() const;

	/**
	 * Creates a unique ID for the form.
	 *
	 * @return unique ID out of current data.
	 */
	virtual const string createUniqueId() const;

	/**
	 * Alinas of value(primary key)
	 * @return a string with the primary key value or ""
	 */
	const string getPrimaryValue() const;

	/**
	 * Removes a value by key name.
	 * @param key
	 */
	virtual void unSet(const string& key);

	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @param defaultValue the default value if the key is not found.
	 * @return
	 */
	virtual string getValue(const string& key, const string& defaultValue = "") const;

	/**
	 * Allows the change of an internal value.
	 * @param key
	 * @param value
	 */
	virtual void setValue(const string& key, const string& value);

	/**
	 * @param number the copy number.
	 * @return a copy of the values with a new primary key.
	 */
	StringUMap copyValues(uint8_t number) const;

	/**
	 * @return a list of stored values
	 */
	const StringUMap* getValues() const;

	/**
	 * Replace values from a map.
	 *
	 * @param values
	 */
	void setValues(const StringUMap& values);

	/**
	 * Removes all data.
	 */
	virtual void wipe();

	/**
	 * Cleans other dependent data.
	 */
	virtual void reset();

	/**
	 * If the data owns dialogs, do any data handling setup.
	 */
	virtual void activate() {}

	/**
	 * Do any shutdown when the dialog finished handling data.
	 */
	virtual void deActivate() {}

	/**
	 * @brief Converts the data into XML string.
	 * @return The XML string representation of the box element.
	 */
	virtual const string toXML() const;

	/**
	 * Sets a property value.
	 *
	 * @param key Property name.
	 * @param value Property value.
	 */
	void setProperty(const string& key, const string& value);

	/**
	 * Gets a property value.
	 * @param key Property name.
	 * @param def Default value if not found.
	 * @return Property value or default.
	 */
	string getProperty(const string& key, const string& def = "") const;

	/**
	 * Checks if a property exists.
	 * @param key Property name.
	 * @return True if property exists.
	 */
	bool hasProperty(const string& key) const;

	/**
	 * Removes a property by key name.
	 * @param key
	 */
	void removeProperty(const string& key);

	/**
	 * @return All properties.
	 */
	const StringUMap& getProperties() const;

	/**
	 * @return an empty data object.
	 */
	static StringUMap& createEmptyData();

protected:

	/// Data pairs.
	StringUMap fieldsData;

	/// Extra property with important information.
	StringUMap properties;

	/// List of ignored data fields.
	mutable StringUSet ignored;

	/**
	 * Returns the field name used as the primary identifier for this data type.
	 *
	 * @return Field name to use as primary key (default: NAME).
	 */
	virtual const string getPrimaryKey() const;

	/**
	 * Organizes data vertically or horizontally based on the number of elements.
	 * @param ignored
	 * @param data
	 * @return
	 */
	static string valuesXML(
		const StringUSet& ignored,
		const StringUMap& data
	);

	/**
	 * Based on data it creates a single or multiple node.
	 * @param node
	 * @param data
	 * @param ignored
	 * @param empty
	 * @return
	 */
	static string createOpeningXML(
		const string& node,
		const StringUMap& data,
		const StringUSet& ignored,
		bool empty
	);

	/**
	 * @param node
	 * @return
	 */
	static string createClosingXML(const string& node);

};

using StringDataMap = std::map<string, Data*>;

} // namespace
