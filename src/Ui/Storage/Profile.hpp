/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Sep 25, 2023
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

#include "Data.hpp"
#include "BoxButtonCollection.hpp"
#include "CollectionHandler.hpp"
#include "DataDialogs/DialogSelect.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Profile
 */
class Profile: public Data {

public:

	using Data::Data;

	Profile(StringUMap& data);

	virtual ~Profile();

	const string getCssClass() const override;
	void activate() override;
	const string toXML() const override;

protected:

	BoxButtonCollection
		alwaysOnElements,
		alwaysOnGroups,
		inputs,
		animations,
		startTransitions,
		endTransitions;

	unordered_map<string, BoxButtonCollection*> itemCollections {
		{TYPE_ELEMENT,          &alwaysOnElements},
		{"group",            &alwaysOnGroups},
		{"input",            &inputs},
		{"animation",        &animations},
		{"startTransitions", &startTransitions},
		{"endTransitions",   &endTransitions}
	};

	const string getPrimaryKey() const override;

};

} /* namespace */
