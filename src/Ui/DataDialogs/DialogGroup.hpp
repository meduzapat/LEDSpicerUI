/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.hpp
 * @since     Feb 13, 2023
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
#include "Storage/Group.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::DialogGroup
 * Dialog to create or edit groups.
 */
class DialogGroup: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogGroup() = delete;

	virtual ~DialogGroup() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);
	static DialogGroup* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void isValid() const override;
	void clearForm() override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

protected:

	static DialogGroup* instance;

	Gtk::Entry*  inputGroupName       = nullptr;
	Gtk::Button* btnGroupDefaultColor = nullptr;

	/// The box where group elements are displayed.
	OrdenableFlowBox* boxElements = nullptr;

	const DialogSelect::SettingRequest groupElementsSetting {
		boxElements,
		NAME,
		TYPE_ELEMENT,
		COLLECTION_ELEMENT,
		DialogSelect::BUTTON_DELETER,
	};

	DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;
};

} /* namespace */
