/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSource.cpp
 * @since     Feb 20, 2026
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

#include "InputSource.hpp"

using namespace LEDSpicerUI::Ui::Storage;

InputSource::InputSource(StringUMap& data, const string& ownerId) noexcept :
	Parent(data, {COLLECTION_INPUT_MAPS}),
	Revertible(*this, children)
{
	getProperties().setValue(UID, std::to_string(++sourceCounter));
	getProperties().setValue(PID, ownerId);
	registerDependency(COLLECTION_ELEMENTS, COLLECTION_INPUT_MAPS);
	registerDependency(COLLECTION_GROUPS,   COLLECTION_INPUT_MAPS);
}

string InputSource::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({getProperties().getValue(PID), getValue(SOURCE)});
}

string InputSource::createPrettyName() const noexcept {
	return getProperties().getValue(NAME);
}

string InputSource::createTooltip() const noexcept {
	return "Source " + createPrettyName() + " with "
		+ std::to_string(children.at(COLLECTION_INPUT_MAPS).getSize()) + " maps";
}

CollectionHandler* InputSource::getCollectionHandler() const noexcept {
	return CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES + getProperties().getValue(PID));
}

void InputSource::wipe() noexcept {
	clearSnap();
	Data::wipe();
}

void InputSource::tearDown() noexcept {
	revert();
	Data::tearDown();
}

bool InputSource::shouldSerialize(const string& key, const string& value) const noexcept {
	return not (key == SOURCE and value.empty());
}
