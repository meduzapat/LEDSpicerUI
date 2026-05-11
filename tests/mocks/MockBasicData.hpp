/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MockBasicData.hpp
 * @since     May 11, 2026
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

#include <gtest/gtest.h>
#include "Storage/Data.hpp"

#pragma once

using namespace LEDSpicerUI;
using namespace Ui::Storage;
using namespace Constants;

namespace LEDSpicerUI::Test::Mocks {

const string
	testCss {"testCss"},
	testTag {"testTag"};

/**
 * A mock class that completes Data with two constants and null collection.
 */
class MockBasicData : public Data {

	public:

		using Data::Data;

		const string& getCssClass() const noexcept override { return testCss; }
		const string& getXmlTag()   const noexcept override { return testTag; }

		CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
	};
}
