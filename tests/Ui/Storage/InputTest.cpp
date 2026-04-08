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

class StubDirNode : public DirNode {

public:

	StubDirNode() : DirNode(nullptr) {}
	const string& getName()  const noexcept override { return id; }
	const string& getFsId()  const noexcept override { return id; }

private:

	inline static const string id = "stub_1";
};

class InputTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap rootData{{NAME, "Actions"}, {FILENAME, "myinput"}};
		rootInput = std::make_unique<Input>(rootData, nullptr);

		parentDir = std::make_unique<StubDirNode>();

		StringUMap nestedData{{NAME, "Mame"}, {FILENAME, "nestedinput"}};
		nestedInput = std::make_unique<Input>(nestedData, parentDir.get());
	}

	void TearDown() override {
		rootInput.reset();
		nestedInput.reset();
		parentDir.reset();
		CollectionHandler::purgeAll();
	}

	std::unique_ptr<Input>       rootInput;
	std::unique_ptr<StubDirNode> parentDir;
	std::unique_ptr<Input>       nestedInput;
};

// getCssClass.
TEST_F(InputTest, CssClass) {
	EXPECT_EQ("InputBoxButton", rootInput->getCssClass());
}

// createTooltip uses NAME field.
TEST_F(InputTest, CreateTooltip) {
	EXPECT_EQ("Input of type Actions", rootInput->createTooltip());
}

// createPrettyName includes path and NAME.
TEST_F(InputTest, CreatePrettyNameAtRoot) {
	const string pretty(rootInput->createPrettyName());
	EXPECT_NE(string::npos, pretty.find("myinput"));
	EXPECT_NE(string::npos, pretty.find("Actions"));
}

TEST_F(InputTest, CreatePrettyNameNested) {
	const string pretty(nestedInput->createPrettyName());
	EXPECT_NE(string::npos, pretty.find("stub_1"));
	EXPECT_NE(string::npos, pretty.find("nestedinput"));
	EXPECT_NE(string::npos, pretty.find("Mame"));
}

// createUniqueId uses parent FsId + FILENAME.
TEST_F(InputTest, CreateUniqueIdAtRoot) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"", "myinput"}),
		rootInput->createUniqueId()
	);
}

TEST_F(InputTest, CreateUniqueIdNested) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"stub_1", "nestedinput"}),
		nestedInput->createUniqueId()
	);
}

// Child collections keyed correctly.
TEST_F(InputTest, HasSourcesChild) {
	EXPECT_NE(nullptr, rootInput->getChild(COLLECTION_INPUT_SOURCES));
}

TEST_F(InputTest, HasLinkedMapsChild) {
	EXPECT_NE(nullptr, rootInput->getChild(COLLECTION_INPUT_LINKMAP));
}

// FILENAME is a property, not serialized.
TEST_F(InputTest, FilenameNotSerialized) {
	EXPECT_EQ(string::npos, rootInput->toXML().find(FILENAME));
}

// toXML contains Input header and footer.
TEST_F(InputTest, ToXMLStructure) {
	const string xml(rootInput->toXML());
	EXPECT_NE(string::npos, xml.find("type=\"Input\""));
	EXPECT_NE(string::npos, xml.find("</" PACKAGE_DATA_NAME ">"));
}

// wipe clears fieldsData.
TEST_F(InputTest, WipeClearsFields) {
	rootInput->wipe();
	EXPECT_TRUE(rootInput->getValues()->empty());
}
