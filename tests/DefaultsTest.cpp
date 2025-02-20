/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DefaultsTest.cpp
 * @since     Feb 19, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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
#include "Defaults.hpp"

using namespace LEDSpicerUI;

// Test fixture for Defaults class (optional, used if setup/teardown is needed)
class DefaultsTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Reset static state if necessary
		Defaults::cleanDirty();
		Defaults::setIgnoreChanges(false);
		Defaults::tabs.clear();
	}
};

// Test isNumber
TEST_F(DefaultsTest, IsNumber) {
	EXPECT_TRUE(Defaults::isNumber("123"));
	EXPECT_TRUE(Defaults::isNumber("0"));
	EXPECT_FALSE(Defaults::isNumber("12a3"));
	EXPECT_FALSE(Defaults::isNumber("12.3"));
	EXPECT_FALSE(Defaults::isNumber(""));
	EXPECT_FALSE(Defaults::isNumber("abc"));
}

// Test isBetween
TEST_F(DefaultsTest, IsBetween) {
	EXPECT_TRUE(Defaults::isBetween("5", 1, 10));
	EXPECT_TRUE(Defaults::isBetween("1", 1, 10));
	EXPECT_TRUE(Defaults::isBetween("10", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("0", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("11", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("abc", 1, 10));
	// Test with high = -1 (no upper bound)
	EXPECT_TRUE(Defaults::isBetween("5", 1, -1));
	EXPECT_FALSE(Defaults::isBetween("0", 1, -1));
}

// Test addUnitSeparator
TEST_F(DefaultsTest, AddUnitSeparator) {
	EXPECT_EQ(Defaults::addUnitSeparator("test"), "\x20test\x20");
	EXPECT_EQ(Defaults::addUnitSeparator(""), "\x20\x20");
	EXPECT_EQ(Defaults::addUnitSeparator("a b"), "\x20a b\x20");
}

// Test explode
TEST_F(DefaultsTest, Explode) {
	// Basic delimiter
	auto result1 = Defaults::explode("a,b,c", ',');
	EXPECT_EQ(result1.size(), 3);
	EXPECT_EQ(result1[0], "a");
	EXPECT_EQ(result1[1], "b");
	EXPECT_EQ(result1[2], "c");

	// With limit
	auto result2 = Defaults::explode("1,2,3,4", ',', 2);
	EXPECT_EQ(result2.size(), 2);
	EXPECT_EQ(result2[0], "1");
	EXPECT_EQ(result2[1], "2,3,4");

	// Empty string
	auto result3 = Defaults::explode("", ',');
	EXPECT_TRUE(result3.empty());

	// Single item
	auto result4 = Defaults::explode("item", ',');
	EXPECT_EQ(result4.size(), 1);
	EXPECT_EQ(result4[0], "item");

	// Spaces trimmed
	auto result5 = Defaults::explode("  a  , b ,  c  ", ',');
	EXPECT_EQ(result5.size(), 3);
	EXPECT_EQ(result5[0], "a");
	EXPECT_EQ(result5[1], "b");
	EXPECT_EQ(result5[2], "c");
}

// Test implode (char delimiter)
TEST_F(DefaultsTest, ImplodeChar) {
	std::vector<std::string> values{"a", "b", "c"};
	EXPECT_EQ(Defaults::implode(values, ','), "a,b,c");

	std::vector<std::string> single{"item"};
	EXPECT_EQ(Defaults::implode(single, ','), "item");

	std::vector<std::string> empty;
	EXPECT_EQ(Defaults::implode(empty, ','), "");

	std::vector<std::string> spaced{" a ", " b ", " c "};
	EXPECT_EQ(Defaults::implode(spaced, ','), " a , b , c ");
}

// Test implode (string delimiter)
TEST_F(DefaultsTest, ImplodeString) {
	std::vector<std::string> values{"x", "y", "z"};
	EXPECT_EQ(Defaults::implode(values, "---"), "x---y---z");

	std::vector<std::string> single{"test"};
	EXPECT_EQ(Defaults::implode(single, "---"), "test");

	std::vector<std::string> empty;
	EXPECT_EQ(Defaults::implode(empty, "---"), "");
}

// Test ltrim
TEST_F(DefaultsTest, LTrim) {
	std::string s1 = "   text";
	Defaults::ltrim(s1);
	EXPECT_EQ(s1, "text");

	std::string s2 = "text";
	Defaults::ltrim(s2);
	EXPECT_EQ(s2, "text");

	std::string s3 = "   ";
	Defaults::ltrim(s3);
	EXPECT_EQ(s3, "");

	std::string s4 = "";
	Defaults::ltrim(s4);
	EXPECT_EQ(s4, "");
}

// Test rtrim
TEST_F(DefaultsTest, RTrim) {
	std::string s1 = "text   ";
	Defaults::rtrim(s1);
	EXPECT_EQ(s1, "text");

	std::string s2 = "text";
	Defaults::rtrim(s2);
	EXPECT_EQ(s2, "text");

	std::string s3 = "   ";
	Defaults::rtrim(s3);
	EXPECT_EQ(s3, "");

	std::string s4 = "";
	Defaults::rtrim(s4);
	EXPECT_EQ(s4, "");
}

// Test trim
TEST_F(DefaultsTest, Trim) {
	std::string s1 = "   text   ";
	Defaults::trim(s1);
	EXPECT_EQ(s1, "text");

	std::string s2 = "text";
	Defaults::trim(s2);
	EXPECT_EQ(s2, "text");

	std::string s3 = "   ";
	Defaults::trim(s3);
	EXPECT_EQ(s3, "");

	std::string s4 = "";
	Defaults::trim(s4);
	EXPECT_EQ(s4, "");
}

// Test createCommonUniqueId
TEST_F(DefaultsTest, CreateCommonUniqueId) {
	std::vector<std::string> fields{"field1", "field2", "field3"};
	EXPECT_EQ(Defaults::createCommonUniqueId(fields), "field1\x1Efield2\x1Efield3");

	std::vector<std::string> single{"single"};
	EXPECT_EQ(Defaults::createCommonUniqueId(single), "single");

	std::vector<std::string> empty;
	EXPECT_EQ(Defaults::createCommonUniqueId(empty), "");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
