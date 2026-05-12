/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Values.cpp
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

#include "Values.hpp"

using namespace LEDSpicerUI;

bool Values::isSet(const string& key) const noexcept {
	return values.find(key) != values.end();
}

const string& Values::getValue(const string& key) const noexcept {
	if (auto it = values.find(key); it != values.end())
		return it->second;
	return emptyString;
}

string Values::getValue(const string& key, const string& defaultValue) const noexcept {
	if (auto it = values.find(key); it != values.end())
		return it->second;
	return defaultValue;
}

void Values::unSet(const string& key) noexcept {
	values.erase(key);
}

void Values::setValue(const string& key, const string& value) noexcept {
	values[key] = value;
}

void Values::setValues(const StringUMap& values) noexcept {
	this->values.insert(values.begin(), values.end());
}

Values Values::copyValues() const noexcept {
	StringUMap c {values.begin(), values.end()};
	return Values {c} ;
}

void Values::wipe() noexcept {
	values.clear();
}
