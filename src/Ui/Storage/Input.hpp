/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.hpp
 * @since     Sep 27, 2023
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

#include "FileData.hpp"
#include "DataDialogs/DialogInputSource.hpp"
//#include "DataDialogs/DialogInputLinkMaps.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Input
 * Represents a single input plugin configuration file.
 * Owns a collection of InputSource objects (one per hardware source),
 * and a collection of InputMapLinks that cross sources.
 * PATH and FILENAME are stored as properties and never serialized to XML.
 */
class Input: public FileData {

public:

	using FileData::FileData;

	virtual ~Input();

	const string createTooltip() const override;
	const string getCssClass() const override;
	void activate() override;
	const string toXML() const override;

protected:

	/// Keeps track of different input sources.
	BoxButtonCollection sources;

	/// Linked maps, these are used to link multiple input maps together.
	BoxButtonCollection linkedMaps;

};

} // namespace
