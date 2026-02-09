/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputLinkMaps.hpp
 * @since     Oct 1, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

#include "DialogForm.hpp"
#include "Storage/InputMapLink.hpp"
#include "Storage/CollectionHandler.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogInputLinkElements
 * This Dialog handles the linking of maps logic from input dialog, it also provide sorting functionality for linked maps.
 * All the dialog functionality is for the sorting mechanism, that is a FAKE data generator.
 * The real link is created when 2 or more maps are selected in input dialog and the add button is clicked.
 * Is a very simple and a little nasty way to handle the links.
 */
class DialogInputLinkMaps: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogInputLinkMaps() = delete;

	virtual ~DialogInputLinkMaps();

	static void initialize(const Glib::RefPtr<Gtk::Builder> &builder);
	static DialogInputLinkMaps* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;
	void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner = nullptr) override;

protected:

	/// Self instance.
	static DialogInputLinkMaps* instance;

	/// Linked maps are per input.
	static StringVector localCollection;

	/// Where the temporary input mappings will be displayed and sorted.
	OrdenableFlowBox* boxInputLinkedMappings = nullptr;

	Storage::BoxButtonCollection indivitualMaps;

	DialogInputLinkMaps(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;

	/**
	 * To remove used linked maps.
	 * @param boxButton
	 */
	void afterDeleteConfirmation(Storage::BoxButton& boxButton) override;

	/**
	 * Checks if a group of IDs is used on the local collection.
	 * @param id
	 * @return
	 */
	bool isUsed(const string& ids) const;

	/**
	 * Extracts the Ids from the link data field.
	 * @param data
	 * @return
	 */
	string extractIds(Storage::Data* data) const;

};

} /* namespace */
