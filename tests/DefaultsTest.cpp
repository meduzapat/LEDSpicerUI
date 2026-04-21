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

TEST(DefaultsTest, IsNumber) {
	EXPECT_TRUE(Defaults::isNumber("123"));
	EXPECT_TRUE(Defaults::isNumber("0"));
	EXPECT_FALSE(Defaults::isNumber("12a3"));
	EXPECT_FALSE(Defaults::isNumber("12.3"));
	EXPECT_FALSE(Defaults::isNumber(""));
	EXPECT_FALSE(Defaults::isNumber("abc"));
}

TEST(DefaultsTest, IsBetween) {
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

TEST(DefaultsTest, AddUnitSeparator) {
	EXPECT_EQ(Defaults::addUnitSeparator("test"), UNIT_SEPARATOR + string("test") + UNIT_SEPARATOR);
	EXPECT_EQ(Defaults::addUnitSeparator(emptyString), UNIT_SEPARATOR + emptyString + UNIT_SEPARATOR);
	EXPECT_EQ(Defaults::addUnitSeparator("a b"), UNIT_SEPARATOR + string("a b") + UNIT_SEPARATOR);
}

TEST(DefaultsTest, Explode) {
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

	// Spaces trimmed limit
	auto result6 = Defaults::explode("  a  , b ,  c  ", ',', 2);
	EXPECT_EQ(result6.size(), 2);
	EXPECT_EQ(result6[0], "a");
	EXPECT_EQ(result6[1], "b ,  c");
}

TEST(DefaultsTest, ImplodeChar) {
	StringVector values{"a", "b", "c"};
	EXPECT_EQ(Defaults::implode(values, ','), "a,b,c");

	StringVector single{"item"};
	EXPECT_EQ(Defaults::implode(single, ','), "item");

	StringVector empty;
	EXPECT_EQ(Defaults::implode(empty, ','), "");

	StringVector spaced{" a ", " b ", " c "};
	EXPECT_EQ(Defaults::implode(spaced, ','), " a , b , c ");
}

TEST(DefaultsTest, ImplodeString) {
	StringVector values{"x", "y", "z"};
	EXPECT_EQ(Defaults::implode(values, "---"), "x---y---z");

	StringVector single{"test"};
	EXPECT_EQ(Defaults::implode(single, "---"), "test");

	StringVector empty;
	EXPECT_EQ(Defaults::implode(empty, "---"), "");
}

TEST(DefaultsTest, ImplodeUnorderedSet) {
	StringUSet values{"one", "two", "three"};
	string result = Defaults::implode(values, ',');

	// Use explode to split result back into a vector
	StringVector exploded(Defaults::explode(result, ','));
	StringUSet resultSet(exploded.begin(), exploded.end());

	EXPECT_EQ(resultSet, values);

	StringUSet single{"only"};
	EXPECT_EQ(Defaults::implode(single, ','), "only");

	StringUSet empty;
	EXPECT_EQ(Defaults::implode(empty, ','), "");
}

TEST(DefaultsTest, LTrim) {
	string s1 = "   text";
	Defaults::ltrim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::ltrim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::ltrim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::ltrim(s4);
	EXPECT_EQ(s4, "");
}

TEST(DefaultsTest, RTrim) {
	string s1 = "text   ";
	Defaults::rtrim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::rtrim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::rtrim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::rtrim(s4);
	EXPECT_EQ(s4, "");

	string s5 = " 1";
	Defaults::rtrim(s5);
	EXPECT_EQ(s5, " 1");

	string s6 = "  1 ";
	Defaults::rtrim(s6);
	EXPECT_EQ(s6, "  1");
}

TEST(DefaultsTest, Trim) {
	string s1 = "   text   ";
	Defaults::trim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::trim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::trim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::trim(s4);
	EXPECT_EQ(s4, "");
}

TEST(DefaultsTest, CreateCommonUniqueId) {
	StringVector fields{"field1", "field2", "field3"};
	string expected = string("field1") + FIELD_SEPARATOR + "field2" + FIELD_SEPARATOR + "field3";
	EXPECT_EQ(Defaults::createCommonUniqueId(fields), expected);

	StringVector single{"single"};
	EXPECT_EQ(Defaults::createCommonUniqueId(single), "single");

	StringVector empty;
	EXPECT_EQ(Defaults::createCommonUniqueId(empty), "");
}

TEST(DefaultsTest, ExtractName) {

	// Common Test cases.
	EXPECT_EQ("file",   Defaults::extractName("file",              "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("file.xml",          "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("/a/b/c/file.xml",   "/a/b/c"));
	EXPECT_EQ("d/file", Defaults::extractName("/a/b/c/d/file.xml", "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("/a1/b1/file.xml",   "/a/b/c"));
	// Test with deeper directory structures.
	EXPECT_EQ("d/e/f/file", Defaults::extractName("/a/b/c/d/e/f/file.xml", "/a/b/c"));
	EXPECT_EQ("file",       Defaults::extractName("/a/b/c/file",           "/a/b/c"));
	// Nasty corner cases.
	EXPECT_EQ("a/file",  Defaults::extractName("/a/file.xml",        ""));
	EXPECT_EQ("",        Defaults::extractName("",                   "/a/b/c"));
	EXPECT_EQ("file",    Defaults::extractName("/a/b/c/file.xml",    "/a/b/c/d/e/f"));
	EXPECT_EQ("file.v1", Defaults::extractName("/a/b/c/file.v1.xml", "/a/b/c"));
	EXPECT_EQ("file",    Defaults::extractName("/a/b/c/file.xml",    "/a/b/c/file.xml"));
}
