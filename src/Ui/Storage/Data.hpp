/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.hpp
 * @since     Feb 28, 2023
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

	virtual ~Data();

	/**
	 * @returns a CSS class that identifies the object and the data.
	 */
	virtual const string getCssClass() const = 0;

	/**
	 * Creates a human readable name for the form.
	 * @return pretty name for the form.
	 */
	virtual const string createPrettyName() const;

	/**
	 * Creates a tooltip.
	 * @return the tooltip text.
	 */
	virtual const string createTooltip() const;

	/**
	 * Creates a unique ID for the form.
	 * @return unique ID out of current data.
	 */
	virtual const string createUniqueId() const;

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
	void setValue(const string& key, const string& value);

	/**
	 * @param number the copy number.
	 * @return a copy of the values with a new primary key.
	 */
	StringUMap copyValues(uint8_t number) const;

	/**
	 * Returns a list of stored values.
	 * @return
	 */
	const StringUMap* getValues() const;

	/**
	 * Replace values from a map.
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
	 * Sets a parameter value.
	 * @param key Parameter name.
	 * @param value Parameter value.
	 */
	void setParameter(const string& key, const string& value);

	/**
	 * Gets a parameter value.
	 * @param key Parameter name.
	 * @param def Default value if not found.
	 * @return Parameter value or default.
	 */
	string getParameter(const string& key, const string& def = "") const;

	/**
	 * Checks if a parameter exists.
	 * @param key Parameter name.
	 * @return True if parameter exists.
	 */
	bool hasParameter(const string& key) const;

	/**
	 * @return All parameters.
	 */
	const StringUMap& getParameters() const;

protected:

	/// Data pairs.
	StringUMap fieldsData;

	/// Extra parameters with important information.
	StringUMap parameters;

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

} /* namespace */
