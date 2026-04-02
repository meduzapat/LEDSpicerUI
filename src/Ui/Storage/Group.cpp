/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.cpp
 * @since     Mar 16, 2023
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

#include "Group.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Group::Group(StringUMap& data) noexcept :
	Parent(data, COLLECTION_GROUP, {COLLECTION_GROUP_LINKS})
{
	registerDependency(COLLECTION_ELEMENT, COLLECTION_GROUP_LINKS, 1, [this]() {
		getCollectionHandler()->remove(this);
	});
}

bool Group::shouldSerialize(const string& key, const string& value) const noexcept {
	return not (key == DEFAULT_COLOR and value.empty());
}
