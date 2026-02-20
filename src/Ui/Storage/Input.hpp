/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.hpp
 * @since     Sep 27, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

#include "DataDialogs/DialogInputLinkMaps.hpp"
#include "DataDialogs/DialogInputMap.hpp"
#include "BoxButtonCollection.hpp"
#include "Data.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Input
 *
 * This class have maps, optional events and event maps.
 * Normally
 */
class Input: public Data {

public:

	Input(StringUMap& data);

	virtual ~Input();

	const string createPrettyName() const override;
	const string createTooltip() const override;
	const string getCssClass() const override;
	void activate() override;
	const string toXML() const override;

protected:

	/// Maps, where 0 is used for single source input plugins.
	vector<BoxButtonCollection> maps;

	/// Keeps track of different input sources.
	BoxButtonCollection listeners;

	/// Linked maps, these are used to link multiple input maps together.
	BoxButtonCollection linkedMaps;

	const string getPrimaryKey() const override;

};

} // namespace
