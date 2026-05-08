/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMapLink.cpp
 * @since     Feb 6, 2024
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

#include "InputMapLink.hpp"

using namespace LEDSpicerUI::Ui::Storage;

InputMapLink::InputMapLink(StringUMap& data, const string& inputPid) noexcept :
	Parent(data, vector<string>{COLLECTION_INPUT_MAP_LINKS})
{
	getProperties().setValue(PID, inputPid);
	registerDependency(COLLECTION_INPUT_MAPS, COLLECTION_INPUT_MAP_LINKS);
	// Load data only.
	getProperties().setValue(LINKED_ITEMS, getValue(LINKED_ITEMS));
}

string InputMapLink::createPrettyName() const noexcept {
	const auto size {primaryChild->getSize()};
	if (not size) return "Empty";
	return "Linked " + std::to_string(size) + " Maps";
}

string InputMapLink::createTooltip() const noexcept {
	if (primaryChild->getSize() == 0)
		return "No maps linked.";

	StringVector parts;
	for (auto btn : *primaryChild)
		parts.push_back(btn->getData()->createPrettyName());

	return "Starts with " + Defaults::implode(parts, ", ") + " will start over";
}

string InputMapLink::toXML() const noexcept {
	const string pid{getProperties().getValue(PID)};
	StringVector indexes;
	size_t idx{0};
	for (const auto& [id, data] : *CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)) {
		if (data->getProperties().getValue(IID) == pid) {
			if (primaryChild->isSet(data))
				indexes.push_back(std::to_string(idx));
			++idx;
		}
	}
	return Defaults::implode(indexes, ",");
}
