/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.hpp
 * @since     Feb 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "Storage/Group.hpp"
#include "DialogSelectElements.hpp"

#ifndef GROUPDIALOG_HPP_
#define GROUPDIALOG_HPP_ 1

#define groupCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_GROUP)

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

	/**
	 * Instantiate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogGroup* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogGroup* instance;

	Gtk::Entry*  inputGroupName       = nullptr;
	Gtk::Button* btnGroupDefaultColor = nullptr;

	DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;
};

} /* namespace */

#endif /* GROUPDIALOG_HPP_ */
