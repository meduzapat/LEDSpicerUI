/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      NameOnly.cpp
 * @since     Apr 6, 2023
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

#include "NameOnly.hpp"

using namespace LEDSpicerUI::Ui::Storage;

const string NameOnly::createTooltip() const {
	if (tooltipFunction)
		return tooltipFunction(fieldsData);
	return Data::createTooltip();
}

const string NameOnly::createPrettyName() const {
	if (prettyNamedFunction)
		return prettyNamedFunction(fieldsData);
	return Data::createPrettyName();
}

const string NameOnly::createUniqueId() const {
	return getValue(NAME);
}

const string NameOnly::getCssClass() const {
	return cssClass;
}

const string NameOnly::toXML() const {
	string r("<" + node + "\n");
	Defaults::increaseTab();
	r += Data::toXML();
	Defaults::reduceTab();
	r += ">\n";
	return r;
}
