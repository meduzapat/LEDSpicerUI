/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LinkTest.cpp
 * @since     Mar 30, 2026
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
#include "Storage/Link.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

class TargetData : public Data {
public:
	TargetData(StringUMap& d) : Data(d) {}
	constexpr string_view getCssClass() const noexcept override { return "TargetClass"; }
	constexpr string_view getXmlTag()   const noexcept override { return "target"; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

class LinkTest : public ::testing::Test {
protected:
	void SetUp() override {
		StringUMap td{{NAME, "P1_BUTTON1"}};
		target  = std::make_unique<TargetData>(td);

		StringUMap ld{{COLOR, "Red"}, {FILTER, "Normal"}};
		link = std::make_unique<Link>(ld, Link::LinkData{"element", NAME, target.get()});
	}

	void TearDown() override {
		CollectionHandler::purgeAll();
	}

	std::unique_ptr<TargetData> target;
	std::unique_ptr<Link>       link;
};

// Delegates identity to target.
TEST_F(LinkTest, CssClassDelegatesToTarget) {
	EXPECT_EQ("TargetClass", link->getCssClass());
}

TEST_F(LinkTest, CreatePrettyNameDelegatesToTarget) {
	EXPECT_EQ("P1_BUTTON1", link->createPrettyName());
}

TEST_F(LinkTest, CreateUniqueIdDelegatesToTarget) {
	EXPECT_EQ(target->createUniqueId(), link->createUniqueId());
}

// getValue routes the link key to target's primary value.
TEST_F(LinkTest, GetValueRoutesLinkKeyToTarget) {
	EXPECT_EQ("P1_BUTTON1", link->getValue(NAME));
}

// getValue for own fields returns own data.
TEST_F(LinkTest, GetValueReturnsOwnFieldsForOtherKeys) {
	EXPECT_EQ("Red",    link->getValue(COLOR));
	EXPECT_EQ("Normal", link->getValue(FILTER));
}

// toXML emits self-closing element with target's unique ID.
TEST_F(LinkTest, ToXMLSelfClosing) {
	const string xml(link->toXML());
	EXPECT_NE(string::npos, xml.find("/>"));
	EXPECT_NE(string::npos, xml.find("element"));
	EXPECT_NE(string::npos, xml.find(target->createUniqueId()));
}

// operator== — true for same pointer and for target pointer.
TEST_F(LinkTest, OperatorEqualsSamePointer) {
	EXPECT_TRUE(*link == *link);
}

TEST_F(LinkTest, OperatorEqualsTargetPointer) {
	EXPECT_TRUE(*link == *target);
}
