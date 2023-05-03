/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogGroup.hpp
 * @since     Feb 13, 2023
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

#include "Forms/Group.hpp"

#ifndef GROUPDIALOG_HPP_
#define GROUPDIALOG_HPP_ 1

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::DialogGroup
 * Dialog to create or edit groups.
 */
class DialogGroup: public DialogForm {

public:

	DialogGroup() = delete;

	DialogGroup(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	virtual ~DialogGroup() = default;

protected:

	DialogSelectElements* dialogSelectElements;

	virtual string getType();

	virtual Forms::Form* getForm(unordered_map<string, string>& rawData);
};

} /* namespace */

#endif /* GROUPDIALOG_HPP_ */
