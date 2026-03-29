/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Revertible.cpp
 * @since     Mar 2026
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

#include "Revertible.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Revertible::Revertible(
	StringUMap& data,
	const string& collectionId,
	StringBoxButtonCollectionUMap children
) :
	Parent(data, collectionId, std::move(children))
{
	for (auto& child : this->children) registerChild(child.second);
}

Revertible::~Revertible() {
	snapFields.clear();
	for (auto& [live, snap] : childrenSnaps)
		snap.wipe();
}

void Revertible::swap() {
	if (fieldsData.empty() or not snapFields.empty()) return;
	for (auto& [id, collection] : children)
		for (auto btn : collection)
			btn->getData()->getCollectionHandler()->remove(btn->getData());
	std::swap(fieldsData, snapFields);
	for (auto& [live, snap] : childrenSnaps)
		live->swap(snap);
}

void Revertible::revert() {
	if (snapFields.empty()) return;
	std::swap(fieldsData, snapFields);
	snapFields.clear();
	for (auto& [live, snap] : childrenSnaps) {
		live->swap(snap);
		snap.wipe();
	}
	for (auto& [id, collection] : children)
		for (auto btn : collection)
			btn->getData()->getCollectionHandler()->add(btn->getData());
}

void Revertible::wipe() {
	snapFields.clear();
	for (auto& [live, snap] : childrenSnaps)
		snap.wipe();
	Data::wipe();
}

void Revertible::tearDown() {
	revert();
	Data::tearDown();
}

void Revertible::registerChild(BoxButtonCollection& child) {
	childrenSnaps.emplace_back(&child, BoxButtonCollection{});
}
