/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Transition.hpp
 * @since     Jun 2026
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

#include "Data.hpp"
#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Transition
 *
 * A single per-profile transition record.
 * Owned by Profile by value; an empty NAME means "no transition" (instant switch).
 */
class Transition : public Data {

public:

	Transition() noexcept = default;

	Transition(Values& data) noexcept : Data(data) {}

	virtual ~Transition() = default;

	string createPrettyName()   const noexcept override;
	string toXML()              const noexcept override;
	const string& getCssClass() const noexcept override { return emptyString; }
	const string& getXmlTag()   const noexcept override { return TYPE_TRANSITION; }

	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }

	/**
	 * @return true when no transition is selected.
	 */
	bool isNone() const noexcept { return getValue(NAME).empty(); }

};

} // namespace
