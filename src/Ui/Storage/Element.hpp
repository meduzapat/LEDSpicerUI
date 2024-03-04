/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
 * @since     Mar 16, 2023
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

#include "Data.hpp"
#include "Storage/CollectionHandler.hpp"

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::ElementForm
 */
class Element: public Data {

public:

	using Data::Data;

	virtual ~Element();

	const string createPrettyName() const override;

	const string createUniqueId() const override;

	const string getCssClass() const override;

	const string toXML() const override;

	static string getPinCssByData(uint8_t pin, Data* data);
};

} /* namespace */

#endif /* ELEMENT_HPP_ */
