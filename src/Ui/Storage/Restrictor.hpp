/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.hpp
 * @since     Feb 26, 2023
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

#include "Parent.hpp"
#include "Revertible.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Restrictor
 * Stores a hardware restrictor and its player mappings.
 */
class Restrictor : public Parent, public Revertible {

public:

	Restrictor(StringUMap& data) noexcept :
		Parent(data, {COLLECTION_RESTRICTOR_MAP}),
		Revertible(*this, children)
	{}

	virtual ~Restrictor() = default;

	string createPrettyName() const noexcept override;
	string createUniqueId()   const noexcept override;
	string_view getCssClass()       const noexcept override { return "RestrictorBoxButton"; }
	string_view getXmlTag()         const noexcept override { return "restrictor"; }

	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance(COLLECTION_RESTRICTORS);
	}

	void wipe()     noexcept override;
	void tearDown() noexcept override;

};

} // namespace
