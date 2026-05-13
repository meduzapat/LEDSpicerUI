/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ValuesTest.cpp
 * @since     Apr 6, 2026
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
#include "Values.hpp"

using namespace LEDSpicerUI;

class ValuesTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap d{{"name", "TestItem"}, {"type", "button"}, {"value", "42"}, {"ignored", "yes"}, {"boolean", HUMAN_TRUE}};
		values = std::make_unique<Values>(d);
	}

	void TearDown() override {
		values.reset();
	}

	std::unique_ptr<Values> values;
};

TEST_F(ValuesTest, Constructors) {
	// Construction from StringUMap (move)
	StringUMap map1{{"key1", "value1"}, {"key2", "value2"}};
	Values v1(map1);
	EXPECT_EQ("value1", v1.getValue("key1"));
	EXPECT_EQ("value2", v1.getValue("key2"));
	// original map should be moved-from
	EXPECT_TRUE(map1.empty());

	// Move construction
	Values v2(std::move(v1));
	EXPECT_EQ("value1", v2.getValue("key1"));
	EXPECT_EQ("value2", v2.getValue("key2"));
	EXPECT_EQ(0,        v1.getSize());

	// Copy constructor from iterators
	StringUMap map2{{"key1", "value1"}, {"key2", "value2"}};
	Values v3(map2.begin(), map2.end());
	EXPECT_EQ("value1", v3.getValue("key1"));
	EXPECT_EQ("value2", v3.getValue("key2"));

	// Initializer list
	Values v4 {{"key3", "value3"}};
	EXPECT_EQ("value3", v4.getValue("key3"));

	// Move assignment
	v4 = std::move(v3);
	EXPECT_EQ("value1", v4.getValue("key1"));
	EXPECT_EQ("value2", v4.getValue("key2"));
	// should not have old data
	EXPECT_EQ(0, v3.getSize());

	// Move from other.
	Values v5 {v4};
	EXPECT_EQ("value1", v5.getValue("key1"));
	EXPECT_EQ("value2", v5.getValue("key2"));
	// should not have old data
	EXPECT_EQ(0, v4.getSize());
}

TEST_F(ValuesTest, GetSetValue) {

	EXPECT_EQ("TestItem", values->getValue("name"));
	EXPECT_EQ("",         values->getValue("nonexistent"));
	EXPECT_EQ("default",  values->getValue("nonexistent", "default"));
	EXPECT_EQ("yes",      values->getValue("ignored"));

	values->setValue("key", "value");
	EXPECT_EQ("value",   values->getValue("key"));
	values->setValue("key", "NewName");
	EXPECT_EQ("NewName", values->getValue("key"));
}

TEST_F(ValuesTest, UnSet) {
	values->unSet("name");
	EXPECT_EQ("", values->getValue("name"));
	EXPECT_EQ(0,  values->getValues().count("name"));
}

TEST_F(ValuesTest, Wipe) {
	values->wipe();
	EXPECT_TRUE(values->getValues().empty());
}

TEST_F(ValuesTest, GetValues) {
	EXPECT_EQ(5, values->getSize());
}

TEST_F(ValuesTest, is) {
	EXPECT_TRUE(values->is("boolean"));
	EXPECT_FALSE(values->is("type"));
}

TEST_F(ValuesTest, isA) {
	EXPECT_TRUE(values->isA("name", "TestItem"));
}

TEST_F(ValuesTest, setTrue) {
	EXPECT_FALSE(values->is("new"));
	values->setTrue("new");
	EXPECT_TRUE(values->is("new"));
	values->unSet("new");
	EXPECT_FALSE(values->is("new"));
}

TEST_F(ValuesTest, Swap) {
	StringUMap other{{"a", "1"}, {"b", "2"}};
	Values v2(other);
	values->swap(v2);
	EXPECT_EQ("1",        values->getValue("a"));
	EXPECT_EQ("2",        values->getValue("b"));
	EXPECT_EQ("TestItem", v2.getValue("name"));
	EXPECT_EQ("42",       v2.getValue("value"));
}
