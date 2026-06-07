/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputTest.cpp
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
#include "Storage/Input.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

// Private-base trick: ensures Values is initialized before DirNode.
struct StubValues { Values v; };

class StubDirNode : private StubValues, public DirNode {

public:

	StubDirNode() noexcept : StubValues{}, DirNode(v, nullptr, "stub") {}

};

class InputTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(InputTest, TestFunctionality) {

	Values rootData {{NAME, "Actions"}, {FILENAME, "myinput"}};
	Input rootInput {rootData, nullptr};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_INPUT_BOX_BUTTON, rootInput.getCssClass());
	EXPECT_EQ(TYPE_INPUT,           rootInput.getXmlTag());
	EXPECT_NE(nullptr,              rootInput.getCollectionHandler());

	// createTooltip and createPrettyName.
	EXPECT_EQ("Input of type Actions", rootInput.createTooltip());
	EXPECT_NE(string::npos, rootInput.createPrettyName().find("myinput"));
	EXPECT_NE(string::npos, rootInput.createPrettyName().find("Actions"));

	// createUniqueId at root and nested.
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString, "myinput"}), rootInput.createUniqueId());

	StubDirNode stubDir;
	Values nestedData {{NAME, "Mame"}, {FILENAME, "nestedinput"}};
	Input nestedInput {nestedData, &stubDir};
	EXPECT_EQ(Defaults::createCommonUniqueId({stubDir.getFsId(), "nestedinput"}), nestedInput.createUniqueId());

	// Child collections.
	EXPECT_NE(nullptr, rootInput.getChild(COLLECTION_INPUT_SOURCES));
	EXPECT_NE(nullptr, rootInput.getChild(COLLECTION_INPUT_LINKMAPS));

	// toXML.
	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			  "<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
			  "<LEDSpicer\n\tversion=\"1.1\"\n\ttype=\"Input\"\n\tname=\"Actions\"\n\tfilename=\"myinput\"\n>\n</LEDSpicer>\n",
			  rootInput.toXML());

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
