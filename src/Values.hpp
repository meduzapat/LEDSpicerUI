/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Values.hpp
 * @since     Apr 6, 2026
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

namespace LEDSpicerUI {

/**
 * Wrapper class over a unordered map of strings -> strings to add generic functionality.
 */
class Values {

public:

	Values() = default;

	/**
	 * Creates an object pre-populated.
	 * @param data Iten's data.
	 */
	Values(StringUMap& data) noexcept : values(std::move(data)) {}

	virtual ~Values() = default;

	bool isSet(const string& key) const noexcept;

	/**
	 * Removes a value by key name.
	 * @param key
	 */
	virtual void unSet(const string& key) noexcept;

	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @return value or empty string if not found.
	 */
	virtual const string& getValue(const string& key) const noexcept;

	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @param defaultValue the default value if the key is not found.
	 * @return value or default.
	 */
	virtual string getValue(const string& key, const string& defaultValue) const noexcept;

	/**
	 * Allows the change of an internal value.
	 * @param key
	 * @param value
	 */
	virtual void setValue(const string& key, const string& value) noexcept;

	/**
	 * Compares a value against another value
	 *
	 * @param key
	 * @return true if the stored value in key == value
	 */
	bool isA(const string& key, const string& value) const noexcept { return getValue(key) == value; }

	/**
	 * Verify than a value is "True"
	 *
	 * @param key
	 * @return true if the value == "True"
	 */
	bool is(const string& key) const noexcept { return isA(key, HUMAN_TRUE); }

	/**
	 * @return a list of stored values
	 */
	const StringUMap& getValues() const noexcept { return values; }

	/**
	 * Replace values from a map.
	 *
	 * @param values
	 */
	void setValues(const StringUMap& values) noexcept;

	/**
	 * Sets the value in key to true
	 *
	 * @param key
	 */
	void setTrue(const string& key) noexcept { setValue(key, HUMAN_TRUE); }

	/**
	 * @return a copy of the internal data.
	 */
	virtual StringUMap copyValues() const noexcept { return {values}; }

	/**
	 * Clears all serializable fields.
	 */
	virtual void wipe() noexcept;

	auto begin()        noexcept { return values.begin();  }
	auto end()          noexcept { return values.end();    }
	auto begin()  const noexcept { return values.begin();  }
	auto end()    const noexcept { return values.end();    }
	auto cbegin() const noexcept { return values.cbegin(); }
	auto cend()   const noexcept { return values.cend();   }

	/**
	 * Swaps the internal data with another Values object.
	 * @param other the other Values object to swap with.
	 */
	void swap(Values& other) noexcept { std::swap(values, other.values); }

protected:

	/// Data pairs.
	StringUMap values;

};

} // Namespace
