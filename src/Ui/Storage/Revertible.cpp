/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file   Revertible.cpp
 * @since  Mar 2026
 * @author Patricio A. Rossi (MeduZa)
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

#include "Revertible.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Revertible::Revertible(
	StringUMap& fields,
	StringBoxButtonCollectionUMap* children
) noexcept :
	liveFields(fields),
	liveChildren(children)
{
	if (not liveChildren) return;
	for (auto& [id, collection] : *liveChildren)
		childrenSnaps.emplace_back(&collection, BoxButtonCollection{});
}

Revertible::~Revertible() {
	clearSnap();
}

void Revertible::swap() {

	if (liveFields.empty() or not snapFields.empty()) return;

	if (liveChildren) {
		for (auto& [id, collection] : *liveChildren) {
			for (auto btn : collection) {
				if (auto handler{btn->getData()->getCollectionHandler()}; handler) handler->remove(btn->getData());
			}
		}
	}

	std::swap(liveFields, snapFields);
	for (auto& [live, snap] : childrenSnaps)
		live->swap(snap);
}

void Revertible::revert() {

	if (snapFields.empty()) return;

	std::swap(liveFields, snapFields);
	snapFields.clear();
	for (auto& [live, snap] : childrenSnaps) {
		live->swap(snap);
		snap.wipe();
	}

	if (liveChildren) {
		for (auto& [id, collection] : *liveChildren) {
			for (auto btn : collection)
				if (auto handler{btn->getData()->getCollectionHandler()}; handler) handler->add(btn->getData());
		}
	}
}

void Revertible::clearSnap() noexcept {
	snapFields.clear();
	for (auto& [live, snap] : childrenSnaps)
		snap.wipe();
}
