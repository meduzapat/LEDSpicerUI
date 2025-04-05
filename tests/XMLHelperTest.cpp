#include <gtest/gtest.h>
#include <fstream>
#include "XMLHelper.hpp"

using namespace LEDSpicerUI;

// Constructor tests
TEST(XMLHelperTest, ConstructorErrorHandling) {
	GTEST_LOG_(INFO) << "Loading " PACKAGE_SAMPLES_DIR << "data/config.xml";
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml",    "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/input.xml",     "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/animation.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/profile.xml",   "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/malformed.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidLedspicerNode.xml", "InvalidType"), Message);
	EXPECT_THROW(XMLHelper helper(PACKAGE_SAMPLES_DIR "data/invalidDataVersion.xml",   "InvalidType"), Message);
}

TEST(XMLHelperTest, ProcessNodeReturnsCorrectMap) {
	// Arrange
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	// Act
	auto result = XMLHelper::processNode(helper.getRoot());

	// Assert
	EXPECT_EQ(result.size(), 12) << "Maps have different sizes";
}

TEST(XMLHelperTest, ProcessNodeByName) {
	// Success case - test 'layout' node under root
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");
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
	EXPECT_STREQ(root->Name(), "LEDSpicer") << "Root node name should be LEDSpicer";
}

TEST(XMLHelperTest, CheckAttributes) {
	// Success case - all attributes present
	StringVector attributeList = {"name", "type", "id"};
	StringUMap subjects = {
		{"name",  "Test"},
		{"type",  "Button"},
		{"id",    "1"},
		{"extra", "ignored"} // Ignored per spec
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

	// Present key
	EXPECT_EQ(XMLHelper::valueOf(values, "name"), "Test") << "Should return existing value";

	// Missing key, default empty
	EXPECT_EQ(XMLHelper::valueOf(values, "id"), "") << "Should return empty string for missing key";

	// Missing key, custom default
	EXPECT_EQ(XMLHelper::valueOf(values, "id", "defaultId"), "defaultId") << "Should return custom default for missing key";
}

TEST(XMLHelperTest, ToXML) {
	// Basic case - map with attributes
	StringUMap values = {
		{"name", "Test"},
		{"type", "Button"},
		{"id", "1"}
	};
	string expected = "id=\"1\"\ntype=\"Button\"\nname=\"Test\"\n";
	string result   = XMLHelper::toXML(values);
	EXPECT_EQ(result, expected) << "XML string should match expected format";

	// Empty map
	StringUMap emptyValues;
	EXPECT_EQ(XMLHelper::toXML(emptyValues), "") << "Empty map should return empty string";
}

TEST(XMLHelperTest, GetData) {
	XMLHelper helper(PACKAGE_SAMPLES_DIR "data/config.xml", "Configuration");

	// Empty case - unpopulated data
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
