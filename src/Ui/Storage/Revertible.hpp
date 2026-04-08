/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Revertible.hpp
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

#include "CollectionHandler.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Revertible
 *
 * Pure mixin for objects that participate in the DialogFormHost EDIT lifecycle
 * and need safe cancel semantics when a type-selector combo changes mid-edit.
 *
 * Receives a reference to the consumer's own values and optionally its
 * children map at construction.
 * Consumers are responsible for calling clearSnap() inside their wipe() and
 * revert() inside their tearDown().
 *
 * Lifecycle:
 *   - DialogFormHost calls swap() on type switch — fields and children move into snapshot storage in one pass.
 *   - On APPLY: consumer calls clearSnap() then Data::wipe().
 *   - On CANCEL: consumer calls revert() from tearDown() before Data::tearDown().
 */
class Revertible {

public:

	/**
	 * @param fields   Reference to the consumer's values.
	 * @param children Pointer to the consumer's children map, or nullptr if none.
	 */
	Revertible(
		Values& fields,
		StringBoxButtonCollectionUMap* children = nullptr
	) noexcept;

	virtual ~Revertible();

	/**
	 * Moves fields and all child collections into snapshot storage.
	 * No-op if fields are empty or a snapshot already exists.
	 */
	virtual void snapshot();

	/**
	 * Restores fields and all child collections from snapshot.
	 * No-op if no snapshot is present.
	 */
	virtual void revert();

	/**
	 * Discards all snapshot storage without touching live fields or children.
	 * Call this from the consumer's wipe() before Data::wipe().
	 */
	void clearSnap() noexcept;

protected:

	/// Reference to the consumer's serializable field storage.
	Values& liveFields;

	/// Pointer to the consumer's children map. nullptr = no children.
	StringBoxButtonCollectionUMap* liveChildren;

	/// Snapshot of liveFields. Empty = no snapshot active.
	Values snapFields;

	/// Child collections paired with their snapshot storage.
	vector<std::pair<BoxButtonCollection*, BoxButtonCollection>> childrenSnaps;

};

} // namespace
