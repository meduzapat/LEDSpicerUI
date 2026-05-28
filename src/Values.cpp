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

#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>

using namespace LEDSpicerUI;

namespace {

bool parseDoubleStrict(const string& s, double& out) noexcept {
	if (s.empty()) return false;
	errno = 0;
	const char* begin = s.c_str();
	char* end = nullptr;
	const double v = std::strtod(begin, &end);
	if (end == begin or errno == ERANGE) return false;
	while (*end != '\0' and std::isspace(static_cast<unsigned char>(*end))) ++end;
	if (*end != '\0') return false;
	out = v;
	return true;
}

} // namespace

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

void Values::setValue(const string& key, const char* value)   noexcept { values[key] = value ? value : emptyString; }
void Values::setValue(const string& key, int value)           noexcept { values[key] = std::to_string(value); }
void Values::setValue(const string& key, unsigned value)      noexcept { values[key] = std::to_string(value); }
void Values::setValue(const string& key, long value)          noexcept { values[key] = std::to_string(value); }
void Values::setValue(const string& key, unsigned long value) noexcept { values[key] = std::to_string(value); }
void Values::setValue(const string& key, double value)        noexcept { values[key] = std::to_string(value); }
void Values::setValue(const string& key, bool value)          noexcept { values[key] = value ? HUMAN_TRUE : HUMAN_FALSE; }

bool Values::isNumber(const string& key) const noexcept {
	double tmp;
	return parseDoubleStrict(getValue(key), tmp);
}

int Values::getInt(const string& key) const noexcept {
	double v;
	if (not parseDoubleStrict(getValue(key), v)) return 0;
	if (v < static_cast<double>(std::numeric_limits<int>::min()) or
		v > static_cast<double>(std::numeric_limits<int>::max())) return 0;
	return static_cast<int>(v);
}

double Values::getDouble(const string& key) const noexcept {
	double v;
	return parseDoubleStrict(getValue(key), v) ? v : 0.00f;
}

void Values::setValues(const StringUMap& values) noexcept {
	this->values.insert(values.begin(), values.end());
}

Values Values::copyValues() const noexcept {
	return Values(values.begin(), values.end());
}

void Values::wipe() noexcept {
	values.clear();
}
