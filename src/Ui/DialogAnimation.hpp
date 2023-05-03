/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogAnimation.hpp
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

#ifndef UI_DIALOGANIMATION_HPP_
#define UI_DIALOGANIMATION_HPP_ 1

#include "DialogForm.hpp"

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DialogAnimation
 * Dialog to create and edit animations.
 */
class DialogAnimation: public DialogForm {

public:

	using DialogForm::DialogForm;

	virtual ~DialogAnimation() = default;

	void resetFields();

	void readFields();

	void setFields(unordered_map<string, string> parameters);

	unordered_map<string, string> readFields();

};

} /* namespace */

#endif /* UI_DIALOGANIMATION_HPP_ */
