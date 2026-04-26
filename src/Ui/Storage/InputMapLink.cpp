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
	Parent(data, vector<string>{LINKED_ITEMS})
{
	getProperties().setValue(PID, inputPid);
	// Preserve an existing ILM_ID (loaded from XML); otherwise generate a new one.
	if (getValue(ILM_ID).empty())
		setValue(ILM_ID, Defaults::createCommonUniqueId({inputPid, std::to_string(++linkMapCounter)}));
}

string InputMapLink::createPrettyName() const noexcept {
	StringVector names;
	for (auto btn : *primaryChild)
		names.push_back(btn->getData()->createPrettyName());
	return names.empty() ? "Empty" : Defaults::implode(names, " ➡️ ") + " 🔙";
}

string InputMapLink::createTooltip() const noexcept {
	for (auto btn : *primaryChild)
		txts.push_back(btn->getData()->createTooltip());
	if (txts.empty()) return "No maps linked.";
	return "Sequence: " + Defaults::implode(txts, " → ");
}

CollectionHandler* InputMapLink::getCollectionHandler() const noexcept {
	return CollectionHandler::getInstance(COLLECTION_INPUT_LINKMAPS + getProperties().getValue(PID));
}

void InputMapLink::wipe() noexcept {
	// Preserve the stable identity through wipe/storeData cycles.
	string id(getValue(ILM_ID));
	Data::wipe();
	if (not id.empty())
		setValue(ILM_ID, id);
}

string InputMapLink::createUniqueId() const noexcept {
	// ID holds the canonical trigger combination key built by DialogInputLinkMaps.
	return getValue(ID);
}
