/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
 * @since     Feb 25, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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
#include <fstream>
#include "XMLHelper.hpp"

using namespace LEDSpicerUI;

// Constructor tests
TEST(XMLHelperTest, ConstructorErrorHandling) {
	GTEST_LOG_(INFO) << "Loading " PACKAGE_SAMPLES_DIR << "data/config.xml";
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml",    "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/inputs/input.xml",     "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/animations/animation.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/profiles/profile.xml",   "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/malformed.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidLedspicerNode.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidDataVersion.xml",   "InvalidType"), Message);
}

TEST(XMLHelperTest, ConstructorWithValidType) {
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/inputs/input.xml", "Input"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/animations/animation.xml", "Animation"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/profiles/profile.xml", "Profile"));
}

TEST(XMLHelperTest, ConstructorForeignFile) {
	// Foreign files skip validation
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/colors.xml", XML_FILE_PLAIN));
}

TEST(XMLHelperTest, ProcessNodeReturnsCorrectMap) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	auto result = XMLHelper::processNode(helper.getRoot());

	EXPECT_EQ(result.size(), 12) << "Maps have different sizes";
}

TEST(XMLHelperTest, ProcessNodeByName) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	// Success case - test 'layout' node under root
	StringUMap expected = {
		{"defaultProfile", "default"}
	};
	StringUMap nodeParam;
	EXPECT_NO_THROW(nodeParam = helper.processNode("layout"));
	EXPECT_EQ(nodeParam.size(), expected.size()) << "Layout node attribute count mismatch";
	EXPECT_EQ(nodeParam["defaultProfile"], "default") << "defaultProfile value mismatch";

	// Failure case - nonexistent node
	EXPECT_THROW(helper.processNode("bogus"), Message);
}

TEST(XMLHelperTest, GetRoot) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	tinyxml2::XMLElement* root = helper.getRoot();

	ASSERT_NE(root, nullptr) << "Root should not be null";
	EXPECT_STREQ(root->Name(), PACKAGE_DATA_NAME) << "Root node name should be " PACKAGE_DATA_NAME;
}

TEST(XMLHelperTest, GetRootInfo) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	const auto& rootInfo = helper.getRootInfo();

	EXPECT_EQ(rootInfo.version, PACKAGE_DATA_VERSION) << "Version should match " PACKAGE_DATA_VERSION;
	EXPECT_EQ(rootInfo.type, "Configuration") << "Type should be Configuration";
	EXPECT_FALSE(rootInfo.attributes.empty()) << "Attributes should not be empty";
}

TEST(XMLHelperTest, GetSettings) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	StringUMap settings = helper.getSettings();

	EXPECT_FALSE(settings.empty()) << "Settings should not be empty";
	EXPECT_EQ(settings["version"], PACKAGE_DATA_VERSION) << "Version should be present";
	EXPECT_EQ(settings["type"], "Configuration") << "Type should be present";
}

TEST(XMLHelperTest, CheckAttributes) {
	// Success case - all attributes present
	StringVector attributeList = {"name", "type", "id"};
	StringUMap subjects = {
		{"name",  "Test"},
		{"type",  "Button"},
		{"id",    "1"},
		{"extra", "ignored"}
	};
	EXPECT_NO_THROW(XMLHelper::checkAttributes(attributeList, subjects, "testNode"));

	// Failure case - missing 'type'
	StringUMap incompleteSubjects = {
		{"name", "Test"},
		{"id",   "1"}
	};
	EXPECT_THROW(XMLHelper::checkAttributes(attributeList, incompleteSubjects, "testNode"), Message);
}

TEST(XMLHelperTest, ValueOf) {
	StringUMap values = {
		{"name", "Test"},
		{"type", "Button"}
	};

	EXPECT_EQ(XMLHelper::valueOf(values, "name"), "Test") << "Should return existing value";
	EXPECT_EQ(XMLHelper::valueOf(values, "id"), "") << "Should return empty string for missing key";
	EXPECT_EQ(XMLHelper::valueOf(values, "id", "defaultId"), "defaultId") << "Should return custom default for missing key";
}

TEST(XMLHelperTest, ToXML) {
	StringUMap values = {
		{"name", "Test"},
		{"type", "Button"},
		{"id", "1"}
	};
	string result = XMLHelper::toXML(values);

	// Check that all key-value pairs are present (order may vary due to unordered_map)
	EXPECT_NE(result.find("id=\"1\""), string::npos) << "Should contain id";
	EXPECT_NE(result.find("type=\"Button\""), string::npos) << "Should contain type";
	EXPECT_NE(result.find("name=\"Test\""), string::npos) << "Should contain name";

	// Empty map
	StringUMap emptyValues;
	EXPECT_EQ(XMLHelper::toXML(emptyValues), "") << "Empty map should return empty string";
}

TEST(XMLHelperTest, XmlHeader) {
	string header = XMLHelper::xmlHeader("Configuration");

	EXPECT_NE(header.find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"), string::npos)
		<< "Should contain XML declaration";
	EXPECT_NE(header.find(DEFAULT_MESSAGE), string::npos)
		<< "Should contain default message";
	EXPECT_NE(header.find("<" PACKAGE_DATA_NAME), string::npos)
		<< "Should contain root element";
	EXPECT_NE(header.find("version=\"" PACKAGE_DATA_VERSION "\""), string::npos)
		<< "Should contain version";
	EXPECT_NE(header.find("type=\"Configuration\""), string::npos)
		<< "Should contain type";
}

TEST(XMLHelperTest, XmlHeaderNoType) {
	string header = XMLHelper::xmlHeader("");

	EXPECT_NE(header.find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"), string::npos)
		<< "Should contain XML declaration";
	EXPECT_NE(header.find("version=\"" PACKAGE_DATA_VERSION "\""), string::npos)
		<< "Should contain version";
	EXPECT_NE(header.find("type=\"\""), string::npos)
		<< "Should contain type when empty";
}

TEST(XMLHelperTest, XmlFooter) {
	string footer = XMLHelper::xmlFooter();

	EXPECT_EQ(footer, "</" PACKAGE_DATA_NAME ">\n") << "Footer should be closing tag";
}

TEST(XMLHelperTest, GetData) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	StringUMapVector& data = helper.getData("devices");

	EXPECT_TRUE(data.empty()) << "Unpopulated data should return empty vector";
}

TEST(XMLHelperTest, CleanError) {
	// Valid error with full details
	string rawError = "Unable to read the file /xxx/yyy/zzzz.xml Error=XML_ERROR_MISMATCHED_ELEMENT ErrorID=14 (0xe) Line number=369: XMLElement name=map";
	string expected = "Unable to read the file /xxx/yyy/zzzz.xml\nError: XML_ERROR_MISMATCHED_ELEMENT\nLine: 369\nNode: map";
	string result = XMLHelper::cleanError(rawError);
	EXPECT_EQ(result, expected) << "Full error should be cleaned correctly";

	// Simple error with no extra details
	string simpleError = "Unable to read the file /xxx/yyy/zzzz.xml";
	EXPECT_EQ(XMLHelper::cleanError(simpleError), simpleError) << "Simple error should return unchanged";
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
