/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
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

#include "Hardware.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Device
 *
 * Stores a hardware device and its attached elements.
 */
class Device : public Hardware {

public:

	Device(Values& data) noexcept : Hardware(data, {COLLECTION_ELEMENTS}) {}

	virtual ~Device() = default;

	const string& getXmlTag()   const noexcept override { return TYPE_DEVICE; }
	const string& getCssClass() const noexcept override { return CSS_DEVICE_BOX_BUTTON; }
	string createUniqueId()     const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override;

protected:

	bool shouldSerialize(const string& key, const string& value) const noexcept override;
};

} // namespace
