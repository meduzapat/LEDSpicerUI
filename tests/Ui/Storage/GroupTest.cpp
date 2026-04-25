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
using LEDSpicerUI::Defaults;

class GroupTest : public ::testing::Test {
protected:
	void SetUp() override {
		StringUMap d{{NAME, "CONTROLS"}, {DEFAULT_COLOR, ""}};
		group = std::make_unique<Group>(d);
	}

	void TearDown() override {
		group.reset();
		CollectionHandler::purgeAll();
	}

	std::unique_ptr<Group> group;
};

// createUniqueId uses NAME.
TEST_F(GroupTest, CreateUniqueId) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"CONTROLS"}),
		group->createUniqueId()
	);
}

// Child collection is keyed as COLLECTION_GROUP_LINKS not COLLECTION_ELEMENTS.
TEST_F(GroupTest, ChildCollectionKeyIsGroupLinks) {
	EXPECT_NE(nullptr, group->getChild(COLLECTION_GROUP_LINKS));
	EXPECT_EQ(nullptr, group->getChild(COLLECTION_ELEMENTS));
}

// Child collection starts empty.
TEST_F(GroupTest, ChildCollectionStartsEmpty) {
	EXPECT_EQ(0u, group->getChild(COLLECTION_GROUP_LINKS)->getSize());
}

// getCssClass.
TEST_F(GroupTest, CssClass) {
	EXPECT_EQ("GroupBoxButton", group->getCssClass());
}

// toXML contains group tag and closes correctly.
TEST_F(GroupTest, ToXMLStructure) {
	string xml(group->toXML());
	EXPECT_NE(string::npos, xml.find("<group"));
	// Empty group emits self-closing tag
	EXPECT_NE(string::npos, xml.find("/>"));
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
