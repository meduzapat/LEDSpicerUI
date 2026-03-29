/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ParentTest.cpp
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
#include "Storage/Parent.hpp"

using namespace LEDSpicerUI::Constants;
using namespace LEDSpicerUI::Ui::Storage;

class TestData : public Data {
public:
	TestData(StringUMap& d) : Data(d, "TEST_DATA") {}
	string_view getCssClass() const noexcept override { return ""; }
};

struct TestParent : Parent {
	TestParent(StringUMap& d, StringBoxButtonCollectionUMap ch)
		: Parent(d, "TEST_COLLECTION", std::move(ch)) {}
	string_view getCssClass() const noexcept override { return ""; }
	using Parent::registerDependency;
};

struct ParentTest : ::testing::Test {

	StringUMap                    data;
	StringBoxButtonCollectionUMap children;

	void SetUp() override {
		children["A"]; // default-constructs in place — no copy
		children["B"];
	}
};

TEST_F(ParentTest, getChild_existingKey_returnsPointer) {
	TestParent p(data, std::move(children));
	EXPECT_NE(p.getChild("A"), nullptr);
}

TEST_F(ParentTest, getChild_missingKey_returnsNull) {
	TestParent p(data, std::move(children));
	EXPECT_EQ(p.getChild("MISSING"), nullptr);
}

TEST_F(ParentTest, getChild_constOverload_returnsPointer) {
	const TestParent p(data, std::move(children));
	EXPECT_NE(p.getChild("A"), nullptr);
}

TEST_F(ParentTest, getChildren_returnsAllChildren) {
	TestParent p(data, std::move(children));
	EXPECT_EQ(p.getChildren().size(), 2u);
}

TEST_F(ParentTest, getSize_matchesInjectedSize) {
	TestParent p(data, std::move(children));
	EXPECT_EQ(p.getSize(), 2u);
}

TEST_F(ParentTest, empty_falseWhenChildrenPresent) {
	TestParent p(data, std::move(children));
	EXPECT_FALSE(p.empty());
}

TEST_F(ParentTest, empty_trueWhenNoChildren) {
	TestParent p(data, {});
	EXPECT_TRUE(p.empty());
}

TEST_F(ParentTest, iteration_visitsAllChildren) {
	TestParent p(data, std::move(children));
	size_t count = 0;
	for (auto& _ : p)
		++count;
	EXPECT_EQ(count, 2u);
}
