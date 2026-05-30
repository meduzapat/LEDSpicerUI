/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
 * @since     May 2026
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

#include "Actor.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Actor::Actor(Values& data, Data* group, const string& ownerId) noexcept :
	Link(data, ACTOR_GROUP, TYPE_ACTOR, {}, group)
{
	getProperties().setValue(UID, ++actorCounter);
	getProperties().setValue(PID, ownerId);
}

string Actor::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		getProperties().getValue(PID),
		getProperties().getValue(UID)
	});
}

string Actor::createPrettyName() const noexcept {
	const auto& type {getValue(TYPE)};
	const auto& group{getValue(ACTOR_GROUP)};
	if (type.empty()) return "Actor";
	if (group.empty()) return type;
	return type + " (" + group + ")";
}

string Actor::createTooltip() const noexcept {
	const auto& type {getValue(TYPE)};
	const auto& group{getValue(ACTOR_GROUP)};
	string r {type.empty() ? "Actor" : (type + " actor")};
	if (not group.empty())
		r += " on group " + group;
	return r;
}

CollectionHandler* Actor::getCollectionHandler() const noexcept {
	return CollectionHandler::getInstance(COLLECTION_ACTORS + getProperties().getValue(PID));
}
