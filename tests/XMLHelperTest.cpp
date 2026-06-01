/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
 * @since     Feb 25, 2025
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
#include <fstream>
#include "XMLHelper.hpp"

using namespace LEDSpicerUI;

// Constructor tests
TEST(XMLHelperTest, ConstructorErrorHandling) {
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml",               "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/inputs/input.xml",         "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/animations/animation.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/profiles/profile.xml",     "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/malformed.xml",            "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidLedspicerNode.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidDataVersion.xml",   "InvalidType"), Message);
}

TEST(XMLHelperTest, ConstructorWithValidType) {
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml",               "Configuration"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/inputs/input.xml",         "Input"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/animations/animation.xml", "Animation"));
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/profiles/profile.xml",     "Profile"));
}

TEST(XMLHelperTest, ConstructorForeignFile) {
	// Foreign files skip validation
	EXPECT_NO_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/colors.xml", XML_FILE_PLAIN));
}

TEST(XMLHelperTest, ProcessNodeReturnsCorrectMap) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	auto result = XMLHelper::processNode(helper.getRoot());

	EXPECT_EQ(result.getSize(), 12) << "Maps have different sizes";
}

TEST(XMLHelperTest, ProcessNodeByName) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	// Success case - test 'layout' node under root
	const Values expected {
		{"defaultProfile", DEFAULT}
	};
	Values nodeParam;
	EXPECT_NO_THROW(nodeParam = helper.processNode("layout"));
	EXPECT_EQ(nodeParam.getSize(), expected.getSize())       << "Layout node attribute count mismatch";
	EXPECT_EQ(nodeParam.getValue("defaultProfile"), DEFAULT) << "defaultProfile value mismatch";

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

	EXPECT_EQ(rootInfo.getValue("version"), PACKAGE_DATA_VERSION) << "Version should match " PACKAGE_DATA_VERSION;
	EXPECT_EQ(rootInfo.getValue("type"), "Configuration")         << "Type should be Configuration";
	EXPECT_FALSE(rootInfo.getValues().empty())                    << "Attributes should not be empty";
}

TEST(XMLHelperTest, CheckAttributes) {
	// Success case - all attributes present
	StringVector attributeList = {"name", "type", "id"};
	Values subjects {
		{"name",  "Test"},
		{"type",  "Button"},
		{"id",    "1"},
		{"extra", "ignored"}
	};
	EXPECT_NO_THROW(XMLHelper::checkAttributes(attributeList, subjects, "testNode"));

	// Failure case - missing 'type'
	Values incompleteSubjects {
		{"name", "Test"},
		{"id",   "1"}
	};
	EXPECT_THROW(XMLHelper::checkAttributes(attributeList, incompleteSubjects, "testNode"), Message);
}

TEST(XMLHelperTest, ToXML) {

	Values values {
		{"name", "Test"},
		{"type", "Button"},
		{"id",   "1"}
	};
	const string result = XMLHelper::toXML(values);

	// Check that all key-value pairs are present (order may vary due to unordered_map)
	EXPECT_EQ("id=\"1\"\ntype=\"Button\"\nname=\"Test\"\n", result);

	// Empty map
	Values emptyValues;
	EXPECT_EQ(XMLHelper::toXML(emptyValues), "") << "Empty map should return empty string";
}

TEST(XMLHelperTest, XmlHeader) {

	const string
		header = XMLHelper::xmlHeader("Configuration"),
		result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				 "<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
				 "<LEDSpicer\n\tversion=\"1.1\"\n\ttype=\"Configuration\"\n>\n";

	EXPECT_EQ(header, result);
}

TEST(XMLHelperTest, XmlHeaderWithAttrs) {

	const string
		header = XMLHelper::xmlHeader("Input", {{NAME, "Mame"}}),
		result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				 "<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
				 "<LEDSpicer\n\t\tversion=\"1.1\"\n\t\ttype=\"Input\"\n\t\tname=\"Mame\"\n>\n";

	EXPECT_EQ(header, result);
}

TEST(XMLHelperTest, XmlSection) {

	const string
		content = Defaults::tab() + "<item/>\n",
		result  = XMLHelper::xmlSection("items", content);
	EXPECT_EQ("\t\t<items>\n\t\t<item/>\n\t\t</items>\n", result);
}

TEST(XMLHelperTest, XmlSectionEmptySkipped) {
	EXPECT_TRUE(XMLHelper::xmlSection("items", "").empty());
}

TEST(XMLHelperTest, XmlSectionWithAttrs) {

	const string result = XMLHelper::xmlSection("maps", "<map/>\n", {{"source", "hw1"}});
	EXPECT_EQ("\t\t<maps source=\"hw1\">\n<map/>\n\t\t</maps>\n", result);
}

TEST(XMLHelperTest, XmlHeaderNoType) {

	const string
		header = XMLHelper::xmlHeader(""),
		result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				 "<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
				 "<LEDSpicer\n\t\t\tversion=\"1.1\"\n\t\t\ttype=\"\"\n>\n";
	EXPECT_EQ(header, result);
}

TEST(XMLHelperTest, XmlFooter) {
	EXPECT_EQ("</" PACKAGE_DATA_NAME ">\n", XMLHelper::xmlFooter());
}

TEST(XMLHelperTest, GetData) {

	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");
	ValueVector& data = helper.getData("devices");
	EXPECT_TRUE(data.empty()) << "Unpopulated data should return empty vector";
}

TEST(XMLHelperTest, CleanError) {

	const string
		// Valid error with full details
		rawError = "Unable to read the file /xxx/yyy/zzzz.xml Error=XML_ERROR_MISMATCHED_ELEMENT ErrorID=14 (0xe) Line number=369: XMLElement name=map",
		// Simple error with no extra details
		simpleError = "Unable to read the file /xxx/yyy/zzzz.xml",
		expected = "Unable to read the file /xxx/yyy/zzzz.xml\nError: XML_ERROR_MISMATCHED_ELEMENT\nLine: 369\nNode: map",
		// run clean.
		result1  = XMLHelper::cleanError(rawError),
		result2  = XMLHelper::cleanError(simpleError);

	EXPECT_EQ(result1, expected) << "Full error should be cleaned correctly";
	EXPECT_EQ(result2, simpleError) << "Simple error should return unchanged";
}

TEST(XMLHelperTest, GetDataMap) {

	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");
	const DataMap& dataMap = helper.getDataMap();
	EXPECT_TRUE(dataMap.empty()) << "Unpopulated data map should be empty";
}
