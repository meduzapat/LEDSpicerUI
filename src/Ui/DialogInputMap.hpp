/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputMap.hpp
 * @since     Sep 30, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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
#include "Forms/CollectionHandler.hpp"
#include "Forms/InputMap.hpp"

#ifndef UI_DIALOGINPUTMAP_HPP_
#define UI_DIALOGINPUTMAP_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogInputMap
 */
class DialogInputMap: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogInputMap() = delete;

	virtual ~DialogInputMap() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	static DialogInputMap* getInstance();

//	virtual string toXml();

protected:

	static DialogInputMap* instance;

	DialogInputMap(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	virtual string getType();

	virtual Forms::Form* getForm(unordered_map<string, string>& rawData);
};

} /* namespace */

#endif /* UI_DIALOGINPUTMAP_HPP_ */
