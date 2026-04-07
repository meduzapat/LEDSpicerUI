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
	 * @return a list of stored values
	 */
	const StringUMap* getValues() const noexcept;

	/**
	 * Replace values from a map.
	 *
	 * @param values
	 */
	virtual void setValues(const StringUMap& values) noexcept;

	/**
	 * @return a copy of the internal data but adding.
	 */
	virtual StringUMap copyValues() const noexcept;

	/**
	 * Clears all serializable fields.
	 */
	virtual void wipe() noexcept;

	StringUMap::iterator begin()        noexcept       { return values.begin();  }
	StringUMap::iterator end()          noexcept       { return values.end();    }
	StringUMap::const_iterator begin()  const noexcept { return values.begin();  }
	StringUMap::const_iterator end()    const noexcept { return values.end();    }
	StringUMap::const_iterator cbegin() const noexcept { return values.cbegin(); }
	StringUMap::const_iterator cend()   const noexcept { return values.cend();   }

protected:

	/// Data pairs.
	StringUMap values;

};

} // Namespace
