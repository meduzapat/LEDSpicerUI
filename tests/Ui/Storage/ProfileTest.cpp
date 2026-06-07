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
#include "Storage/Link.hpp"

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

// Minimal stub target for Link — avoids pulling in Element/Group machinery.
class StubTarget : public Data {

public:

	StubTarget(Values& d, const string& cssClass, const string& xmlTag) :
		Data(d), css(cssClass), tag(xmlTag) {}

	const string& getCssClass() const noexcept override { return css; }
	const string& getXmlTag()   const noexcept override { return tag; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }

private:

	const string css;
	const string tag;
};

class ProfileTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(ProfileTest, TestFunctionality) {

	Values rootData   {{FILENAME, DEFAULT}, {BACKGROUND_COLOR, "Off"}};
	Profile rootProfile {rootData, nullptr};

	StubDirNode parentDir;

	Values nestedData {{FILENAME, "nested"}, {BACKGROUND_COLOR, "Red"}};
	Profile nestedProfile {nestedData, &parentDir};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_PROFILE_BOX_BUTTON, rootProfile.getCssClass());
	EXPECT_EQ(TYPE_PROFILE,           rootProfile.getXmlTag());
	EXPECT_NE(nullptr,                rootProfile.getCollectionHandler());

	// createUniqueId at root and nested.
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString,         DEFAULT}),  rootProfile.createUniqueId());
	EXPECT_EQ(Defaults::createCommonUniqueId({parentDir.getFsId(), "nested"}), nestedProfile.createUniqueId());

	// Child collections.
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_ELEMENTS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_GROUPS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_INPUTS));
	EXPECT_NE(nullptr, rootProfile.getChild(COLLECTION_PROFILE_ANIMATIONS));

}

TEST_F(ProfileTest, ToXMLEmpty) {

	Values data {{FILENAME, "empty"}, {BACKGROUND_COLOR, "Off"}};
	Profile profile {data, nullptr};

	// Empty profile: header + footer only, no inner sections.
	EXPECT_EQ(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
		"<LEDSpicer\n\tversion=\"1.1\"\n\ttype=\"Profile\"\n\tbackgroundColor=\"Off\"\n>\n</LEDSpicer>\n",
		profile.toXML()
	);
}

TEST_F(ProfileTest, ToXMLWithChildren) {

	// Target items the Links will point at.
	Values eRaw {{NAME, "P1_BUTTON_1"}};
	StubTarget targetElement {eRaw, "ElementClass", TYPE_ELEMENT};

	Values gRaw {{NAME, "CONTROLS"}};
	StubTarget targetGroup {gRaw, "GroupClass", TYPE_GROUP};

	Values profileRaw {{FILENAME, "withChildren"}, {BACKGROUND_COLOR, "Red"}};
	Profile profile {profileRaw, nullptr};

	// Element link inside alwaysOnElements.
	Values elLink {{COLOR, "White"}};
	profile.getChild(COLLECTION_PROFILE_ELEMENTS)->create(
		new Link(elLink, NAME, TYPE_ELEMENT, {}, &targetElement)
	);

	// Group link inside alwaysOnGroups.
	Values grLink {{COLOR, "Gray"}};
	profile.getChild(COLLECTION_PROFILE_GROUPS)->create(
		new Link(grLink, NAME, TYPE_GROUP, {}, &targetGroup)
	);

	const string xml(profile.toXML());
	EXPECT_NE(string::npos, xml.find("type=\"Profile\""));
	EXPECT_NE(string::npos, xml.find("backgroundColor=\"Red\""));
	EXPECT_NE(string::npos, xml.find("<alwaysOnElements>"));
	EXPECT_NE(string::npos, xml.find("<element"));
	EXPECT_NE(string::npos, xml.find("name=\"P1_BUTTON_1\""));
	EXPECT_NE(string::npos, xml.find("</alwaysOnElements>"));
	EXPECT_NE(string::npos, xml.find("<alwaysOnGroups>"));
	EXPECT_NE(string::npos, xml.find("<group"));
	EXPECT_NE(string::npos, xml.find("name=\"CONTROLS\""));
	EXPECT_NE(string::npos, xml.find("</alwaysOnGroups>"));
	// No animations or inputs sections — children empty.
	EXPECT_EQ(string::npos, xml.find("<animations>"));
	EXPECT_EQ(string::npos, xml.find("<inputs>"));
	EXPECT_NE(string::npos, xml.find("</LEDSpicer>"));
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
