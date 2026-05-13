/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileTest.cpp
 * @since     Apr 2026
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
#include "Storage/Profile.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

// Private-base trick: ensures Values is initialized before DirNode.
struct StubValues { Values v; };

class StubDirNode : private StubValues, public DirNode {

public:

	StubDirNode() noexcept : StubValues{}, DirNode(v, nullptr, "stub_1") {}

};

class ProfileTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(ProfileTest, TestFunctionality) {

	Values rootData   {{FILENAME, "default"}, {BACKGROUND_COLOR, "Off"}};
	Profile rootProfile {rootData, nullptr};

	StubDirNode parentDir;

	Values nestedData {{FILENAME, "nested"}, {BACKGROUND_COLOR, "Red"}};
	Profile nestedProfile {nestedData, &parentDir};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_PROFILE_BOX_BUTTON, rootProfile.getCssClass());
	EXPECT_EQ(TYPE_PROFILE,           rootProfile.getXmlTag());
	EXPECT_NE(nullptr,                rootProfile.getCollectionHandler());

	// createUniqueId at root and nested.
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString,          "default"}), rootProfile.createUniqueId());
	EXPECT_EQ(Defaults::createCommonUniqueId({parentDir.getFsId(),  "nested"}),  nestedProfile.createUniqueId());

	// Child collections.
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_ELEMENTS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_GROUPS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_INPUTS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_ANIMATIONS));

	// toXML.
	const string xml(rootProfile.toXML());
	EXPECT_EQ("<profile filename=\"default\" backgroundColor=\"Off\"/>\n", xml);

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
