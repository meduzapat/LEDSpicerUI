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

#include <vector>
using std::vector;

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <string>
using std::string;
using std::stringstream;

#include <functional>

#pragma once

namespace LEDSpicerUI {

using StringMap    = std::map<string, string>;
using StringUMap   = std::unordered_map<string, string>;
using StringVector = std::vector<string>;
using StringUSet   = std::unordered_set<string>;
using StringSet    = std::set<string>;

namespace Constants {

/// Common values used in configs.
inline const string

	emptyString,
	emptyNumber  {"0"},
	DEFAULT      {"default"},
	NAME         {"name"}, // Generic parameter
	HUMAN_TRUE   {"True"},
	HUMAN_FALSE  {"False"},
	HUMAN_ON     {"On"},
	HUMAN_OFF    {"Off"},
	HUMAN_NORMAL {"Normal"},
	HUMAN_RANDOM {"Random"};
}

using namespace Constants;

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
	Values(Values& other)    noexcept : values(std::move(other.values)) {}
	Values(Values&& other)   noexcept : values(std::move(other.values)) {}

	Values& operator=(Values&& other) noexcept {
		values = std::move(other.values);
		return *this;
	}

	Values(StringUMap::const_iterator first, StringUMap::const_iterator last)
		: values(first, last) {}

	Values(std::initializer_list<std::pair<std::string, std::string>> init)
		: values(init.begin(), init.end())
	{}

	Values(const Values&) = delete;
	Values& operator=(const Values&) = delete;

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
	 * @return true if the value parses as a number.
	 */
	bool isNumber(const string& key) const noexcept;

	/**
	 * @return value parsed as int, or 0 if missing, empty, unparseable, or out of int range.
	 */
	int getInt(const string& key) const noexcept;

	/**
	 * @return value parsed as double, or 0.00 if missing, empty, or unparseable.
	 */
	double getDouble(const string& key) const noexcept;

	/**
	 * Allows the change of an internal value.
	 * @param key
	 * @param value
	 */
	virtual void setValue(const string& key, const string& value) noexcept;

	/**
	 * String-literal overload, avoids resolving to setValue(bool).
	 */
	void setValue(const string& key, const char* value)       noexcept;

	/**
	 * Typed overloads, stringify the value into the map.
	 */
	void setValue(const string& key, int value)           noexcept;
	void setValue(const string& key, unsigned value)      noexcept;
	void setValue(const string& key, long value)          noexcept;
	void setValue(const string& key, unsigned long value) noexcept;
	void setValue(const string& key, double value)        noexcept;
	void setValue(const string& key, bool value)          noexcept;

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
	 * @return the number of stored values
	 */
	size_t getSize() const noexcept { return values.size(); }

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
	virtual Values copyValues() const noexcept;

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
