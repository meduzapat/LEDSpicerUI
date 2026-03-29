/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TestData.cpp
 * @since     Mar 27, 2026
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
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;

// Does not self-register.
class TestData : public Data {
public:
	TestData(StringUMap& data) noexcept : Data(data, "") { ignored.insert("ignored"); }
	constexpr string_view getCssClass() const noexcept override { return "test-class"; }
};

// Self-registers into "t".
class RegisteringData : public Data {
public:
	RegisteringData(StringUMap& data) noexcept : Data(data, "t") {}
	constexpr string_view getCssClass() const noexcept override { return "reg"; }
};

class DataTest : public ::testing::Test {
protected:
	void SetUp() override {
		StringUMap d{{"name", "TestItem"}, {"type", "button"}, {"value", "42"}, {"ignored", "yes"}};
		data = std::make_unique<TestData>(d);
	}
	void TearDown() override {
		data.reset();
		CollectionHandler::purgeAll();
	}
	std::unique_ptr<TestData> data;
};

TEST_F(DataTest, GetValue) {
	EXPECT_EQ("TestItem", data->getValue("name"));
	EXPECT_EQ("",         data->getValue("nonexistent"));
	EXPECT_EQ("default",  data->getValue("nonexistent", "default"));
	EXPECT_EQ("yes",      data->getValue("ignored"));
}

TEST_F(DataTest, SetValue) {
	data->setValue("key", "value");
	EXPECT_EQ("value",   data->getValue("key"));
	data->setValue("key", "NewName");
	EXPECT_EQ("NewName", data->getValue("key"));
}

TEST_F(DataTest, UnSet) {
	data->unSet("name");
	EXPECT_EQ("", data->getValue("name"));
	EXPECT_EQ(0,  data->getValues()->count("name"));
}

TEST_F(DataTest, Wipe) {
	data->wipe();
	EXPECT_TRUE(data->getValues()->empty());
}

TEST_F(DataTest, ToXML) {
	string expected{"value=\"42\"\ntype=\"button\"\nname=\"TestItem\"\n"};
	EXPECT_EQ(expected, data->toXML());
	data->unSet("value");
	expected = "type=\"button\"\nname=\"TestItem\"\n";
	EXPECT_EQ(expected, data->toXML());
}

TEST_F(DataTest, GetValues) {
	EXPECT_EQ(4, data->getValues()->size());
}

TEST_F(DataTest, CreatePrettyName) {
	EXPECT_EQ("TestItem", data->createPrettyName());
}

TEST_F(DataTest, CreateUniqueId) {
	EXPECT_EQ("TestItem", data->createUniqueId());
}

TEST_F(DataTest, PropertyOperations) {
	data->setProperty("k1", "v1");
	data->setProperty("k2", "v2");
	EXPECT_EQ("v1",      data->getProperty("k1"));
	EXPECT_EQ("v2",      data->getProperty("k2"));
	EXPECT_EQ("default", data->getProperty("missing", "default"));
	EXPECT_TRUE(data->hasProperty("k1"));
	EXPECT_FALSE(data->hasProperty("missing"));
}

TEST_F(DataTest, RemoveProperty) {
	data->setProperty("k", "v");
	data->removeProperty("k");
	EXPECT_FALSE(data->hasProperty("k"));
}

TEST_F(DataTest, OverwriteProperty) {
	data->setProperty("k", "original");
	data->setProperty("k", "updated");
	EXPECT_EQ("updated", data->getProperty("k"));
}

// copyValues returns empty when no collection — nothing to check uniqueness against.
TEST_F(DataTest, CopyValuesNoCollection) {
	EXPECT_TRUE(data->copyValues().empty());
}

// setValue on primary key registers, destructor unregisters.
TEST_F(DataTest, RegistrationLifecycle) {
	auto ch = CollectionHandler::getInstance("t");
	auto d {StringUMap{}};
	auto item = new RegisteringData(d);
	EXPECT_FALSE(ch->isSet(item));
	item->setValue("name", "MyItem");
	EXPECT_TRUE(ch->isSet(item));
	delete item;
	EXPECT_FALSE(ch->isIdSet("MyItem"));
}

// Primary key change re-keys the registration.
TEST_F(DataTest, SetValueReplacesOnPrimaryKeyChange) {
	auto ch = CollectionHandler::getInstance("t");
	auto d{StringUMap{}};
	RegisteringData item(d);
	item.setValue("name", "OldName");
	item.setValue("name", "NewName");
	EXPECT_FALSE(ch->isIdSet("OldName"));
	EXPECT_TRUE(ch->isIdSet("NewName"));
}

// wipe removes from collection and clears fields.
TEST_F(DataTest, WipeRemovesFromCollection) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "MyItem");
	item.wipe();
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_TRUE(item.getValues()->empty());
}

// copyValues finds non-colliding ID without touching the original.
TEST_F(DataTest, CopyValuesFindsUniqueId) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "Item");
	auto copy = item.copyValues();
	ASSERT_FALSE(copy.empty());
	EXPECT_FALSE(ch->isIdSet(copy.at("name")));
	EXPECT_EQ("Item", item.getValue("name"));
}
