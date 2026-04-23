/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Process.hpp
 * @since     May 2, 2023
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
 * LEDSpicerUI::Ui::Storage::Process
 */
class Process: public Data {

public:

	using Data::Data;

	virtual ~Process() = default;

	string_view getXmlTag()   const noexcept override { return "map"; }
	string_view getCssClass() const noexcept override { return "ProcessBoxButton"; }
	string createPrettyName() const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override;

protected:

	string getPrimaryKey() const noexcept override {return PARAM_PROCESS_NAME;}

};

} // namespace
