/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSource.hpp
 * @since     Feb 20, 2026
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "DataDialogs/DialogInputMap.hpp"
#include "Revertible.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::InputSource
 * Represents a single input source inside Input.
 * Owns the maps that belong to this source.
 */
class InputSource : public Revertible {

public:

	InputSource(StringUMap& data);

	virtual ~InputSource();

	const string createUniqueId() const override;
	const string createPrettyName() const override;
	const string createTooltip() const override;
	string_view getCssClass() const noexcept override;
	void reset() override;
	void activate() override;
	const string toXML() const override;

protected:

	/// Maps owned by this source.
	BoxButtonCollection maps;

	/// Counter for sId generation.
	inline static size_t sourceCounter = 0;

	const string getPrimaryKey() const override;

};

} // namespace
