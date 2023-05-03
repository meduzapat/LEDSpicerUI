/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      NameOnly.hpp
 * @since     Apr 6, 2023
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

#include "Form.hpp"

#ifndef NAMEONLY_HPP_
#define NAMEONLY_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::NameOnly
 */
class NameOnly: public Form {

public:

	NameOnly() = default;

	NameOnly(unordered_map<string, string>& data, const string& cssClass = "") :
		Form(data),
		cssClass(cssClass) {}

	virtual ~NameOnly() = default;

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode) {}

	virtual void retrieveData(Modes mode) {}

	virtual void cancelData(Modes mode) {}

	virtual const string getCssClass() const;

	virtual const string createPrettyName() const;

	virtual bool canEdit() const;

protected:

	const string cssClass;

};

} /* namespace */


#endif /* NAMEONLY_HPP_ */
