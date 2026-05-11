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

#include "MockBasicData.hpp"
#include "Storage/CollectionHandler.hpp"

using LEDSpicerUI::Test::Mocks::MockBasicData;

// A simple Data subclass without a collection handler.
class TestData : public MockBasicData {

public:

	using MockBasicData::MockBasicData;

protected:

	bool shouldSerialize(const string& key, const string&) const noexcept override {
		return key != "ignored";
	}
};

// A Data subclass that registers into collection "t".
class RegisteringData : public MockBasicData {

public:

	using MockBasicData::MockBasicData;

	CollectionHandler* getCollectionHandler() const noexcept override {return CollectionHandler::getInstance("t");}
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

TEST_F(DataTest, Constructors) {

	// StringUMap got moved
	StringUMap map{{"name", "TestItem"}, {"type", "button"}, {"value", "42"}};
	MockBasicData d1(map);
	// Original map was moved.
	EXPECT_TRUE(map.empty());
	// New got data.
	EXPECT_FALSE(d1.getValues().empty());

	// Values was moved.
	Values values1{{"key1", "val1"}, {"key2", "val2"}};
	MockBasicData d2(values1);
	// Original map was moved.
	EXPECT_TRUE(values1.getValues().empty());
	// New got data.
	EXPECT_FALSE(d2.getValues().empty());

	// 3. Construction from Values&& (move - destructive)
	Values values2{{"move1", "movedValue"}, {"move2", "another"}};
	MockBasicData d3(std::move(values2));
	// Original map was moved.
	EXPECT_TRUE(values2.getValues().empty());
	// New got data.
	EXPECT_FALSE(d3.getValues().empty());

	// 4. Move construction between Data objects
	MockBasicData d4(std::move(d3));
	// Original map was moved.
	EXPECT_TRUE(d3.getValues().empty());
	// New got data.
	EXPECT_FALSE(d4.getValues().empty());

	// Move assignment
	MockBasicData d5;
	d5 = std::move(d4);
	// Original map was moved.
	EXPECT_TRUE(d4.getValues().empty());
	// New got data.
	EXPECT_FALSE(d5.getValues().empty());
}

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
	// Note: escape tabs and end lines to get the expected results.
	const string
		xml(data->toXML()),
		// ignored should not be set.
		expected {"<testTag\n\tname=\"TestItem\"\n\ttype=\"button\"\n\tvalue=\"42\"\n/>\n"};
	EXPECT_EQ(expected, xml);
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

TEST_F(DataTest, CollectionRegistration) {

	auto ch {CollectionHandler::getInstance("t")};

	RegisteringData item{{"name", "Alpha"}};

	// Register adds the item.
	item.registerToCollection();
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_TRUE(ch->isIdSet("Alpha"));

	// Re-registering an already-registered item is a no-op.
	item.registerToCollection();
	EXPECT_EQ(1u, ch->getSize());

	// Unregister removes it.
	item.unregisterFromCollection();
	EXPECT_FALSE(ch->isSet(&item));

	// Unregistering again is safe.
	EXPECT_NO_FATAL_FAILURE(item.unregisterFromCollection());

	// syncRegistration re-keys the entry when the primary key changes.
	item.setValue("name", "Beta");
	item.registerToCollection();
	string oldId{item.createUniqueId()};
	item.setValue("name", "Gamma");

	item.syncRegistration(oldId);
	EXPECT_FALSE(ch->isIdSet("Beta"));
	EXPECT_TRUE(ch->isIdSet("Gamma"));

	// syncRegistration is a no-op when the ID has not changed.
	item.syncRegistration(item.createUniqueId());
	EXPECT_TRUE(ch->isIdSet("Gamma"));
	EXPECT_EQ(1u, ch->getSize());

	// syncRegistration with an empty old ID acts as a plain registration.
	item.unregisterFromCollection();
	item.setValue("name", "Delta");
	item.syncRegistration("");
	EXPECT_TRUE(ch->isIdSet("Delta"));
}
