/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Revertible.hpp
 * @since     Mar 2026
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
 * ...
 */

#include "Data.hpp"
#include "BoxButtonCollection.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Revertible
 *
 * Intermediate base for Data subclasses that participate in the DialogFormHost
 * EDIT lifecycle and need safe cancel semantics when the type-selector combo
 * is changed mid-edit.
 *
 * Subclasses call registerChild() once per owned BoxButtonCollection from
 * their constructor body. Revertible then handles swap and restore of both
 * fieldsData and all registered children automatically.
 *
 * Lifecycle:
 *   - DialogFormHost calls swap() just before onEmpty()/onSelected() on a
 *     type switch. fieldsData and all child collections are swapped into
 *     snapshot storage in one O(n) pass.
 *   - On APPLY, DialogForm calls wipe() before storeData(), which discards
 *     all snapshot storage as a side effect.
 *   - On CANCEL, DialogForm calls deActivate() without having called wipe().
 *     deActivate() detects the live snapshot and calls restore(), returning
 *     fieldsData and all child collections to their pre-edit state.
 */
class Revertible : public Data {

public:

	virtual ~Revertible() = default;

	/**
	 * Swaps fieldsData and all registered child collections into snapshot storage.
	 * No-op if fieldsData is empty or a snapshot already exists.
	 */
	virtual void swap();

	/**
	 * Restores fieldsData and all registered child collections from snapshot.
	 * No-op if no snapshot is present.
	 */
	virtual void restore();

	/**
	 * Clears fieldsData and discards all snapshot storage.
	 * This is the APPLY path — called by DialogForm before storeData().
	 */
	void wipe() override;

	/**
	 * Restores snapshot if present, then delegates to Data::deActivate().
	 * This is the CANCEL path — called by DialogForm after the modal loop.
	 */
	void deActivate() override;

protected:

	using Data::Data;

	/**
	 * Registers a child BoxButtonCollection to be included in swap/restore.
	 * Must be called from the subclass constructor body, after all members
	 * are initialized.
	 * @param child The child collection to register.
	 */
	void registerChild(BoxButtonCollection& child);

private:

	/// Snapshot of fieldsData. Empty means no snapshot is active.
	StringUMap snapFields;

	/// Registered children paired with their snapshot storage.
	vector<std::pair<BoxButtonCollection*, BoxButtonCollection>> children;

};

} // namespace
