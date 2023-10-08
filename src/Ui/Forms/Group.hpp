/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.hpp
 * @since     Mar 16, 2023
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

#include "../DialogSelectElements.hpp"
#include "../DialogColors.hpp"

#include "Form.hpp"

#ifndef GROUP_HPP_
#define GROUP_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::ElementForm
 */
class Group: public Form {

public:

	struct GroupFields {
		Gtk::Entry*  InputGroupName       = nullptr;
		Gtk::Button* BtnGroupDefaultColor = nullptr;
	};

	Group(unordered_map<string, string>& data);

	virtual ~Group();

	virtual const string createPrettyName() const;

	static void initialize(GroupFields& groupFields);

	virtual void resetForm(Modes mode);

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

protected:

	static GroupFields groupFields;

	static CollectionHandler
		* groupHandler,
		* elementHandler;

	BoxButtonCollection elements;

	virtual void onActivate();

	virtual void onDeActivate();
};

} /* namespace */

#endif /* GROUP_HPP_ */
