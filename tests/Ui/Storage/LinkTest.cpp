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
	const string& getCssClass() const noexcept override { return "TargetClass"; }
	const string& getXmlTag()   const noexcept override { return "target"; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

class LinkTest : public ::testing::Test {
protected:
	void SetUp() override {
		StringUMap td{{NAME, "P1_BUTTON1"}};
		target = std::make_unique<TargetData>(td);

		StringUMap ld{{COLOR, "Red"}, {FILTER, "Normal"}};
		link = std::make_unique<Link>(ld, linkKey, linkType, linkFields, target.get());
	}

	void TearDown() override {
		CollectionHandler::purgeAll();
	}

	// Stable owners — Link holds refs to these.
	const string                   linkKey    = NAME;
	const string                   linkType   = "element";
	const vector<Link::LinkField>  linkFields = {{
		"color", "Default Color", "Red", Link::LinkField::Widget::COLOR_PICKER
	}};

	std::unique_ptr<TargetData> target;
	std::unique_ptr<Link>       link;
};

// Delegates identity to target.
TEST_F(LinkTest, CssClassDelegatesToTarget) {
	EXPECT_EQ("TargetClass", link->getCssClass());
}

TEST_F(LinkTest, CreatePrettyNameDelegatesToTarget) {
	EXPECT_EQ(target->createPrettyName(), link->createPrettyName());
}

TEST_F(LinkTest, CreateUniqueIdDelegatesToTarget) {
	EXPECT_EQ(target->createUniqueId(), link->createUniqueId());
}

TEST_F(LinkTest, GetXmlTagDelegatesToTarget) {
	EXPECT_EQ("element", link->getXmlTag());
}

// getValue routes linkKey to target's primary value..
TEST_F(LinkTest, GetValueRoutesLinkKeyToTargetPrimaryValue) {
	EXPECT_EQ(target->getPrimaryValue(), link->getValue(NAME));
}

// getValue for own fields returns link's own fieldsData.
TEST_F(LinkTest, GetValueReturnsOwnFieldsForOtherKeys) {
	EXPECT_EQ("Red",    link->getValue(COLOR));
	EXPECT_EQ("Normal", link->getValue(FILTER));
}

// setValue on linkKey is silently ignored.
TEST_F(LinkTest, SetValueIgnoresLinkKey) {
	link->setValue(NAME, "SOMETHING_ELSE");
	EXPECT_EQ(target->createUniqueId(), link->getValue(NAME));
}

// setValue on own fields updates fieldsData.
TEST_F(LinkTest, SetValueUpdatesOwnFields) {
	link->setValue(COLOR, "Blue");
	EXPECT_EQ("Blue", link->getValue(COLOR));
}

// setLink replaces the target pointer.
TEST_F(LinkTest, SetLinkReplacesTarget) {
	StringUMap td2{{NAME, "P2_BUTTON1"}};
	TargetData target2(td2);
	link->setLink(&target2);
	EXPECT_EQ(target2.createUniqueId(), link->createUniqueId());
}

// getLinkFields returns the stable ref.
TEST_F(LinkTest, GetLinkFieldsReturnsFields) {
	ASSERT_EQ(1u, link->getLinkFields().size());
	EXPECT_EQ("color", link->getLinkFields()[0].key);
}

// toXML emits self-closing tag with linkType, linkKey, and target's uniqueId.
TEST_F(LinkTest, ToXMLStructure) {
	const string xml(link->toXML());
	EXPECT_NE(string::npos, xml.find("/>"));
	EXPECT_NE(string::npos, xml.find("element"));
	EXPECT_NE(string::npos, xml.find(target->createUniqueId()));
}

// toXML includes own fieldsData.
TEST_F(LinkTest, ToXMLIncludesOwnFields) {
	const string xml(link->toXML());
	EXPECT_NE(string::npos, xml.find("Red"));
	EXPECT_NE(string::npos, xml.find("Normal"));
}

// operator== true for same Data and for target pointer.
TEST_F(LinkTest, OperatorEqualsSelf) {
	EXPECT_TRUE(*link == *link);
	EXPECT_TRUE(*link == *target);
}

TEST_F(LinkTest, OperatorEqualsUnrelatedFalse) {
	StringUMap td2{{NAME, "OTHER"}};
	TargetData other(td2);
	EXPECT_FALSE(*link == other);
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
