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

class StubDirNode : public DirNode {

public:

	StubDirNode() : DirNode(nullptr) {}
	const string& getName()  const noexcept override { return id; }
	const string& getFsId()  const noexcept override { return id; }

private:

	inline static const string id = "stub_1";
};

class ProfileTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap rootData{{FILENAME, "default"}, {BACKGROUND_COLOR, "Off"}};
		rootProfile = std::make_unique<Profile>(rootData, nullptr);

		parentDir = std::make_unique<StubDirNode>();

		StringUMap nestedData{{FILENAME, "nested"}, {BACKGROUND_COLOR, "Red"}};
		nestedProfile = std::make_unique<Profile>(nestedData, parentDir.get());
	}

	void TearDown() override {
		rootProfile.reset();
		nestedProfile.reset();
		parentDir.reset();
		CollectionHandler::purgeAll();
	}

	std::unique_ptr<Profile>    rootProfile;
	std::unique_ptr<Profile>    nestedProfile;
	std::unique_ptr<StubDirNode> parentDir;
};

// getCssClass.
TEST_F(ProfileTest, CssClass) {
	EXPECT_EQ("ProfileBoxButton", rootProfile->getCssClass());
}

// FILENAME is a property, not serialized.
TEST_F(ProfileTest, FilenameInProperties) {
	EXPECT_EQ("default", rootProfile->getProperty(FILENAME));
	EXPECT_EQ("",        rootProfile->getValue(FILENAME));
}

// createUniqueId uses PID + FILENAME.
TEST_F(ProfileTest, CreateUniqueIdAtRoot) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"", "default"}),
		rootProfile->createUniqueId()
	);
}

TEST_F(ProfileTest, CreateUniqueIdNested) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"stub_1", "nested"}),
		nestedProfile->createUniqueId()
	);
}

// Child collections keyed correctly.
TEST_F(ProfileTest, HasElementsChild) {
	EXPECT_NE(nullptr, rootProfile->getChild(COLLECTION_PROFILE_ELEMENTS));
}

TEST_F(ProfileTest, HasGroupsChild) {
	EXPECT_NE(nullptr, rootProfile->getChild(COLLECTION_PROFILE_GROUPS));
}

TEST_F(ProfileTest, HasInputsChild) {
	EXPECT_NE(nullptr, rootProfile->getChild(COLLECTION_PROFILE_INPUTS));
}

TEST_F(ProfileTest, HasAnimationsChild) {
	EXPECT_NE(nullptr, rootProfile->getChild(COLLECTION_PROFILE_ANIMATIONS));
}

// toXML contains Profile header and footer.
TEST_F(ProfileTest, ToXMLStructure) {
	const string xml(rootProfile->toXML());
	EXPECT_NE(string::npos, xml.find("type=\"Profile\""));
	EXPECT_NE(string::npos, xml.find("</" PACKAGE_DATA_NAME ">"));
}

// FILENAME must not appear in XML output.
TEST_F(ProfileTest, FilenameNotSerialized) {
	EXPECT_EQ(string::npos, rootProfile->toXML().find(FILENAME));
}

// Empty collections produce no XML sections.
TEST_F(ProfileTest, EmptyCollectionsProduceNoSections) {
	const string xml(rootProfile->toXML());
	EXPECT_EQ(string::npos, xml.find("alwaysOnElements"));
	EXPECT_EQ(string::npos, xml.find("alwaysOnGroups"));
	EXPECT_EQ(string::npos, xml.find("inputs"));
	EXPECT_EQ(string::npos, xml.find("animations"));
}

// wipe clears fieldsData.
TEST_F(ProfileTest, WipeClearsFields) {
	rootProfile->wipe();
	EXPECT_TRUE(rootProfile->getValues()->empty());
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
