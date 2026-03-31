/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RevertibleTest.cpp
 * @since     Mar 2026
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

using namespace LEDSpicerUI::Ui::Storage;

class TestRevertible : public Revertible {

public:

	using Revertible::Revertible;

	// Expose snap storage for assertions only.
	const StringUMap& getSnapFields() const noexcept { return snapFields; }

};

// Fields swap into snap and restore correctly.
TEST(RevertibleTest, SwapAndRevert) {
	StringUMap data{{"key1", "value1"}, {"key2", "value2"}};
	const StringUMap expected{data};

	TestRevertible r(data);

	EXPECT_EQ(data, expected);
	EXPECT_TRUE(r.getSnapFields().empty());

	r.swap();
	EXPECT_TRUE(data.empty());
	EXPECT_EQ(r.getSnapFields(), expected);

	r.revert();
	EXPECT_EQ(data, expected);
	EXPECT_TRUE(r.getSnapFields().empty());
}

// swap() is a no-op when fields are already empty.
TEST(RevertibleTest, SwapNoOpOnEmptyFields) {
	StringUMap data;
	TestRevertible r(data);

	r.swap();
	EXPECT_TRUE(r.getSnapFields().empty());
}

// swap() is a no-op when a snapshot already exists.
TEST(RevertibleTest, SwapNoOpWhenAlreadySnapped) {
	StringUMap data{{"k", "v"}};
	TestRevertible r(data);

	r.swap();
	const StringUMap firstSnap{r.getSnapFields()};
	data["extra"] = "x";
	r.swap();

	EXPECT_EQ(r.getSnapFields(), firstSnap);
}

// revert() is a no-op when no snapshot exists.
TEST(RevertibleTest, RevertNoOpWithoutSnap) {
	StringUMap data{{"k", "v"}};
	const StringUMap expected{data};
	TestRevertible r(data);

	r.revert();
	EXPECT_EQ(data, expected);
}

// clearSnap() discards snap without touching live fields.
TEST(RevertibleTest, ClearSnapLeavesLiveFields) {
	StringUMap data{{"k", "v"}};
	const StringUMap expected{data};
	TestRevertible r(data);

	r.swap();
	r.clearSnap();

	EXPECT_TRUE(r.getSnapFields().empty());
	// Live fields were swapped out — clearSnap does not restore them.
	EXPECT_TRUE(data.empty());
}

// nullptr children — construction and swap/revert work without crash.
TEST(RevertibleTest, NullChildrenSafe) {
	StringUMap data{{"k", "v"}};
	TestRevertible r(data, nullptr);

	r.swap();
	r.revert();
	EXPECT_TRUE(r.getSnapFields().empty());
}

// With children map — swap clears it, revert restores it.
TEST(RevertibleTest, SwapAndRevertWithChildren) {
	StringUMap data{{"k", "v"}};
	StringBoxButtonCollectionUMap children;
	children["A"];
	children["B"];

	TestRevertible r(data, &children);

	r.swap();
	EXPECT_TRUE(data.empty());

	r.revert();
	EXPECT_FALSE(data.empty());
}
