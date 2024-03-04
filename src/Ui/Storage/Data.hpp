/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.hpp
 * @since     Feb 28, 2023
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

#include "Message.hpp"

#ifndef FORM_HPP_
#define FORM_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Data
 *
 * Data will store information and create a small box with a label and to optional buttons, edit and close.
 */
class Data: public Gtk::VBox {

public:

	Data() = delete;

	/**
	 * Creates an object pre-populated.
	 * @param data
	 */
	Data(unordered_map<string, string>& data);

	virtual ~Data();

	/**
	 * Provides a CSS class that identifies the object and the data.
	 */
	virtual const string getCssClass() const = 0;

	/**
	 * Creates a pretty name for the form.
	 * @return
	 */
	virtual const string createPrettyName() const;

	/**
	 * Creates a tooltip.
	 * @return
	 */
	virtual const string createTooltip() const;

	/**
	 * Returns a unique ID out of current data.
	 * @return
	 */
	virtual const string createUniqueId() const = 0;

	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @param defaultValue
	 * @return
	 */
	string getValue(const string& key, const string& defaultValue = "") const;

	/**
	 * Allows the change of an internal value.
	 * @param key
	 * @param value
	 */
	void setValue(const string& key, const string& value);

	/**
	 * Similar to set but to be used to replace an old pre-existing value.
	 * @param key
	 * @param oldValue
	 * @param newValue
	 * @throw std::out_of_range if the value is not existing.
	 */
	void replaceValue(const string& key, const string& newValue);

	/**
	 * Returns a list of stored values.
	 * @return
	 */
	const unordered_map<string, string>* getValues() const;

	/**
	 * Removes all data.
	 */
	virtual void wipe();

	/**
	 * Removes any internal property, similar to the destructor but keeps the ownership.
	 */
	virtual void destroy() {};

	/**
	 * Do any preparation when the dialog starts data handling.
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

protected:

	/// Data pairs.
	unordered_map<string, string> fieldsData;

	/// List of ignored data fields.
	vector<string> ignored;

	/**
	 *
	 * @param ignored
	 * @param data
	 * @return
	 */
	static string valuesXML(const vector<string>& ignored, const unordered_map<string, string>& data);

	/**
	 *
	 * @param node
	 * @param data
	 * @param ignored
	 * @param empty
	 * @return
	 */
	static string createOpeningXML(const string& node, const unordered_map<string, string>& data, const vector<string>& ignored, bool empty);

	/**
	 *
	 * @param node
	 * @return
	 */
	static string createClosingXML(const string& node);

};

} /* namespace */

#endif /* FORM_HPP_ */
