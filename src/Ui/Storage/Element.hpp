/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
 * @since     Mar 16, 2023
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

#include "CollectionHandler.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::ElementForm
 */
class Element: public Data {

public:

	using Data::Data;

	virtual ~Element();

	string_view getXmlTag()         const noexcept override { return "element"; }
	string_view getCssClass()       const noexcept override { return "ElementBoxButton"; }
	const string createPrettyName() const noexcept override;

	/**
	 * Adds a child to the element, this is used for RGB strips.
	 * @param child
	 */
	void addStripChild(Element* child) noexcept;

	/**
	 * @return the list of strip children.
	 */
	vector<Element*> copyStripChildren() noexcept { return stripChildren; }

	/**
	 * Clears all strip children.
	 */
	void clearStripChildren() noexcept;

	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance(COLLECTION_ELEMENT);
	}

	/**
	 * Attempts to convert any RGB setup into scattered RGB.
	 * @param data
	 */
	static void splitRGB(Data* data) noexcept;

	/**
	 * Using the position, stores the split RGB connectors.
	 * @param data
	 * @param position
	 */
	static void convertPositionToRGB(Data* data, const string& position, const string& colorFormat) noexcept;

	/**
	 * @param position
	 * @return the position of the first connector on the position.
	 */
	static uint16_t findFirstConnectorIndexByPosition(const string& position) noexcept;

protected:

	// Owns pseudo children.
	vector<Element*> stripChildren;

	bool shouldSerialize(const string& key, const string& value) const noexcept override {
		return not (key == BRIGHTNESS and value == DEFAULT_BRIGHTNESS);
	}
};

} // namespace

