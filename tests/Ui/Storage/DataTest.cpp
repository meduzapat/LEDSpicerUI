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

#include "MockData.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Test::Mocks;
using LEDSpicerUI::Values;

// A simple Data subclass without a collection handler.
class TestData : public MockBasicData {

public:

	using MockBasicData::MockBasicData;

protected:

	bool shouldSerialize(const string& key, const string&) const noexcept override {
		return key != "ignored";
	}
};

class DataTest : public ::testing::Test {

protected:

	void SetUp() override {
		Values d {
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

	// StringUMap moved on construction.
	StringUMap map {{"name", "TestItem"}, {"type", "button"}, {"value", "42"}};
	MockBasicData d1 {map};
	EXPECT_TRUE(map.empty());
	EXPECT_TRUE(d1.getSize());

	// Values lvalue moved on construction.
	Values values1 {{"key1", "val1"}, {"key2", "val2"}};
	MockBasicData d2 {values1};
	EXPECT_EQ(0u, values1.getSize());
	EXPECT_TRUE(d2.getSize());

	// Values rvalue move construction.
	Values values2 {{"move1", "movedValue"}, {"move2", "another"}};
	MockBasicData d3(std::move(values2));
	EXPECT_TRUE(values2.getValues().empty());
	EXPECT_FALSE(d3.getValues().empty());

	// Data move construction.
	MockBasicData d4(std::move(d3));
	EXPECT_TRUE(d3.getValues().empty());
	EXPECT_FALSE(d4.getValues().empty());

	// Data move assignment.
	MockBasicData d5;
	d5 = std::move(d4);
	EXPECT_TRUE(d4.getValues().empty());
	EXPECT_FALSE(d5.getValues().empty());

}

TEST_F(DataTest, ValuesLifecycle) {

	// getSize counts all stored values.
	EXPECT_EQ(4, data->getSize());

	// getValue: existing, missing, missing with default.
	EXPECT_EQ("TestItem", data->getValue("name"));
	EXPECT_EQ("",         data->getValue("nonexistent"));
	EXPECT_EQ(DEFAULT,    data->getValue("nonexistent", DEFAULT));

	// setValue: set then overwrite.
	data->setValue("key", "value");
	EXPECT_EQ("value",   data->getValue("key"));
	data->setValue("key", "updated");
	EXPECT_EQ("updated", data->getValue("key"));

}

TEST_F(DataTest, PropertyLifecycle) {

	// Set, read, default for missing.
	data->getProperties().setValue("k1", "v1");
	data->getProperties().setValue("k2", "v2");
	EXPECT_EQ("v1",    data->getProperties().getValue("k1"));
	EXPECT_EQ("v2",    data->getProperties().getValue("k2"));
	EXPECT_EQ(DEFAULT, data->getProperties().getValue("missing", DEFAULT));
	EXPECT_TRUE(data->getProperties().isSet("k1"));
	EXPECT_FALSE(data->getProperties().isSet("missing"));

	// Overwrite.
	data->getProperties().setValue("k1", "updated");
	EXPECT_EQ("updated", data->getProperties().getValue("k1"));

	// Remove.
	data->getProperties().unSet("k1");
	EXPECT_FALSE(data->getProperties().isSet("k1"));

}

TEST_F(DataTest, IdentityAndOutput) {

	// createPrettyName and createUniqueId return primary value.
	EXPECT_EQ("TestItem", data->createPrettyName());
	EXPECT_EQ("TestItem", data->createUniqueId());

	// getPrimaryValue aliases the primary key's value.
	EXPECT_EQ("TestItem", data->getPrimaryValue());

	// createTooltip returns empty string by default.
	EXPECT_EQ(emptyString, data->createTooltip());

	// operator==: same object true, different object false.
	EXPECT_TRUE(*data == *data);
	Values other {{"name", "Other"}};
	TestData otherData {other};
	EXPECT_FALSE(*data == otherData);

}

TEST_F(DataTest, ToXML) {

	// shouldSerialize filters "ignored"; remaining fields emitted as attributes.
	const string xml(data->toXML());
	EXPECT_EQ("<testTag\n\tname=\"TestItem\"\n\ttype=\"button\"\n\tvalue=\"42\"\n/>\n", xml);

}

TEST_F(DataTest, UnSetLifecycle) {

	Values d {{"name", "TestPrimary"}, {"extra", "value"}};
	MockData item {d};
	auto ch {item.getCollectionHandler()};
	ch->add(&item);

	// Non-primary unset — stays registered.
	item.unSet("extra");
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_EQ("", item.getValue("extra"));
	EXPECT_EQ(0,  item.getValues().count("extra"));

	// Primary key unset — unregisters.
	item.unSet("name");
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_EQ("", item.getValue("name"));
	EXPECT_EQ(0,  item.getValues().count("name"));

}

TEST_F(DataTest, WipeLifecycle) {

	// Wipe registered item removes from collection.
	Values d;
	MockData item {d};
	auto ch {item.getCollectionHandler()};
	item.setValue("name", "MyItem");
	ch->add(&item);
	item.wipe();
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_TRUE(item.getValues().empty());

	// Wipe unregistered item is a safe no-op.
	MockData ghost {d};
	ghost.setValue("name", "Ghost");
	EXPECT_NO_FATAL_FAILURE(ghost.wipe());
	EXPECT_TRUE(ghost.getValues().empty());

}

TEST_F(DataTest, CopyValuesLifecycle) {

	// Without collection handler returns empty.
	EXPECT_EQ(0u, data->copyValues().getSize());

	// With collection handler produces a non-colliding ID.
	Values d;
	MockData item {d};
	auto ch {item.getCollectionHandler()};
	item.setValue("name", "Item");
	ch->add(&item);
	auto copy {item.copyValues()};
	ASSERT_FALSE(copy.getSize() == 0);
	EXPECT_FALSE(ch->isIdSet(copy.getValue("name")));
	EXPECT_EQ("Item", item.getValue("name"));

}

TEST_F(DataTest, CollectionRegistration) {

	MockData item {{"name", "Alpha"}};
	auto ch {item.getCollectionHandler()};

	// Register adds; re-registering is a no-op.
	item.registerToCollection();
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_TRUE(ch->isIdSet("Alpha"));
	item.registerToCollection();
	EXPECT_EQ(1u, ch->getSize());

	// Unregister removes; unregistering again is safe.
	item.unregisterFromCollection();
	EXPECT_FALSE(ch->isSet(&item));
	EXPECT_NO_FATAL_FAILURE(item.unregisterFromCollection());

	// syncRegistration re-keys on primary key change.
	item.setValue("name", "Beta");
	item.registerToCollection();
	string oldId {item.createUniqueId()};
	item.setValue("name", "Gamma");
	item.syncRegistration(oldId);
	EXPECT_FALSE(ch->isIdSet("Beta"));
	EXPECT_TRUE(ch->isIdSet("Gamma"));

	// syncRegistration is a no-op when ID unchanged.
	item.syncRegistration(item.createUniqueId());
	EXPECT_TRUE(ch->isIdSet("Gamma"));
	EXPECT_EQ(1u, ch->getSize());

	// syncRegistration with empty old ID acts as plain registration.
	item.unregisterFromCollection();
	item.setValue("name", "Delta");
	item.syncRegistration("");
	EXPECT_TRUE(ch->isIdSet("Delta"));

}
