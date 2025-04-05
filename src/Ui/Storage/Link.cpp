/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Link.cpp
 * @since     Mar 23, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "Link.hpp"

using namespace LEDSpicerUI::Ui::Storage;

const string Link::getCssClass() const {
	return link->createPrettyName();
}

const string Link::createPrettyName() const {
	return link->createPrettyName();
}

const string Link::createTooltip() const {
	return link->createTooltip();
}

const string Link::createUniqueId() const {
	return link->createUniqueId();
}

string Link::getValue(const string &key, const string &defaultValue) const {
	if (key == this->key) {
		return link->getValue(key, defaultValue);
	}
	return Data::getValue(key, defaultValue);
}

const string Link::toXML() const {

	StringUMap values{{key, link->createUniqueId()}};
	values.insert(fieldsData.begin(), fieldsData.end());

	// Generate XML using Data's utility methods
	return createOpeningXML(type, values, ignored, true);
}
