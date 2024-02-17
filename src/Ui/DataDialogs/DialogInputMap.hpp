/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.hpp
 * @since     Sep 30, 2023
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

#include "DialogForm.hpp"
#include "DialogColors.hpp"
#include "Storage/InputMap.hpp"

#ifndef UI_DIALOGINPUTMAP_HPP_
#define UI_DIALOGINPUTMAP_HPP_ 1

#define inputMapCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)
#define inputLinkMapCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_INPUT_LINKED_MAPS)
#define mapElementCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)
#define mapGroupCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_GROUP)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogInputMap
 * This dialog will control the input maps, is directly related to
 * linked maps, and changes here need to be updated there.
 */
class DialogInputMap: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogInputMap() = delete;

	virtual ~DialogInputMap() = default;

	/**
	 * Instanciate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogInputMap* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogInputMap* instance;

	Gtk::ComboBoxText
		* comboBoxInputMapElement = nullptr,
		* comboBoxInputMapGroup   = nullptr,
		* comboBoxInputMapFilter  = nullptr;
	Gtk::Entry*  inputInputMapTrigger = nullptr;
	Gtk::Button* inputMapDefaultColor = nullptr;
	Gtk::Stack*  stackElementAndGroup = nullptr;

	DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

};

} /* namespace */

#endif /* UI_DIALOGINPUTMAP_HPP_ */
