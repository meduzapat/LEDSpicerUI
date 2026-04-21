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

#include "Parent.hpp"
#include "DirNode.hpp"
#include "Revertible.hpp"
// TODO: wire once DialogInputLinkMaps is reworked.
//#include "DataDialogs/DialogInputLinkMaps.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Input
 * Represents a single input configuration file.
 * Owns collections of InputSource objects and linked maps.
 */
class Input : public Parent, public DirNode, public Revertible {

public:

	Input(StringUMap& data, DirNode* parent) noexcept :
		Parent(data, vector<string>{
			COLLECTION_INPUT_SOURCES
			//COLLECTION_INPUT_LINKMAP
		}),
		DirNode(getProperties(), parent, getValue(FILENAME)),
		Revertible(*this, children)
	{}

	virtual ~Input() = default;

	string createUniqueId()   const noexcept override;
	string_view getXmlTag()         const noexcept override { return "Input"; }
	string_view getCssClass()       const noexcept override { return "InputBoxButton"; }
	string createPrettyName() const noexcept override;
	string createTooltip()    const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance(COLLECTION_INPUT);
	}

	void wipe()     noexcept override;
	void tearDown() noexcept override;

};

} // namespace
