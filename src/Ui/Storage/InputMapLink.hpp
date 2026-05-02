/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputMapLink.hpp
 * @since     Feb 6, 2024
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

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::InputMapLink
 *
 * A single linked-map group owned by an Input.
 * Acts as a Parent whose primary child collection holds Link objects.
 */
class InputMapLink: public Parent {

public:

	InputMapLink(StringUMap& data, const string& inputPid) noexcept;

	virtual ~InputMapLink() = default;

	string_view getXmlTag()   const noexcept override { return emptyString; }
	string_view getCssClass() const noexcept override { return CSS_LINK_BOX_BUTTON; }
	string createPrettyName() const noexcept override;
	string createTooltip()    const noexcept override;
	string createUniqueId()   const noexcept override { return emptyString; }
	string toXML()            const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

} // namespace
