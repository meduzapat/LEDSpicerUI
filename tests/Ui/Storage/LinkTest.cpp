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
using LEDSpicerUI::Values;

class TargetData : public Data {

public:

	TargetData(Values& d) : Data(d) {}

	const string& getCssClass() const noexcept override { static const string s {"TargetClass"}; return s; }
	const string& getXmlTag()   const noexcept override { static const string s {"target"};      return s; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

class LinkTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(LinkTest, TestFunctionality) {

	Values td {{NAME, "P1_BUTTON1"}};
	TargetData target {td};

	const string
		linkKey    = NAME,
		linkType   = "element";

	const vector<Link::LinkField> linkFields = {{
		"color", "Default Color", "Red", Link::LinkField::Widget::COLOR_PICKER
	}};

	Values ld {{COLOR, "Red"}, {FILTER, "Normal"}};
	Link link {ld, linkKey, linkType, linkFields, &target};

	// getCssClass, getXmlTag, createPrettyName, createUniqueId, createTooltip delegate to target.
	EXPECT_EQ(target.getCssClass(),      link.getCssClass());
	EXPECT_EQ("element",                 link.getXmlTag());
	EXPECT_EQ(target.createPrettyName(), link.createPrettyName());
	EXPECT_EQ(target.createUniqueId(),   link.createUniqueId());
	EXPECT_EQ(target.createTooltip(),    link.createTooltip());

	// getCollectionHandler always nullptr; getCollectionHandlerSource routes to target's handler.
	EXPECT_EQ(nullptr,                       link.getCollectionHandler());
	EXPECT_EQ(target.getCollectionHandler(), link.getCollectionHandlerSource());

	// getValue: linkKey routes to target primary value; other keys return own fields.
	EXPECT_EQ(target.getPrimaryValue(), link.getValue(NAME));
	EXPECT_EQ("Red",                    link.getValue(COLOR));
	EXPECT_EQ("Normal",                 link.getValue(FILTER));

	// setValue: linkKey is silently ignored; own fields update normally.
	link.setValue(NAME, "SOMETHING_ELSE");
	EXPECT_EQ(target.createUniqueId(), link.getValue(NAME));
	link.setValue(COLOR, "Blue");
	EXPECT_EQ("Blue", link.getValue(COLOR));

	// setLink replaces the target pointer.
	Values td2 {{NAME, "P2_BUTTON1"}};
	TargetData target2 {td2};
	link.setLink(&target2);
	EXPECT_EQ(target2.createUniqueId(), link.createUniqueId());
	link.setLink(&target);

	// getLinkFields returns the stable ref.
	ASSERT_EQ(1u, link.getLinkFields().size());
	EXPECT_EQ("color", link.getLinkFields()[0].key);

	// operator==: same object true, target pointer true, unrelated false.
	EXPECT_TRUE(link == link);
	EXPECT_TRUE(link == target);
	Values td3 {{NAME, "OTHER"}};
	TargetData other {td3};
	EXPECT_FALSE(link == other);

	// toXML.
	const string xml(link.toXML());
	EXPECT_EQ("<element\n\tcolor=\"Blue\"\n\tfilter=\"Normal\"\n\tname=\"P1_BUTTON1\"\n/>\n", xml);

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
