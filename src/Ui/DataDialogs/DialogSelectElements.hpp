/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelectElements.hpp
 * @since     Feb 22, 2023
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
#include "Storage/CollectionHandler.hpp"
#include "Storage/NameOnly.hpp"

#ifndef DIALOGSELECTELEMENTS_HPP_
#define DIALOGSELECTELEMENTS_HPP_ 1

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogSelect
 * Handles the selection of already created elements by dialog elements.
 */
class DialogSelectElements: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogSelectElements() = delete;

	virtual ~DialogSelectElements() = default;

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
	static DialogSelectElements* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override {};

	void clearFormOthers();

	void isValid() const override {};

	void storeData() override {}

	void retrieveData() override {}

	const string createUniqueId() const override {return "";}

	const size_t getNumberOfSelectedElements() const;

	const size_t getNumberElements() const;

protected:

	/// Self instance.
	static DialogSelectElements* instance;

	Gtk::FlowBox* boxAllElements = nullptr;

	DialogSelectElements(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	void populateElements();

	bool elementExist(const string& name);

};

} /* namespace */

#endif /* DIALOGSELECTELEMENTS_HPP_ */
