/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Link.hpp
 * @since     Mar 23, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#ifndef SRC_UI_STORAGE_LINK_HPP_
#define SRC_UI_STORAGE_LINK_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Link
 * Class that links other Data classes as values.
 */
class Link: public Data {

public:

	using Data::Data;

	Link(StringUMap& data, const string& type, const string &key, Data *link) : Data(data), type(type), key(key), link(link) {}

	virtual ~Link() = default;

	const string getCssClass() const override;

	const string createPrettyName() const override;

	const string createTooltip() const override;

	const string createUniqueId() const override;

	string getValue(const string& key, const string& defaultValue = "") const override;

	const string toXML() const override;

protected:

	const string
		type,
		key;

	/// Links a parameter name with a Data object.
	Data* link;

};

} /* namespace */

#endif /* SRC_UI_STORAGE_LINK_HPP_ */
