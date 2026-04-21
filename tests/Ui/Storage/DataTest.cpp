/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataTest.cpp
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

// A simple Data subclass without a collection handler.
class TestData : public Data {

public:

	TestData(StringUMap& data) noexcept : Data(data) {}
	constexpr string_view getCssClass() const noexcept override { return "test-class"; }
	constexpr string_view getXmlTag()   const noexcept override { return "test"; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }

protected:

	bool shouldSerialize(const string& key, const string&) const noexcept override {
		return key != "ignored";
	}
};

// A Data subclass that registers into collection "t".
class RegisteringData : public Data {

public:

	RegisteringData(StringUMap& data) noexcept : Data(data) {}
	constexpr string_view getCssClass() const noexcept override { return "reg"; }
	constexpr string_view getXmlTag()   const noexcept override { return "test"; }
	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance("t");
	}
};

class DataTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap d{
			{"name",    "TestItem"},
			{"type",    "button"},
			{"value",   "42"},
			{"ignored", "yes"}
		};
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
	EXPECT_EQ("value", data->getValue("key"));
	data->setValue("key", "NewName");
	EXPECT_EQ("NewName", data->getValue("key"));
}

TEST_F(DataTest, UnSetPrimaryKeyRemovesFromCollection) {
	StringUMap d{{"name", "TestPrimary"}, {"extra", "value"}};
	RegisteringData item(d);
	auto ch = CollectionHandler::getInstance("t");
	ch->add(&item);

	// Non-primary key removal — stays registered.
	item.unSet("extra");
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_EQ("", item.getValue("extra"));
	EXPECT_EQ(0,  item.getValues().count("extra"));

	// Primary key removal — unregisters.
	item.unSet("name");
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_EQ("", item.getValue("name"));
	EXPECT_EQ(0,  item.getValues().count("name"));
}

TEST_F(DataTest, WipeRemovesFromCollection) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "MyItem");
	ch->add(&item);
	item.wipe();
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_TRUE(item.getValues().empty());
}

// Wipe on an item that was never registered is safe (no crash, no-op).
TEST_F(DataTest, WipeUnregisteredItemSafe) {
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "Ghost");
	EXPECT_NO_FATAL_FAILURE(item.wipe());
	EXPECT_TRUE(item.getValues().empty());
}

TEST_F(DataTest, ToXML) {
	string xml(data->toXML());
	EXPECT_NE(string::npos, xml.find("<test"));
	EXPECT_NE(string::npos, xml.find("name=\"TestItem\""));
	EXPECT_NE(string::npos, xml.find("value=\"42\""));
	EXPECT_EQ(string::npos, xml.find("ignored="));
}

TEST_F(DataTest, GetValues) {
	EXPECT_EQ(4, data->getValues().size());
}

TEST_F(DataTest, CreatePrettyName) {
	EXPECT_EQ("TestItem", data->createPrettyName());
}

TEST_F(DataTest, CreateUniqueId) {
	EXPECT_EQ("TestItem", data->createUniqueId());
}

TEST_F(DataTest, PropertyOperations) {
	data->getProperties().setValue("k1", "v1");
	data->getProperties().setValue("k2", "v2");
	EXPECT_EQ("v1",      data->getProperties().getValue("k1"));
	EXPECT_EQ("v2",      data->getProperties().getValue("k2"));
	EXPECT_EQ("default", data->getProperties().getValue("missing", "default"));
	EXPECT_TRUE(data->getProperties().isSet("k1"));
	EXPECT_FALSE(data->getProperties().isSet("missing"));
}

TEST_F(DataTest, RemoveProperty) {
	data->getProperties().setValue("k", "v");
	data->getProperties().unSet("k");
	EXPECT_FALSE(data->getProperties().isSet("k"));
}

TEST_F(DataTest, OverwriteProperty) {
	data->getProperties().setValue("k", "original");
	data->getProperties().setValue("k", "updated");
	EXPECT_EQ("updated", data->getProperties().getValue("k"));
}

// copyValues returns empty when there is no collection handler.
TEST_F(DataTest, CopyValuesNoCollection) {
	EXPECT_TRUE(data->copyValues().empty());
}

// copyValues produces a non-colliding ID and leaves the original untouched.
TEST_F(DataTest, CopyValuesFindsUniqueId) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "Item");
	ch->add(&item);
	auto copy = item.copyValues();
	ASSERT_FALSE(copy.empty());
	EXPECT_FALSE(ch->isIdSet(copy.at("name")));
	EXPECT_EQ("Item", item.getValue("name"));
}

// syncRegistration re-keys the handler entry when the primary key changes.
TEST_F(DataTest, SyncRegistrationReKeys) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "OldName");
	ch->add(&item);
	EXPECT_TRUE(ch->isIdSet("OldName"));

	string oldId{item.createUniqueId()};
	item.setValue("name", "NewName");
	item.syncRegistration(oldId);

	EXPECT_FALSE(ch->isIdSet("OldName"));
	EXPECT_TRUE(ch->isIdSet("NewName"));
}

// syncRegistration is a no-op when the ID has not changed.
TEST_F(DataTest, SyncRegistrationNoOpWhenSameId) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "Name");
	ch->add(&item);
	item.syncRegistration(item.createUniqueId());
	EXPECT_TRUE(ch->isIdSet("Name"));
}

// syncRegistration with an empty oldId acts as a plain registration.
TEST_F(DataTest, SyncRegistrationEmptyOldIdRegisters) {
	auto ch = CollectionHandler::getInstance("t");
	StringUMap d{};
	RegisteringData item(d);
	item.setValue("name", "Fresh");
	item.syncRegistration("");
	EXPECT_TRUE(ch->isIdSet("Fresh"));
}
