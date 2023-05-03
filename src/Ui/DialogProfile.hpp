/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.hpp
 * @since     Feb 14, 2023
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

#ifndef UI_DIALOGPROFILE_HPP_
#define UI_DIALOGPROFILE_HPP_ 1

#include "DialogForm.hpp"

namespace LEDSpicerUI {
namespace Ui {

/**
 * LEDSpicerUI::Ui::DialogProfile
 * Dialog to create or edit profiles.
 */
class DialogProfile: public DialogForm {

public:

	using DialogForm::DialogForm;

	virtual ~DialogProfile() = default;

	void resetFields();

	void setFields(unordered_map<string, string> parameters);

	unordered_map<string, string> readFields();

};

} /* namespace Ui */
} /* namespace LEDSpicerUI */

#endif /* UI_DIALOGPROFILE_HPP_ */
