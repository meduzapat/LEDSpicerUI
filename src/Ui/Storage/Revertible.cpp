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

Revertible::Revertible(Values& fields, StringBoxButtonCollectionUMap& children) noexcept :
	liveFields(fields),
	liveChildren(children)
{
	for (auto& [id, col] : liveChildren)
		childrenSnaps.emplace_back(&col, BoxButtonCollection{});
}

Revertible::~Revertible() {
	clearSnap();
}

void Revertible::snapshot() {

	if (liveFields.getValues().empty() or not snapFields.getValues().empty()) return;

	for (auto& [id, col] : liveChildren) {
		for (auto btn : col) {
			btn->getData()->freeze();
		}
	}

	liveFields.swap(snapFields);
	for (auto& [live, snap] : childrenSnaps)
		live->swap(snap);
}

void Revertible::revert() {

	if (snapFields.getValues().empty()) return;

	liveFields.swap(snapFields);
	snapFields.wipe();

	for (auto& [live, snap] : childrenSnaps) {
		live->swap(snap);
		snap.wipe(); // replacers destroyed, their slots erased from handler.
		for (auto btn : *live) {
			auto data{btn->getData()};
			data->unfreeze();
			data->registerToCollection();
		}
	}
}

void Revertible::clearSnap() noexcept {
	snapFields.wipe();
	for (auto& [live, snap] : childrenSnaps)
		snap.wipe();
}
