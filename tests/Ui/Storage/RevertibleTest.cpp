/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RevertibleTest.cpp
 * @since     Mar 27, 2026
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
#include "Storage/Revertible.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Constants;
using namespace LEDSpicerUI::Ui::Storage;

class TestRevertible : public Revertible {

public:

	using Revertible::Revertible;

	// Expose protected members only for testing.
	StringUMap&       getFieldsData()       { return fieldsData; }
	const StringUMap& getFieldsData() const { return fieldsData; }
	StringUMap&       getSnapFields()       { return snapFields; }
	const StringUMap& getSnapFields() const { return snapFields; }

	constexpr string_view getCssClass() const noexcept override { return ""; }
};

TEST(RevertibleTest, SwapAndRevert) {
	StringUMap data{{"key1", "value1"}, {"key2", "value2"}};
	// copy before the move happens
	StringUMap expected = data;

	StringBoxButtonCollectionUMap childrenMap;
	childrenMap[COLLECTION_ELEMENT];

	TestRevertible r(data, "test-collection", std::move(childrenMap));

	EXPECT_EQ(r.getFieldsData(), expected);
	EXPECT_TRUE(r.getSnapFields().empty());

	r.swap();
	EXPECT_TRUE(r.getFieldsData().empty());
	EXPECT_EQ(r.getSnapFields(), expected);

	r.revert();
	EXPECT_EQ(r.getFieldsData(), expected);
	EXPECT_TRUE(r.getSnapFields().empty());
}
