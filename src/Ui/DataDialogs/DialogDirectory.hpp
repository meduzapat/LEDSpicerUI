/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDirectory.hpp
 * @since     Mar 2, 2026
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

#include "DialogFileForm.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogDirectory
 * Utility dialog for creating and renaming directory nodes.
 * Acts as a shared utility across all navigator types (inputs, animations, profiles).
 * The active collection name must be set by the navigator before use via setCollectionName(),
 * keeping directory registries isolated per type.
 * Never serializes — directories are runtime-only.
 */
class DialogDirectory : public DialogFileForm, public SingletonDialog<DialogDirectory> {

	friend class Gtk::Builder;

public:

	virtual ~DialogDirectory() = default;

	/**
	 * Sets the collection name used by getCollectionHandler().
	 * Must be called by the navigator before wiring this dialog.
	 * @param name Collection name, e.g. "directories_inputs".
	 */
	void setCollectionName(const string& name);

	void load(XMLHelper* values) override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	void clearForm() override;
	const string createUniqueId() const override;
	const string getType() const override;

	Storage::CollectionHandler* getCollectionHandler() const override;

protected:

	/// Active collection name. Set by the navigator before use.
	string collectionName;

	/// Directory name input.
	Gtk::Entry* entryDirectoryName = nullptr;

	DialogDirectory(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	Storage::Data* createData(StringUMap& rawData) override;

};

} // namespace
