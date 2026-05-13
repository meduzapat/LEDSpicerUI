/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroupTest.cpp
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
#include "Storage/Group.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Values;

class GroupTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(GroupTest, TestFunctionality) {

	Values d {{NAME, "CONTROLS"}, {DEFAULT_COLOR, ""}};
	Group group {d};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_GROUP_BOX_BUTTON, group.getCssClass());
	EXPECT_EQ(TYPE_GROUP,           group.getXmlTag());
	EXPECT_NE(nullptr,              group.getCollectionHandler());

	// Child collection keyed as COLLECTION_GROUP_LINKS, not COLLECTION_ELEMENTS.
	EXPECT_NE(nullptr, group.getChild(COLLECTION_GROUP_LINKS));
	EXPECT_EQ(nullptr, group.getChild(COLLECTION_ELEMENTS));

	// toXML emits group tag.
	const string xml(group.toXML());
	EXPECT_EQ("<group name=\"CONTROLS\"/>\n", xml);

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
