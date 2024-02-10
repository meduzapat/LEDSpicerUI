/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMapLink.hpp
 * @since     Feb 6, 2024
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2024 Patricio A. Rossi (MeduZa)
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

#include "Data.hpp"
#include "CollectionHandler.hpp"

#ifndef INPUTMAPLINK_HPP_
#define INPUTMAPLINK_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::InputMapLink
 */
class InputMapLink: public Data {

public:

	using Data::Data;

	virtual ~InputMapLink() = default;

	const string createPrettyName() const override;

	const string createUniqueId() const override;

	const string getCssClass() const override;

	const string createTooltip() const override;
};

} /* namespace */


#endif /* INPUTMAPLINK_HPP_ */
